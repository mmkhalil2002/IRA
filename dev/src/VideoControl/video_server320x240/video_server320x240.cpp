#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>
//#include <arducam_mipicamera.h>
#include  <iostream>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>


#define NUM_CLIENTS 100
#define IMGSCALE_320x240  0x02 
#define IMGSCALE_160x120  0x01
#define ZOOM_FACTOR_X     16
#define ZOOM_FACTOR_Y     12

#define ZOOM_CMD         0x12
 
#define imag_width     320
#define imag_height    240

using namespace cv;
using namespace std;


sem_t semaphore, capture_video_semaphore;
bool running  = false;
int QUIT      = 0;
int capDev    = 0;
//VideoCapture cap(capDev);
VideoCapture cap;
Mat img, oldimg;
Mat record_img;
int numfds = 0;
int debug_level = 0;
bool client_status [NUM_CLIENTS];
unsigned char  greyscale          [imag_width*imag_height]; 
unsigned char  out_image          [imag_width*imag_height]; 
unsigned char  out_image_160x120  [imag_width*imag_height/4]; 
unsigned char  cout_image         [3*imag_width*imag_height]; 
unsigned char  cout_image_160x120 [3*imag_width*imag_height/4]; 
int   img_width  = 0;
int   img_height = 0;
float Brightness = 0.0;
float Contrast   = 0.0;  
float Saturation = 0.0;
float Gain       = 0.0;
int   zoom_x   = 0;
int   zoom_y   = 0;


int elmsize  = 1;
bool flip_img = false;
int video_port = 4097;
int ctr_port   = 4098;



unsigned char ImgScale = IMGSCALE_320x240;

void convert_to_byte (Mat frame,unsigned char *buffer,int width, int height){

	for (int j = 0; j < height; j++) {
    		for (int i = 0; i < width; i++) {
       		 uchar& uxy = frame.at<uchar>(j, i);
        	int color = (int) uxy;
		//cout << color << " ";
        	buffer[j * width + i] = color;
       		 }
		//cout << endl;
	}
}

void _print_frame (unsigned char *buffer,int w, int h){
	for (int j = 0; j < h; j++) {
    		for (int i = 0; i < w; i++) {
        	int color =  buffer[j * (unsigned int)w + i] ;
		cout << color << " ";
       		 }
		cout << endl;
	}
}

int bytesAvailable (int sockfd)
{
    long bytes = 0;

    if(ioctl(sockfd, FIONREAD, &bytes) < 0)
    {
        printf("ERROR: Network:bytesAvailable: ioctl() call failed.\n");
        return -1;
    }

    return bytes;
}

int sock_status(int sockfd)
{


    int error = 0;
    socklen_t len = sizeof (error);
    int retval = getsockopt (sockfd, SOL_SOCKET, SO_ERROR, &error, &len);
    if (retval != 0) {
	   /* there was a problem getting the error code */
    fprintf(stderr, "error getting socket error code: %s\n", strerror(retval));
	   return -1;
      }

     if (error != 0) {
      	/* socket has a non zero error status */
    	fprintf(stderr, "socket error: %s\n", strerror(error));
	return -1;
      }

    struct timeval tv;
    fd_set  fd;
    int result = 0;

    tv.tv_sec  = 5;
    tv.tv_usec = 0;

    FD_ZERO(&fd);
    FD_SET(sockfd, &fd);

    result = select(sockfd + 1, &fd, 0, 0, &tv);

    if(result && !bytesAvailable(sockfd))
    {
        std::cout << " disconnected " << std::endl;
        return -1; // disconnected, I'm guessing this is definitely WRONG.
    }
    else if(result > 0 && FD_ISSET(sockfd, &fd))
    {
        std::cout << " bytes available " << std::endl;
        return 0; // bytes available. successful
    }
    else if(!result)
    {
        std::cout << " timeout " << std::endl;
        return -1; // timeout
    }

     std::cout << " select call failed " << std::endl;
    return -1; // select() call failed.
}

void flip_compress_frame (unsigned char *in_frame,unsigned  char * out_frame)
{
	if (ImgScale == IMGSCALE_160x120)
	{
		std::cout << " image height " << img_height << " img width " << img_width << endl;
		std::flush (std::cout);
		for (int y = 0, cy=0;y < img_height;y+=2,cy++)
                  for (int  x = 0, cx=0; x < img_width;x+=2,cx++)
		  {
			  out_frame [cy*elmsize*img_width/2+cx+0] = in_frame[y*elmsize*img_width+(img_width-1-x)+0];
			  //out_frame [dl] = 0xf0;
			  //printf (" pixel %x  dl %d  sl %d\n",out_frame [dl], dl,sl);
			  //printf (" pixel %x  dl %d \n",out_frame [dl], dl);
			  //printf ("PIX %x imgw %d imgh %d  row %d col %d crow %d ccol %d  dl %d  sl %d\n",out_frame[dl], img_width,img_height,row,col,crow,ccol,dl,sl);
			  //printf ("imgw %d imgh %d  row %d col %d crow %d ccol %d  dl %d  sl %d\n",img_width,img_height,row,col,crow,ccol,dl,sl);
                  }
		//for (int i=0;i< greyimg.total()/4;i++)
		//	printf (" PIX %x \n",out_frame[i]);


	}
	else 
	{
		int cnt = 0;
		for (int y = 0; y < img_height;y++)
		{
        	  for (int  x = 0; x < img_width;x++)
                    {
	 		 out_frame [y*elmsize*img_width+x+0] = in_frame[y*elmsize*img_width+img_width-1-x+0];
                    }
                }
	}
}


void compress_frame (unsigned char *in_frame,unsigned  char * out_frame)
{
	if (ImgScale == IMGSCALE_160x120)
	{
		std::cout << " image height " << img_height << " img width " << img_width << endl;
		std::flush (std::cout);
		for (int y = 0, cy=0;y < img_height;y+=2,cy++)
                  for (int  x = 0, cx=0; x < img_width;x+=2,cx++)
		    {
					//if (cx < 3*cx+2  && x < 3*x+2)
			 out_frame [cy*elmsize*img_width/2+cx+0] = in_frame[y*elmsize*img_width+x+0];
		 		         //out_frame [cy*elmsize*img_width/2+3*cx+0] = 0xE0;
                    }
		//for (int i=0;i< greyimg.total()/4;i++)
		//	printf (" PIX %x \n",out_frame[i]);


	}
	else 
	{
		int cnt;
		for (int y = 0; y < img_height;y++)
		{
        	  for (int  x = 0; x < img_width;x++)
                    {
	 		 out_frame [y*elmsize*img_width+x+0] = in_frame[y*elmsize*img_width+x+0];
                    }
		}
	}
}

void flip_compress_color_frame (unsigned char *in_frame,unsigned  char * out_frame)
{
	
	try{
		if (ImgScale == IMGSCALE_160x120)
		{
			std::cout << " image height " << img_height << " img width " << img_width << endl;
			std::flush (std::cout);
			for (int y = 0, cy=0;y < img_height;y+=2,cy++)
	          		for (int  x = 0, cx=0; x < img_width;x+=2,cx++)
		  		{
					//if (cx < 3*cx+2  && x < 3*x+2)
			 		 out_frame [cy*elmsize*img_width/2+3*cx+0] = in_frame[y*elmsize*img_width+3*(img_width-1-x)+0];
			 		 out_frame [cy*elmsize*img_width/2+3*cx+1] = in_frame[y*elmsize*img_width+3*(img_width-1-x)+1];
			 		 out_frame [cy*elmsize*img_width/2+3*cx+2] = in_frame[y*elmsize*img_width+3*(img_width-1-x)+2];
		 		         //out_frame [cy*elmsize*img_width/2+3*cx+0] = 0xE0;
			 		 //out_frame [cy*elmsize*img_width/2+3*cx+1] = 0xE0;
			 		 //out_frame [cy*elmsize*img_width/2+3*cx+2] = 0xE0;
                  		}
				//for (int i=0;i< elmsize*img_width*img_height/4;i++)
					//printf (" index %d  PIX %x \n",i,out_frame[i]);


		}
		else 
       	  	{
			for (int y = 0; y < img_height;y++)
			{
	        	  for (int  x = 0; x < img_width;x++)
		  		{
			 	 out_frame [y*elmsize*img_width+3*x+0] = in_frame[y*elmsize*img_width+3*(img_width-1-x)+0];
			 	 out_frame [y*elmsize*img_width+3*x+1] = in_frame[y*elmsize*img_width+3*(img_width-1-x)+1];
			 	 out_frame [y*elmsize*img_width+3*x+2] = in_frame[y*elmsize*img_width+3*(img_width-1-x)+2];
                  		}
			}
	 	 }
        }
	catch(const std::exception& e)
	{
		std::cout << " compress_color_frame " <<  e.what() << std::endl;
	}
}


void compress_color_frame (unsigned char *in_frame,unsigned  char * out_frame)
{
	
	try{
		if ( ImgScale== IMGSCALE_160x120)
		{
			std::cout << " image height " << img_height << " img width " << img_width << endl;
			std::flush (std::cout);
			for (int y = 0, cy=0;y < img_height;y+=2,cy++)
	          		for (int  x = 0, cx=0; x < img_width;x+=2,cx++)
		  		{
					//if (cx < 3*cx+2  && x < 3*x+2)
					{
			 		 out_frame [cy*elmsize*img_width/2+3*cx+0] = in_frame[y*elmsize*img_width+3*x+0];
			 		 out_frame [cy*elmsize*img_width/2+3*cx+1] = in_frame[y*elmsize*img_width+3*x+1];
			 		 out_frame [cy*elmsize*img_width/2+3*cx+2] = in_frame[y*elmsize*img_width+3*x+2];
		 		         //out_frame [cy*elmsize*img_width/2+3*cx+0] = 0xE0;
			 		 //out_frame [cy*elmsize*img_width/2+3*cx+1] = 0xE0;
			 		 //out_frame [cy*elmsize*img_width/2+3*cx+2] = 0xE0;
					}
                  		}
				//for (int i=0;i< elmsize*img_width*img_height/4;i++)
					//printf (" index %d  PIX %x \n",i,out_frame[i]);

		}
		else 
       	  	{
			for (int y = 0; y < img_height;y++)
			{
	        	  for (int  x = 0; x < img_width;x++)
		  		{
			 		 out_frame [y*elmsize*img_width+3*x+0] = in_frame[y*elmsize*img_width+3*x+0];
			 		 out_frame [y*elmsize*img_width+3*x+1] = in_frame[y*elmsize*img_width+3*x+1];
			 		 out_frame [y*elmsize*img_width+3*x+2] = in_frame[y*elmsize*img_width+3*x+2];
                  		}
			}
	 	 }
        }
	catch(const std::exception& e)
	{
		std::cout << " compress_color_frame " <<  e.what() << std::endl;
	}
}


//
// flip the grey image
void convert_2_greyscale_flip (unsigned char *color, int img_width, int img_height, unsigned char *greyscale)
{
	for (int y=0;y< img_height;y++)
	{
		for (int x=0;x<img_width;x++)
		{
	          greyscale[y*img_width + img_width -1-x] = 
		        (color [3*(y)*img_width +3*x+0] + color [3*(y)*img_width+3*x+1] + color [3*(y)*img_width +3*x+2])/3-1;
		}
	}

}
//
// flip the grey image
void convert_2_greyscale (unsigned char *color, int img_width, int img_height, unsigned char *greyscale)
{
	for (int y=0;y< img_height;y++)
	{
		for (int x=0;x<img_width;x++)
		{
	          greyscale[y*img_width +x] = 
	               (color [3*(y)*img_width +3*x+0] + color [3*(y)*img_width+3*x+1] + color [3*(y)*img_width +3*x+2])/3-1;
		}
	}

}

//
//INTER_NEAREST - a nearest-neighbor interpolation
//INTER_LINEAR - a bilinear interpolation (used by default)
//INTER_AREA - resampling using pixel area relation. 
//  It may be a preferred method for image decimation, as it gives moire’-free results.
//  But when the image is zoomed, it is similar to the INTER_NEAREST method.
//INTER_CUBIC - a bicubic interpolation over 4x4 pixel neighborhood
//INTER_LANCZOS4 - a Lanczos interpolation over 8x8 pixel neighborhood
//
//
// Crop the full image to that image contained by the rectangle myROI
// Note that this doesn't copy the data
void ExtractROI_v2(cv::Mat& inImage, cv::Mat& outImage, cv::Rect roi){
	cv::Mat croppedRef(inImage, roi);
	// Copy the data into new matrix
	croppedRef.copyTo(outImage);
}



//
// this version is working too
//
void ExtractROI_v1(cv::Mat& inImage, cv::Mat& outImage, cv::Rect roi){
    /* Create the image */
    outImage = Mat(roi.height, roi.width, inImage.type(), Scalar(0));
    /* Populate the image */
   
    if (debug_level == 1)
        printf ("ExractROI_v1 width %d height %d x %d y %d \n",roi.width,roi.height,roi.x,roi.y);
    for (int i = roi.y; i < (roi.y+roi.height); i++){
        uchar* inP = inImage.ptr<uchar>(i);
        uchar* outP = outImage.ptr<uchar>(i-roi.y);
        for (int j = roi.x; j < (roi.x+roi.width); j++){
            outP[3*(j-roi.x)+0] = inP[3*j+0];
            outP[3*(j-roi.x)+1] = inP[3*j+1];
            outP[3*(j-roi.x)+2] = inP[3*j+2];
	    //printf (" y %d %d x %d  v %x \n",(i-roi.y)*roi.width,(i-roi.y),j-roi.x,outImage.data[(i-roi.y)*roi.width+(j-roi.x)]);
	    //printf (" y %d x %d  v %x \n",i-roi.y,j-roi.x,outP[j-roi.x]);
        }
    }
}

void Zoom (cv::Mat & inImage, int zoom_x,int zoom_y)
{
	int img_width =  inImage.cols;
	int img_height =  inImage.rows;
	cv::Rect roi;
    	roi.x = ZOOM_FACTOR_X*zoom_x;
    	roi.y = ZOOM_FACTOR_Y*zoom_y;
	if (((img_width-roi.x*2) > 0) && ((img_height-roi.y*2) > 0))
	{
    	   roi.width = img_width - (roi.x*2);
    	   roi.height = img_height - (roi.y*2);
	}
	cv::Mat  crop;
	/* Crop the original image to the defined ROI */
	//cv::Mat crop = img(roi);
	ExtractROI_v1(inImage, crop, roi);
	if (debug_level == 1)
	   printf ("crop img width %d  height %d \n",crop.cols,crop.rows);
	cv::resize(crop, inImage, cv::Size(img_width,img_height), 0.0, 0.0,cv::INTER_LINEAR);
	//cv::resize(crop, img, cv::Size(),img_width/crop.cols,img_height/crop.rows,cv::INTER_LINEAR);
}
      // Creating the Size object
//cv::Size size ;
void *capture_video(void *ptr)
{
	Mat dst;
	//
	//while (true)
	{
		try
		{
		    // your code here
			oldimg = img;
			try
			{
        		  cap >> img;
   			}
   			catch( cv::Exception& e )
   			{
         			const char* err_msg = e.what();
         			std::cout << "exception caught: " << err_msg << std::endl;
         			cout <<"wrong file format, please input the name of an IMAGE file" <<endl;
     				return NULL;
   			}	
                        //cv::subtract (img,oldimg,dst);
			//dst = cv::abs(dst);
			//cv::Scalar meanv = cv::mean(dst);
			img_width =  img.cols;
			img_height =  img.rows;
                        if (zoom_x != 0)
                            Zoom (img, zoom_x,zoom_y);

        		//bool bSuccess = cap.read(img); // read a new frame from video
        		//sem_wait(&capture_video_semaphore);
       			//cv::cvtColor(img, greyimg, cv::COLOR_BGR2GRAY); 
		        //if (elmsize == 1)
			//{
			   //convert_2_greyscale (img.data,img_width,img_height,greyscale);
			//}
        		//sem_post(&capture_video_semaphore);
			//usleep (100);
			//cout << " after capture img "<< endl;
		}
		catch(const std::exception& e)
		{
			std::cout << " <<<<  Excption try again " <<  e.what() << std::endl;
			exit (-1);
			//capture_video (NULL);
		}
	}
	//cout << " thread exit " << endl;
	return NULL;
}

void *send_color_data(void *ptr)
 {
    int socket = *(int *)ptr;
    int bytes;
    //OpenCV Code
    //----------------------------------------------------------
    //VideoCapture cap(capDev);
    try {
   	 while(client_status[socket])
   	{
       		 // get a frame from the camera
		//cout << "send frame status compress_type " << compress_type << " end compress type" << endl;
		//printf (" compress type %x \n", compress_type );
	
   		cout << "get colorimage " << endl;
      		capture_video(NULL);
		//cout << " after capture img in send_color_data"<< endl;

      		if (debug_level == 3)
      		{
			cout << "COLOR  STEP VALUE  " << img.step << endl;
			cout << "COLOR COL VALUE    " << img.cols << endl;
			cout << "COLOR ROWS VALUE   " << img.rows << endl;
       			cout << "COLOR ELEMENT SIZE "<< img.elemSize() << endl;
        		cout << "COLOR IMG TOTAL    "<< img.total() << endl;
        		cout << "COLOR IMG SIZE     " << img.total()*img.elemSize()  << endl;
			//cout << "GREY  STEP VALUE   " << greyimg.step << endl;
			//cout << "GREY  COL VALUE    " << greyimg.cols << endl;
			//cout << "GREY  ROWS VALUE   " << greyimg.rows << endl;
        		//cout << "GREY  ELEMENT SIZE "<< greyimg.elemSize() << endl;
        		//cout << "GREY  IMG TOTAL    "<< greyimg.total() << endl;
        		//cout << "GREY IMG SIZE      " << greyimg.total()*greyimg.elemSize() << endl;
      		}

        	// flip the frame
        	//flip(img, flippedFrame, 1);

        	// send the flipped frame over the network
       		//sem_wait(&capture_video_semaphore);
		//cout << " start send " << endl;
		unsigned char buf [4];

		buf [0] = 0xaf;
		buf [1] = 0xbf;
		buf [2] = 0xcf;
		buf [2] = 0xdf;

		std::cerr << " before send start"<< endl;
        	if ((bytes = send(socket, buf, 4, 0)) < 0)
		{
            		std::cerr << "bytes = " << bytes << std::endl;
	    		std::flush (std::cerr);
       	    		//sem_post(&capture_video_semaphore);
            		break;
        	}
        	std::flush (std::cerr);
       		//bytes = send(socket, greyimg.data, greyimg.total(), 0);
		//std::cerr << " after send start"<< endl;
		bytes = 0;
		int index = 0;

		if (ImgScale == IMGSCALE_160x120)
		{
		       int imgtotal = elmsize*img_width*img_height/4;
			if (flip_img)
   			   flip_compress_color_frame (img.data,cout_image_160x120);
			else
   			   compress_color_frame (img.data,cout_image_160x120);
			//while (bytes != greyimg.total()/4 && client_status[socket] == true)
			while (bytes != imgtotal && client_status[socket] == true)
	 		{
        			//if ((bytes += send(socket, &out_frame_2x2pix[bytes], greytotal/4-bytes, 0)) < 0)
        			if ((bytes += send(socket, &cout_image_160x120[bytes], imgtotal-bytes, 0)) < 0)
				{	
            				std::cerr << "error sending " << std::endl;
	       		         	std::flush (std::cerr);
                       		 	//sem_post(&capture_video_semaphore);  
            				break;
       			 	}	
				if (debug_level == 1)
				{
					std::cerr << "bytes sent " << bytes << " index " << ++ index << "  total size " << imgtotal/4; 
		        		std::cerr << " client_status[socket] " << client_status[socket] << endl;
	        			std::flush (std::cerr);
				}
			}	
		}
		else
		{
		        int imgtotal = elmsize*img_width*img_height;
			if (flip_img)
   			   flip_compress_color_frame (img.data,cout_image);
			else
   			   compress_color_frame (img.data,cout_image);


		        std::cerr << " before  send frame bytes "<< bytes << endl;
	       		while (bytes != imgtotal && client_status[socket] == true)
			{
        			if ((bytes += send(socket, &cout_image[bytes], imgtotal-bytes, 0)) < 0)
				{	
            				std::cerr << "error sending  imgtotal " << imgtotal << " offset " <<  bytes << std::endl;
	                		std::flush (std::cerr);
                        		//sem_post(&capture_video_semaphore);  
            				break;
       			 	}
				if (debug_level == 1)
				{
					std::cerr << "bytes sent " << bytes << " index " << ++ index << "  total size " << imgtotal;
					std::cerr << " client_status[socket] " << client_status[socket] << endl;
	        			std::flush (std::cerr);
				}
			}	
		        //std::cerr << " after  send frame bytes "<< bytes << endl;
		}
		//std::cerr << " before  send iend bytes "<< bytes << endl;
        	std::flush (std::cerr);
        	buf [0] = 0xfa;
        	buf [1] = 0xfb;
        	buf [2] = 0xfc;
        	buf [2] = 0xfd;
        	if ((bytes = send(socket, buf, 4, 0)) < 0)
		{
            		std::cerr << "bytes = " << bytes << std::endl;
	    		std::flush (std::cerr);
            		//sem_post(&capture_video_semaphore);  
            		break;
	    		//std::cerr << " after send bytes "<< bytes << endl;
	    		//std::flush (std::cerr);
			//sleep (60*60);
	 	}	

       	//sem_post(&capture_video_semaphore);
	//cout << "  send " << endl;
	//std::cerr << "after send frame " << endl;
        //std::flush (std::cerr);
	//cout << " end send color data " << endl;
    }

    std::cerr << "send quitting..." << std::endl;
    system ("shutdown -fr now");
    std::flush (std::cerr);
    close(socket);
    //client_status[socket] = true;
    //sem_post(&capture_video_semaphore);  
   }
    catch(const std::exception& e)
	{
		std::cout << " <<<<  Excption try again " <<  e.what() << std::endl;
	}
    return NULL;
 }


void *record_video(void *ptr)
{
	while (1)
	{
       		cap >> record_img;
		usleep (100000);
   		//cout << "record video " << endl;
	}
}

Mat background;
double threshld = 1000;
void motion_detection ()
{
	
	// convert the background frame to grayscale
	cv::cvtColor(background, background, COLOR_BGR2GRAY);

    // apply Gaussian blur to the background frame
	cv::GaussianBlur(background, background, Size(21, 21), 0);

    	cv::Mat frame;
    	frame = img;

        // convert the current frame to grayscale
    	cv::cvtColor(frame, frame, COLOR_BGR2GRAY);

        // apply Gaussian blur to the current frame
    	cv::GaussianBlur(frame, frame, Size(21, 21), 0);

       // compute the absolute difference between the background and the current frame
	cv::Mat diff;
	cv::absdiff(background, frame, diff);

        // threshold the difference image
	cv::Mat thresh;
	cv::threshold(diff, thresh, threshld, 255, THRESH_BINARY);

        // apply dilation and erosion to remove noise
	cv::Mat kernel = getStructuringElement(MORPH_RECT, Size(5, 5));
	cv::dilate(thresh, thresh, kernel);
	cv::erode(thresh, thresh, kernel);

        // find contours in the thresholded image
        vector<vector<Point>> contours;
	cv::findContours(thresh, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

        // iterate over the contours
        for (size_t i = 0; i < contours.size(); i++) {
            // compute the area of the contour
            double area = cv::contourArea(contours[i]);

	                // if the area is greater than the threshold, draw a bounding rectangle around the contour
            if (area > threshld) {
                Rect rect = cv::boundingRect(contours[i]);
		cv::rectangle(frame, rect, Scalar(0, 255, 0), 2);
            }
        }



}

void *send_data(void *ptr)
 {
    int socket = *(int *)ptr;
    int bytes;
    //OpenCV Code
    //----------------------------------------------------------
    //VideoCapture cap(capDev);
    try {
   	 while(client_status[socket])
   	{
       		 // get a frame from the camera
		//cout << "send frame status compress_type " << compress_type << " end compress type" << endl;
		//printf (" compress type %x \n", compress_type );
	
   		//cout << "get grey image " << endl;
      		capture_video(NULL);
       		sem_wait(&capture_video_semaphore);
        	convert_2_greyscale (img.data,img_width,img_height,greyscale);
         	sem_post(&capture_video_semaphore);
		usleep (100);
			//cout << " after capture img "<< endl;

      		if (debug_level == 3)
      		{
			cout << "COLOR  STEP VALUE  " << img.step << endl;
			cout << "COLOR COL VALUE    " << img.cols << endl;
			cout << "COLOR ROWS VALUE   " << img.rows << endl;
       			cout << "COLOR ELEMENT SIZE "<< img.elemSize() << endl;
        		cout << "COLOR IMG TOTAL    "<< img.total() << endl;
        		cout << "COLOR IMG SIZE     " << img.total()*img.elemSize()  << endl;
			//cout << "GREY  STEP VALUE   " << greyimg.step << endl;
			//cout << "GREY  COL VALUE    " << greyimg.cols << endl;
			//cout << "GREY  ROWS VALUE   " << greyimg.rows << endl;
        		//cout << "GREY  ELEMENT SIZE "<< greyimg.elemSize() << endl;
        		//cout << "GREY  IMG TOTAL    "<< greyimg.total() << endl;
        		//cout << "GREY IMG SIZE      " << greyimg.total()*greyimg.elemSize() << endl;
      		}

        	// flip the frame
        	//flip(img, flippedFrame, 1);

        	// send the flipped frame over the network
       		//sem_wait(&capture_video_semaphore);
		//cout << " start send " << endl;
		unsigned char buf [4];

		buf [0] = 0xaf;
		buf [1] = 0xbf;
		buf [2] = 0xcf;
		buf [2] = 0xdf;

		//std::cerr << " before send start"<< endl;
        	if ((bytes = send(socket, buf, 4, 0)) < 0)
		{
            		std::cerr << "bytes = " << bytes << std::endl;
	    		std::flush (std::cerr);
       	    		//sem_post(&capture_video_semaphore);
            		break;
        	}
        	std::flush (std::cerr);
       		//bytes = send(socket, greyimg.data, greyimg.total(), 0);
		bytes = 0;
		int index = 0;

		if (ImgScale == IMGSCALE_160x120)
		{
		       int imgtotal = elmsize*img_width*img_height/4;
			if (flip_img)
   			    flip_compress_frame (greyscale,out_image_160x120);
			else
   			    compress_frame (greyscale,out_image_160x120);
			//while (bytes != greyimg.total()/4 && client_status[socket] == true)
			while (bytes != imgtotal && client_status[socket] == true)
	 		{
        			//if ((bytes += send(socket, &out_frame_2x2pix[bytes], greytotal/4-bytes, 0)) < 0)
        			if ((bytes += send(socket, &out_image_160x120[bytes], imgtotal-bytes, 0)) < 0)
				{	
            				std::cerr << "error sending " << std::endl;
	       		         	std::flush (std::cerr);
                       		 	//sem_post(&capture_video_semaphore);  
            				break;
       			 	}	
				if (debug_level == 1)
				{
					std::cerr << "bytes sent " << bytes << " index " << ++ index << "  total size " << imgtotal/4; 
		        		std::cerr << " client_status[socket] " << client_status[socket] << endl;
	        			std::flush (std::cerr);
				}
			}	
		}
		else
		{
		        int imgtotal = elmsize*img_width*img_height;
			if (flip_img)
   			   flip_compress_frame (greyscale,out_image);
			else
   			   compress_frame (greyscale,out_image);
	       		while (bytes != imgtotal && client_status[socket] == true)
			{
        			if ((bytes += send(socket, &out_image[bytes], imgtotal-bytes, 0)) < 0)
				{	
            				std::cerr << "error sending  imgtotal " << imgtotal << " offset " <<  bytes << std::endl;
	                		std::flush (std::cerr);
                        		//sem_post(&capture_video_semaphore);  
            				break;
       			 	}
				if (debug_level == 1)
				{
					std::cerr << "bytes sent " << bytes << " index " << ++ index << "  total size " << imgtotal;
					std::cerr << " client_status[socket] " << client_status[socket] << endl;
	        			std::flush (std::cerr);
				}
			}	
		}
		//std::cerr << " before  send iend bytes "<< bytes << endl;
        	//std::flush (std::cerr);
        	buf [0] = 0xfa;
        	buf [1] = 0xfb;
        	buf [2] = 0xfc;
        	buf [2] = 0xfd;
        	if ((bytes = send(socket, buf, 4, 0)) < 0)
		{
            		std::cerr << "bytes = " << bytes << std::endl;
	    		std::flush (std::cerr);
            		//sem_post(&capture_video_semaphore);  
            		break;
	    		//std::cerr << " after send bytes "<< bytes << endl;
	    		//std::flush (std::cerr);
			//sleep (60*60);
	 	}	

       	//sem_post(&capture_video_semaphore);
	//cout << "  send " << endl;
	//std::cerr << "after send frame " << endl;
        //std::flush (std::cerr);
    }

    std::cerr << "send quitting..." << std::endl;
    system ("shutdown -fr now");
    std::flush (std::cerr);
    close(socket);
    //client_status[socket] = true;
    //sem_post(&capture_video_semaphore);  
   }
    catch(const std::exception& e)
	{
		std::cout << " <<<<  Excption try again " <<  e.what() << std::endl;
	}
    return NULL;
}

void *recv_data(void *ptr){
    int socket = *(int *)ptr;
    char *buffer;
    int bytes;

    while(1){
        bytes = recv(socket, buffer, sizeof(int), MSG_WAITALL);

        if (bytes < 0){
            std::cerr << "<<< error receiving from client >>>" << std::endl;
            fflush (stderr);
        }
        else if (bytes > 0){
            int msg = atoi(buffer);

            if (msg == QUIT){
                sem_wait(&semaphore);
                running = true;
                sem_post(&semaphore);
                break;
            }
        }
    }

    std::cerr << "<<<<<<<  recv quitting...>>>>>>>" << std::endl;
    system ("shutdown -fr now");
    fflush (stderr);
    close(socket);

    return (NULL);
}






void *recv_ctr_data(void *ptr)
{
    int socket = *(int *)ptr;
    char buffer[4];
    int bytes;
    fprintf (stderr, "<<<<<<< recv ctr data >>>>>>>>>>>>>>\n"); fflush (stderr);
   	 while(client_status[socket])
	{
        	bytes = recv(socket, buffer, 4, MSG_DONTWAIT);
		usleep (100);
                //printf ("<<<<<<<<<  ctr size %d  cmd %x   val %x \n", bytes,buffer[0], buffer [1]);
       		 if (bytes < 0){
            		//std::cerr << "<<< ctr error receiving from client >>>" << std::endl;
            		//fflush (stderr);
			//break;
                  }
		 else if (bytes > 0){
            		int msg = buffer[0];

            		if (msg == QUIT){
                		break;
			}
			else if (msg == ZOOM_CMD) 
			{
				cv::Rect roi;
    				roi.x = ZOOM_FACTOR_X*buffer[1];
    				roi.y = ZOOM_FACTOR_Y*buffer[1];
				if (((img_width-roi.x*2) > 0) && ((img_height-roi.y*2) > 0))
				{
					zoom_x = zoom_y = buffer[1];
				        printf ("zoom level %d \n", zoom_x);
				}
			}
			else
			   {
				break;
			   }
                 }
	}

    std::cerr << "<<<<<<<  recv quitting...>>>>>>>" << std::endl;
    system ("shutdown -fr now");
    fflush (stderr);
    close(socket);
    return (NULL);
}


/*C program to get IP Address of Linux Computer System.*/
char *get_ip_address(const char *intf)
{
    char *ip_address = new char[25];
    int fd;
    struct ifreq ifr;

    /*AF_INET - to define network interface IPv4*/
    /*Creating soket for it.*/
    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /*AF_INET - to define IPv4 Address type.*/
    ifr.ifr_addr.sa_family = AF_INET;

    /*eth0 - define the ifr_name - port name
    where network attached.*/
    memcpy(ifr.ifr_name, intf, IFNAMSIZ - 1);

    /*Accessing network interface information by
    passing address using ioctl.*/
    ioctl(fd, SIOCGIFADDR, &ifr);

    /*closing fd*/
    close(fd);

    /*Extract IP Address*/
    strcpy(ip_address, inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));

    printf("System IP Address is: %s\n", ip_address);

    return ip_address;
}


void *ctr_server (void *ptr)
{

   int                 localSocket;
    struct  sockaddr_in localAddr,
                        remoteAddr;

    int remoteSocket = 0;
  	// A long long wait so that we can easily issue a signal to this process
    int addrLen = sizeof(struct sockaddr_in);
     pthread_t recv_ctr_thread;

    localSocket = socket(AF_INET , SOCK_STREAM , 0);
    cout << " local socket " << localSocket << endl;
    if (localSocket == -1){
         perror("socket() call failed!!");
	 return (0);
    }    
    printf (" local socket id %d \n",localSocket);

    localAddr.sin_family = AF_INET;
    //localAddr.sin_addr.s_addr = INADDR_ANY;
    //localAddr.sin_addr.s_addr =  inet_addr("25.27.164.23");
    //char *addr = get_ip_address ("ham0");
    char *addr = get_ip_address ("eth0");
    if (addr == 0)
       return NULL;

     localAddr.sin_addr.s_addr =  inet_addr(addr);
    localAddr.sin_port = htons( ctr_port );
    if( bind(localSocket,(struct sockaddr *)&localAddr , sizeof(localAddr)) < 0) {
         perror("Can't bind() socket");
         exit(1);
    }
    
    // Listening
    listen(localSocket , 3);
    
    std::cout <<  "Waiting for connections...\n"
              <<  "Server Port:" << ctr_port << std::endl;

    //accept connection from an incoming client
    while(1){
	cout << " before ctr accept" << endl;
        remoteSocket = accept(localSocket, (struct sockaddr *)&remoteAddr, (socklen_t*)&addrLen);  
	cerr << "****************  remote ctr socket " << remoteSocket << endl;
        
        if (remoteSocket < 0) {
            perror("accept failed!");
            exit(1);
        } 
        
        client_status[remoteSocket] = true;
        std::cout << "ctr Connection accepted" << std::endl;
       	pthread_create(&recv_ctr_thread, NULL, recv_ctr_data, &remoteSocket);
       	//recv_ctr_data(&remoteSocket);
    }

    // wait for threads to finish up
    pthread_join(recv_ctr_thread, NULL);
    return 0;
}



void *video_server (void *ptr)
{

   int                 localSocket;
    struct  sockaddr_in localAddr,
                        remoteAddr;
     fprintf (stderr, "START VIDEO SERVER \n");
     fflush (stderr);
  	// A long long wait so that we can easily issue a signal to this process
    int remoteSocket = 0;
    int addrLen = sizeof(struct sockaddr_in);
     pthread_t send_thread;
     pthread_t recv_thread;

    localSocket = socket(AF_INET , SOCK_STREAM , 0);
    cout << " local socket " << localSocket << endl;
    if (localSocket == -1){
         perror("socket() call failed!!");
    }    
    printf (" local socket id %d \n",localSocket);

    localAddr.sin_family = AF_INET;
    //localAddr.sin_addr.s_addr = INADDR_ANY;
    //char *addr = get_ip_address ("ham0");
    char *addr = get_ip_address ("eth0");
    if (addr == 0)
       return NULL;

    localAddr.sin_addr.s_addr =  inet_addr(addr);
    //localAddr.sin_addr.s_addr =  inet_addr("25.27.164.23");
    localAddr.sin_port = htons( video_port );

    if( bind(localSocket,(struct sockaddr *)&localAddr , sizeof(localAddr)) < 0) {
         perror("Can't bind() socket");
         exit(1);
    }
    
    // Listening
    listen(localSocket , 3);
    
    std::cout <<  "Waiting for connections...\n"
              <<  "Server Port:" << video_port << std::endl;

    //accept connection from an incoming client
    while(1){
	cout << " <<<<  before accept" << endl;
        remoteSocket = accept(localSocket, (struct sockaddr *)&remoteAddr, (socklen_t*)&addrLen);  
	cerr << "****************  remote socket " << remoteSocket << endl;
        
        if (remoteSocket < 0) {
            perror("accept failed!");
            exit(1);
        } 
       client_status[remoteSocket] = true;
        
        std::cout << "Connection accepted remote sock id " << remoteSocket <<std::endl;
	if (elmsize == 1)
         	pthread_create(&send_thread, NULL, send_data, &remoteSocket);
	else
         	pthread_create(&send_thread, NULL, send_color_data, &remoteSocket);
        //pthread_create(&recv_thread, NULL, recv_data, &remoteSocket);

        // pthread_join(thread_id, NULL);
    }

    // wait for threads to finish up
    pthread_join(send_thread, NULL);
    //pthread_join(recv_thread, NULL);
    //cout << " <<<< thread is terminated " << endl;
    // close the socket
     //close(remoteSocket);

    return 0;

}

void sig_handler(int signo)
{
  if (signo == SIGPIPE)
  {
   	printf("received SIGPIPE\n");
	for (int i=0;client_status [i];i++)
	{
		if (client_status [i])
		{
			if (sock_status (i) < 0)
			{
				client_status[i] = false;
                                sem_post(&capture_video_semaphore);  

			}
                 }
         }
    }
  if (signo == SIGINT)
  {
    printf("received SIGINT\n");
    cap.release();
    //exit (0);
  }
  system ("shutdown -fr now");
}

int init ()
{
    sem_init(&semaphore, 0, 1);  
    sem_init(&capture_video_semaphore, 0, 1);  
    for (int i = 0;i< NUM_CLIENTS;i++)
	{
		client_status [i] = true;
	}
    if (!cap.isOpened())  // if not success, exit program
    {
        cout << "Cannot open the video cam" << endl;
        return -1;
    }
    cout << "devise is opened" << endl;

    if (!cap.isOpened())  // check if succeeded to connect to the camera
   CV_Assert("Cam open failed");

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 320); // valueX = your wanted width
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 240); // valueY = your wanted heigth
    //cap.set(cv::CAP_PROP_FRAME_WIDTH, 160); // valueX = your wanted width
    //cap.set(cv::CAP_PROP_FRAME_HEIGHT, 120); // valueY = your wanted heigth


    img_height       = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
    img_width        = cap.get(cv::CAP_PROP_FRAME_WIDTH);
    Brightness       = cap.get(cv::CAP_PROP_BRIGHTNESS);
    Contrast         = cap.get(cv::CAP_PROP_CONTRAST );
    Saturation       = cap.get(cv::CAP_PROP_SATURATION);
    Gain             = cap.get(cv::CAP_PROP_GAIN);

    //out_image = new unsigned char [elmsize*img_width*img_height];
    //greyscale= new unsigned char [img_width*img_height];
    
    if (debug_level == 2)
    {
	std::cout << "height:     " << img_height << std::endl;
	std::cout << "width:      " << img_width << std::endl;
	std::cout << "Brightness  " << Brightness << std::endl;
	std::cout << "Contrast    " << Contrast<< std::endl;
	std::cout << "Saturation  " << Saturation<< std::endl;
	std::cout << "Gain        " << Gain << std::endl;

    }

    img = Mat::zeros(img_height, img_width, CV_8UC3);
    //record_img = Mat::zeros(img_height, img_width, CV_8UC3);
    //greyimg = Mat::zeros(height, width, CV_8UC3);

    int imgSize = img.total() * img.elemSize();
    if (debug_level == 2)
    {
    	cout << " ELEMENT SIZE "<< img.elemSize() << endl;
        cout << " IMG TOTAL  "<< img.total() << endl;
    	cout << " IMG SIZE " << imgSize << endl;
    }
    int bytes = 0;

    // make img continuos
    if(!img.isContinuous()){ 
        img = img.clone();
    }
    if(!record_img.isContinuous()){ 
        img = img.clone();
    }

    if (debug_level == 2)
    	std::cout << "Image Size:" << imgSize << std::endl;
    pthread_t capture_video_thread;
    //pthread_create(&capture_video_thread, NULL, capture_video, NULL);
    return 0;

}

int main(int argc, char* argv[])
{

     cout << "start server >>>>>>  " <<  endl;
    if (signal(SIGPIPE, sig_handler) == SIG_ERR)
 	 printf("\ncan't catch SIGINT\n");
    if (signal(SIGINT, sig_handler) == SIG_ERR)
 	 printf("\ncan't catch SIGINT\n");

  	// A long long wait so that we can easily issue a signal to this process
     pthread_t video_server_thread;
     pthread_t ctr_server_thread;
     pthread_t record_video_thread;
     int c;
     while ((c = getopt (argc, argv, "v:p:c:")) != -1)
       {
	   
          printf (" CARACTER is %c %s \n",c,optarg);
           switch (c)
             {
          	 case 'v':
		   capDev = atoi (optarg);
		   cout << " cap vidio #  " << capDev << endl;
            	break;
           	case 'p':
	           video_port = atoi (optarg);
		   cout << "video port  " << video_port << endl;
          	 break;
           	case 'c':
	           ctr_port = atoi (optarg);
		   cout << "ctr port  " << ctr_port << endl;
          	 break;

		case ':':       /* -v or -p or -c without operand */
                       fprintf(stderr,
                           "Option -%c requires an operand\n", optopt);
                       break;
                case '?':
                       fprintf(stderr, "Unrecognized option: '-%c'\n", optopt);
                       break;
		       default:
            			printf ("?? getopt returned character code 0%o ??\n", c);
             }
       }

      if (optind < argc) 
	{
       		 printf ("non-option ARGV-elements: ");
        	while (optind < argc)
            		printf ("%s ", argv[optind++]);
        	printf ("\n");
        }
    cap = cv::VideoCapture(capDev);
    init ();
    pthread_create(&video_server_thread, NULL, video_server, NULL);
    pthread_create(&ctr_server_thread, NULL, ctr_server, NULL);
    pthread_create(&record_video_thread, NULL, record_video, NULL);
    pthread_join(video_server_thread, NULL);
    pthread_join(ctr_server_thread, NULL);
    pthread_join(record_video_thread, NULL);

    return 0;

}
