#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>
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

using namespace cv;
using namespace std;

#define NUM_CLIENTS 100
#define COMPRESS_0PIX    0x00
#define COMPRESS_2PIX    0x01
#define COMPRESS_2x2PIX  0x02
#define COMPRESS_4x4PIX  0x03

#define imag_width     640
#define imag_height    480

sem_t semaphore, capture_video_semaphore;
bool running  = false;
int QUIT      = 0;
int capDev    = 0;
//VideoCapture cap(capDev);
VideoCapture cap;
Mat img, flippedFrameg;
// open the default camera
int remoteSocket = 0;
bool status = true;
int numfds = 0;
int debug_level = 2;
bool client_status [NUM_CLIENTS];
unsigned char  greyscale         [imag_width*imag_height]; 
unsigned char  out_image         [imag_width*imag_height]; 
unsigned char  out_image_2pix    [imag_width*imag_height/2]; 
unsigned char  out_image_2x2pix  [imag_width*imag_height/4]; 
unsigned char  out_image_4x4pix  [imag_width*imag_height/16]; 
unsigned char  cout_image        [3*imag_width*imag_height]; 
unsigned char  cout_image_2pix   [3*imag_width*imag_height/2]; 
unsigned char  cout_image_2x2pix [3*imag_width*imag_height/4]; 
unsigned char  cout_image_4x4pix [3*imag_width*imag_height/16]; 
int   img_width  = 0;
int   img_height = 0;
float Brightness = 0.0;
float Contrast   = 0.0;  
float Saturation = 0.0;
float Gain       = 0.0;


int elmsize  = 3;
bool flip_img = false;
int video_port = 4097;



unsigned char compress_type = COMPRESS_2x2PIX;




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

void flip_compress_frame (unsigned char *in_frame,unsigned  char * out_frame)
{
	if (compress_type == COMPRESS_2PIX)
	{
		for (int y=0;y<img_height;y++)
		   for (int x=0,cx=0;x<img_width;x+=2,cx++)
		    {
			out_frame [y*img_width+cx] = in_frame [y*img_width +img_width-1-x];
		    }
	}
	else if (compress_type == COMPRESS_2x2PIX)
	{
		std::cout << " image height " << img_height << " img width " << img_width << endl;
		std::flush (std::cout);
		for (int row = 0, crow=0; row < img_height;row+=2,crow++)
	          for (int  col = 0, ccol=0; col < img_width;col+=2,ccol++)
		  {
			  int sl = row*img_width+img_width -1 -col;
			  int dl = crow*img_width/2 + ccol;
			  out_frame [dl] = in_frame[sl];
			  //out_frame [dl] = 0xf0;
			  //printf (" pixel %x  dl %d  sl %d\n",out_frame [dl], dl,sl);
			  //printf (" pixel %x  dl %d \n",out_frame [dl], dl);
			  //printf ("PIX %x imgw %d imgh %d  row %d col %d crow %d ccol %d  dl %d  sl %d\n",out_frame[dl], img_width,img_height,row,col,crow,ccol,dl,sl);
			  //printf ("imgw %d imgh %d  row %d col %d crow %d ccol %d  dl %d  sl %d\n",img_width,img_height,row,col,crow,ccol,dl,sl);
                  }
		//for (int i=0;i< greyimg.total()/4;i++)
		//	printf (" PIX %x \n",out_frame[i]);


	}
	else if (compress_type == COMPRESS_4x4PIX)
	{
		std::cout << " image height " << img_height << " img width " << img_width << endl;
		std::flush (std::cout);
		for (int row = 0, crow=0; row < img_height;row+=4,crow++)
		{
	          for (int  col = 0, ccol=0; col < img_width;col+=4,ccol++)
		  {
			  int sl = row*img_width+img_width -1-col;
			  int dl = crow*img_width/4 + ccol;
			  out_frame [dl] = in_frame[sl];
			  //out_frame [dl] = 0xf0;
			  //printf (" pixel %x  dl %d  sl %d\n",out_frame [dl], dl,sl);
			  //printf (" pixel %x  dl %d \n",out_frame [dl], dl);
			  //printf ("PIX %x imgw %d imgh %d  row %d col %d crow %d ccol %d  dl %d  sl %d\n",out_frame[dl], img_width,img_height,row,col,crow,ccol,dl,sl);
			  //printf ("imgw %d imgh %d  row %d col %d crow %d ccol %d  dl %d  sl %d\n",img_width,img_height,row,col,crow,ccol,dl,sl);
                  }
		}
		//for (int i=0;i< greyimg.total()/4;i++)
		//	printf (" PIX %x \n",out_frame[i]);


	}
	else 
	{
		for (int row = 0; row < img_height;row++)
		{
	          for (int  col = 0 ; col < img_width;col++)
		  {
			  int sl = row*img_width+img_width -1-col;
			  int dl = row*img_width + col;
			  out_frame [dl] = in_frame[sl];
		  }
                }
	}
}


void compress_frame (unsigned char *in_frame,unsigned  char * out_frame)
{
	if (compress_type == COMPRESS_2PIX)
	{
		for (int y=0;y<img_height;y++)
		   for (int x=0,cx=0;x<img_width;x+=2,cx++)
		    {
			out_frame [y*img_width+cx] = in_frame [y*img_width +x];
		    }
		/*
		for (int i=0,j=0;i<in_len;i+=2,j++)
		{
			out_frame [j] = in_frame [i];
		}
		*/
	}
	else if (compress_type == COMPRESS_2x2PIX)
	{
		std::cout << " image height " << img_height << " img width " << img_width << endl;
		std::flush (std::cout);
		for (int row = 0, crow=0; row < img_height;row+=2,crow++)
	          for (int  col = 0, ccol=0; col < img_width;col+=2,ccol++)
		  {
			  int sl = row*img_width+col;
			  int dl = crow*img_width/2 + ccol;
			  out_frame [dl] = in_frame[sl];
			  //out_frame [dl] = 0xf0;
			  //printf (" pixel %x  dl %d  sl %d\n",out_frame [dl], dl,sl);
			  //printf (" pixel %x  dl %d \n",out_frame [dl], dl);
			  //printf ("PIX %x imgw %d imgh %d  row %d col %d crow %d ccol %d  dl %d  sl %d\n",out_frame[dl], img_width,img_height,row,col,crow,ccol,dl,sl);
			  //printf ("imgw %d imgh %d  row %d col %d crow %d ccol %d  dl %d  sl %d\n",img_width,img_height,row,col,crow,ccol,dl,sl);
                  }
		//for (int i=0;i< greyimg.total()/4;i++)
		//	printf (" PIX %x \n",out_frame[i]);


	}
	else if (compress_type == COMPRESS_4x4PIX)
	{
		std::cout << " image height " << img_height << " img width " << img_width << endl;
		std::flush (std::cout);
		for (int row = 0, crow=0; row < img_height;row+=4,crow++)
		{
	          for (int  col = 0, ccol=0; col < img_width;col+=4,ccol++)
		  {
			  int sl = row*img_width+col;
			  int dl = crow*img_width/4 + ccol;
			  out_frame [dl] = in_frame[sl];
			  //out_frame [dl] = 0xf0;
			  //printf (" pixel %x  dl %d  sl %d\n",out_frame [dl], dl,sl);
			  //printf (" pixel %x  dl %d \n",out_frame [dl], dl);
			  //printf ("PIX %x imgw %d imgh %d  row %d col %d crow %d ccol %d  dl %d  sl %d\n",out_frame[dl], img_width,img_height,row,col,crow,ccol,dl,sl);
			  //printf ("imgw %d imgh %d  row %d col %d crow %d ccol %d  dl %d  sl %d\n",img_width,img_height,row,col,crow,ccol,dl,sl);
                  }
		}
		//for (int i=0;i< greyimg.total()/4;i++)
		//	printf (" PIX %x \n",out_frame[i]);


	}
	else 
	{
		for (int row = 0; row < img_height;row++)
		{
	          for (int  col = 0; col < img_width;col++)
		  {
			  int sl = row*img_width+col;
			  int dl = row*img_width + col;
			  out_frame [dl] = in_frame[sl];
			  //out_frame [dl] = 0xf0;
			  //printf (" pixel %x  dl %d  sl %d\n",out_frame [dl], dl,sl);
			  //printf (" pixel %x  dl %d \n",out_frame [dl], dl);
			  //printf ("PIX %x imgw %d imgh %d  row %d col %d crow %d ccol %d  dl %d  sl %d\n",out_frame[dl], img_width,img_height,row,col,crow,ccol,dl,sl);
			  //printf ("imgw %d imgh %d  row %d col %d crow %d ccol %d  dl %d  sl %d\n",img_width,img_height,row,col,crow,ccol,dl,sl);
                  }
		}
		//for (int i=0;i< greyimg.total()/4;i++)
	}
}

void flip_compress_color_frame (unsigned char *in_frame,unsigned  char * out_frame)
{
	
	try{
        	if (compress_type == COMPRESS_2PIX)
		{
			for (int y = 0; y < img_height;y++)
			{
	        	  for (int  x = 0, cx=0; x < img_width;cx++,x+=2)
		  		{
			 		 out_frame [y*elmsize*img_width/2+3*cx+0] = in_frame[y*elmsize*img_width+3*(img_width-1-x)+0];
			 		 out_frame [y*elmsize*img_width/2+3*cx+1] = in_frame[y*elmsize*img_width+3*(img_width-1-x)+1];
			 		 out_frame [y*elmsize*img_width/2+3*cx+2] = in_frame[y*elmsize*img_width+3*(img_width-1-x)+2];
                  		}
			}
		}	
		else if (compress_type == COMPRESS_2x2PIX)
		{
			std::cout << " image height " << img_height << " img width " << img_width << endl;
			std::flush (std::cout);
			for (int y = 0, cy=0;y < img_height;y+=2,cy++)
	          		for (int  x = 0, cx=0; x < img_width;x+=2,cx++)
		  		{
					//if (cx < 3*cx+2  && x < 3*x+2)
					{
			 		 out_frame [cy*elmsize*img_width/2+3*cx+0] = in_frame[y*elmsize*img_width+3*(img_width-1-x)+0];
			 		 out_frame [cy*elmsize*img_width/2+3*cx+1] = in_frame[y*elmsize*img_width+3*(img_width-1-x)+1];
			 		 out_frame [cy*elmsize*img_width/2+3*cx+2] = in_frame[y*elmsize*img_width+3*(img_width-1-x)+2];
		 		         //out_frame [cy*elmsize*img_width/2+3*cx+0] = 0xE0;
			 		 //out_frame [cy*elmsize*img_width/2+3*cx+1] = 0xE0;
			 		 //out_frame [cy*elmsize*img_width/2+3*cx+2] = 0xE0;
					}
                  		}
				//for (int i=0;i< elmsize*img_width*img_height/4;i++)
					//printf (" index %d  PIX %x \n",i,out_frame[i]);


		}
		else if (compress_type == COMPRESS_4x4PIX)
		  { 
			std::cout << " image height " << img_height << " img width " << img_width << endl;
			std::flush (std::cout);
			for (int y = 0, cy=0; y < img_height;y+=4,cy++)
			{
	        	  for (int  x = 0, cx=0; x < img_width;x+=4,cx++)
		  		{
			 		 out_frame [cy*elmsize*img_width/4+3*cx+0] = in_frame[y*elmsize*img_width+3*(img_width-1-x)+0];
			 		 out_frame [cy*elmsize*img_width/4+3*cx+1] = in_frame[y*elmsize*img_width+3*(img_width-1-x)+1];
			 		 out_frame [cy*elmsize*img_width/4+3*cx+2] = in_frame[y*elmsize*img_width+3*(img_width-1-x)+2];
                  		}
			}
			//for (int i=0;i< greyimg.total()/4;i++)
			//	printf (" PIX %x \n",out_frame[i]);


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
        	if (compress_type == COMPRESS_2PIX)
		{
			for (int y = 0; y < img_height;y++)
			{
	        	  for (int  x = 0, cx=0; x < img_width;cx++,x+=2)
		  		{
			 		 out_frame [y*elmsize*img_width/2+3*cx+0] = in_frame[y*elmsize*img_width+3*x+0];
			 		 out_frame [y*elmsize*img_width/2+3*cx+1] = in_frame[y*elmsize*img_width+3*x+1];
			 		 out_frame [y*elmsize*img_width/2+3*cx+2] = in_frame[y*elmsize*img_width+3*x+2];
                  		}
			}
			/*
			for (int i=0,j=0;i<in_len;i+=2,j++)
			{
				if (3*i +2 < in_len)
				{
					out_frame [3*j+0] = in_frame [3*i+0];
					out_frame [3*j+1] = in_frame [3*i+1];
					out_frame [3*j+2] = in_frame [3*i+2];
				}
			}
			*/
		}	
		else if (compress_type == COMPRESS_2x2PIX)
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
		else if (compress_type == COMPRESS_4x4PIX)
		  { 
			std::cout << " image height " << img_height << " img width " << img_width << endl;
			std::flush (std::cout);
			for (int y = 0, cy=0; y < img_height;y+=4,cy++)
			{
	        	  for (int  x = 0, cx=0; x < img_width;x+=4,cx++)
		  		{
			 		 out_frame [cy*elmsize*img_width/4+3*cx+0] = in_frame[y*elmsize*img_width+3*x+0];
			 		 out_frame [cy*elmsize*img_width/4+3*cx+1] = in_frame[y*elmsize*img_width+3*x+1];
			 		 out_frame [cy*elmsize*img_width/4+3*cx+2] = in_frame[y*elmsize*img_width+3*x+2];
                  		}
			}
			//for (int i=0;i< greyimg.total()/4;i++)
			//	printf (" PIX %x \n",out_frame[i]);


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
	          greyscale[y*img_width + img_width -1-x] = (color [3*(y)*img_width +3*x+0] + color [3*(y)*img_width+3*x+1] + color [3*(y)*img_width +3*x+2])/3-1;
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
	          greyscale[y*img_width +x] = (color [3*(y)*img_width +3*x+0] + color [3*(y)*img_width+3*x+1] + color [3*(y)*img_width +3*x+2])/3-1;
		}
	}

}


void *capture_video(void *ptr)
{
	//while (true)
	{
		try
		{
		    // your code here
        		cap >> img;
			img_width =  img.cols;
			img_height =  img.rows;
        		//bool bSuccess = cap.read(img); // read a new frame from video
        		sem_wait(&capture_video_semaphore);
       			//cv::cvtColor(img, greyimg, cv::COLOR_BGR2GRAY); 
		        if (elmsize == 1)
			{
			   convert_2_greyscale (img.data,img_width,img_height,greyscale);
			}
        		sem_post(&capture_video_semaphore);
			cout << "reading " << endl;
			usleep (100);
		}
		catch(const std::exception& e)
		{
			std::cout << " <<<<  Excption try again " <<  e.what() << std::endl;
			capture_video (NULL);
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
	
   		//cout << "get grey image " << endl;
      		capture_video(NULL);

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

		if (compress_type == COMPRESS_2PIX)
		{
		        int imgtotal = elmsize*img_width*img_height;
			if (flip_img)
   			   flip_compress_color_frame (img.data,cout_image_2pix);
			else
   			   compress_color_frame (img.data,cout_image_2pix);
			//compress_frame (greyscale,greytotal,out_frame_2pix);
			//while (bytes != greyimg.total()/2 && client_status[socket] == true)

			while (bytes != imgtotal/2 && client_status[socket] == true)
			{
        			//if ((bytes += send(socket, &out_frame_2pix[bytes], greyimg.total()/2-bytes, 0)) < 0)
        			//if ((bytes += send(socket, &out_frame_2pix[bytes], greytotal/2-bytes, 0)) < 0)
        			if ((bytes += send(socket, &cout_image_2pix[bytes], imgtotal/2-bytes, 0)) < 0)
		 		{	
            				std::cerr << "error sending " << std::endl;
	               		 	std::flush (std::cerr);
                        	 	//sem_post(&capture_video_semaphore);  
            				break;
       				 }
				if (debug_level == 1)
				{
					std::cerr << "bytes sent " << bytes << " index " << ++ index << "  total size " << imgtotal/2; 
		        		std::cerr << " client_status[socket] " << client_status[socket] << endl;
	        			std::flush (std::cerr);
				}	
			}	
		}
		else if (compress_type == COMPRESS_2x2PIX)
		{
		       int imgtotal = elmsize*img_width*img_height;
			if (flip_img)
   			   flip_compress_color_frame (img.data,cout_image_2x2pix);
			else
   			   compress_color_frame (img.data,cout_image_2x2pix);
			//while (bytes != greyimg.total()/4 && client_status[socket] == true)
			while (bytes != imgtotal/4 && client_status[socket] == true)
	 		{
        			//if ((bytes += send(socket, &out_frame_2x2pix[bytes], greytotal/4-bytes, 0)) < 0)
        			if ((bytes += send(socket, &cout_image_2x2pix[bytes], imgtotal/4-bytes, 0)) < 0)
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
		else if (compress_type == COMPRESS_4x4PIX)
		{
		       int imgtotal = elmsize*img_width*img_height;
			if (flip_img)
   			   flip_compress_color_frame (img.data,cout_image_4x4pix);
			else
   			   compress_color_frame (img.data,cout_image_4x4pix);
			while (bytes != imgtotal/16 && client_status[socket] == true)
			{
        			//if ((bytes += send(socket, &out_frame_4x4pix[bytes], greyimg.total()/16-bytes, 0)) < 0)
        			//if ((bytes += send(socket, &out_frame_4x4pix[bytes], greytotal/16-bytes, 0)) < 0)
        			if ((bytes += send(socket, &cout_image_4x4pix[bytes], imgtotal/16-bytes, 0)) < 0)
				{	
            				std::cerr << "error sending " << std::endl;
	                		std::flush (std::cerr);
                        		//sem_post(&capture_video_semaphore);  
            				break;
       			 	}
				if (debug_level == 1)
				{	
					std::cerr << "bytes sent " << bytes << " index " << ++ index << "  total size " << imgtotal/16; 
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

		if (compress_type == COMPRESS_2PIX)
		{
		        int imgtotal = elmsize*img_width*img_height;
			if (flip_img)
   			   flip_compress_frame (greyscale,out_image_2pix);
			else
   			   compress_frame (greyscale,out_image_2pix);
			//compress_frame (greyscale,greytotal,out_frame_2pix);
			//while (bytes != greyimg.total()/2 && client_status[socket] == true)

			while (bytes != imgtotal/2 && client_status[socket] == true)
			{
        			//if ((bytes += send(socket, &out_frame_2pix[bytes], greyimg.total()/2-bytes, 0)) < 0)
        			//if ((bytes += send(socket, &out_frame_2pix[bytes], greytotal/2-bytes, 0)) < 0)
        			if ((bytes += send(socket, &out_image_2pix[bytes], imgtotal/2-bytes, 0)) < 0)
		 		{	
            				std::cerr << "error sending " << std::endl;
	               		 	std::flush (std::cerr);
                        	 	//sem_post(&capture_video_semaphore);  
            				break;
       				 }
				if (debug_level == 1)
				{
					std::cerr << "bytes sent " << bytes << " index " << ++ index << "  total size " << imgtotal/2; 
		        		std::cerr << " client_status[socket] " << client_status[socket] << endl;
	        			std::flush (std::cerr);
				}	
			}	
		}
		else if (compress_type == COMPRESS_2x2PIX)
		{
		       int imgtotal = elmsize*img_width*img_height;
			if (flip_img)
   			    flip_compress_frame (greyscale,out_image_2x2pix);
			else
   			    compress_frame (greyscale,out_image_2x2pix);
			//while (bytes != greyimg.total()/4 && client_status[socket] == true)
			while (bytes != imgtotal/4 && client_status[socket] == true)
	 		{
        			//if ((bytes += send(socket, &out_frame_2x2pix[bytes], greytotal/4-bytes, 0)) < 0)
        			if ((bytes += send(socket, &out_image_2x2pix[bytes], imgtotal/4-bytes, 0)) < 0)
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
		else if (compress_type == COMPRESS_4x4PIX)
		{
		       int imgtotal = elmsize*img_width*img_height;
			if (flip_img)
   			   flip_compress_frame (greyscale,out_image_4x4pix);
			else
   			   compress_frame (greyscale,out_image_4x4pix);
			while (bytes != imgtotal/16 && client_status[socket] == true)
			{
        			//if ((bytes += send(socket, &out_frame_4x4pix[bytes], greyimg.total()/16-bytes, 0)) < 0)
        			//if ((bytes += send(socket, &out_frame_4x4pix[bytes], greytotal/16-bytes, 0)) < 0)
        			if ((bytes += send(socket, &out_image_4x4pix[bytes], imgtotal/16-bytes, 0)) < 0)
				{	
            				std::cerr << "error sending " << std::endl;
	                		std::flush (std::cerr);
                        		//sem_post(&capture_video_semaphore);  
            				break;
       			 	}
				if (debug_level == 1)
				{	
					std::cerr << "bytes sent " << bytes << " index " << ++ index << "  total size " << imgtotal/16; 
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

void *recv_dayyta(void *ptr){
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
    fflush (stderr);
    close(socket);
    return (NULL);
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
    int retval = getsockopt (remoteSocket, SOL_SOCKET, SO_ERROR, &error, &len);
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
    exit (0);
  }
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

    cap.set(cv::CAP_PROP_FRAME_WIDTH, 640); // valueX = your wanted width
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 480); // valueY = your wanted heigth


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

    if (debug_level == 2)
    	std::cout << "Image Size:" << imgSize << std::endl;
    pthread_t capture_video_thread;
    //pthread_create(&capture_video_thread, NULL, capture_video, NULL);
    return 0;

}

void *video_server (void *ptr)
{

   int                 localSocket;
    struct  sockaddr_in localAddr,
                        remoteAddr;

  	// A long long wait so that we can easily issue a signal to this process
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
    localAddr.sin_addr.s_addr =  inet_addr("25.27.164.23");
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
	cout << " before accept" << endl;
        remoteSocket = accept(localSocket, (struct sockaddr *)&remoteAddr, (socklen_t*)&addrLen);  
	cerr << "****************  remote socket " << remoteSocket << endl;
        
        if (remoteSocket < 0) {
            perror("accept failed!");
            exit(1);
        } 
       client_status[remoteSocket] = true;
        
        std::cout << "Connection accepted" << std::endl;
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

int main(int argc, char* argv[])
{

    if (signal(SIGPIPE, sig_handler) == SIG_ERR)
 	 printf("\ncan't catch SIGINT\n");
    if (signal(SIGINT, sig_handler) == SIG_ERR)
 	 printf("\ncan't catch SIGINT\n");

  	// A long long wait so that we can easily issue a signal to this process
     pthread_t video_server_thread;
     int c;
     while ((c = getopt (argc, argv, "v:p:")) != -1)
       {
           switch (c)
             {
          	 case 'v':
		   capDev = atoi (optarg);
		   cout << " cap vedio #  " << capDev << endl;
            	break;
           	case 'p':
	          video_port = atoi (optarg);
		   cout << "video port  " << video_port << endl;
          	 break;

		case ':':       /* -v or -p without operand */
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
    pthread_join(video_server_thread, NULL);

    return 0;

}
