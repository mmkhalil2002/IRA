//unsigned int millis (void)

#include <stdio.h>
#include <time.h>
#include <stdio.h>
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

FILE *a_file = NULL;
int current_pos = 0;

void get_time (char *time)
{
 	struct timeval tv;
	 struct tm* ptm;
 	char time_string[40];
 	long milliseconds;

 	/* Obtain the time of day, and convert it to a tm struct. */
 	gettimeofday (&tv, NULL);
 	ptm = localtime (&tv.tv_sec);
 	/* Format the date and time, down to a single second. */
 	strftime (time_string, sizeof (time_string), "%Y-%m-%d %H:%M:%S", ptm);
 	/* Compute milliseconds from microseconds. */
 	milliseconds = tv.tv_usec / 1000;
 	// Print the formatted time, in seconds, followed by a decimal point   and the milliseconds. 
 	 sprintf (time,"%s.%03ld\n", time_string, milliseconds);
}

int open_memory_file_wr (char *time,unsigned short camera_num){
    char file_name [256];
    sprintf (file_name,"media/usb/camera%02d/date-%s",camera_num,time);
    printf ("open_memory_file_wr:file name %s \n",file_name);
    if ((a_file = fopen (file_name, "w")) != NULL)
	    return 1;
    return 0;
}

void close_memory_file (FILE *a_file){
	if (a_file != NULL){
	   fclose (a_file);
	   a_file = NULL;
	}
}



//size_t fread(void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file);

//size_t fwrite(const void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file);
//size_t fread(void *ptr, size_t size_of_elements, size_t number_of_elements, FILE *a_file);

int write_img (char *img, int camera_num,int len){
	int size;
	char time [128];
	FILE *a_file = NULL;
	get_time (time);
        open_memory_file_wr (time,camera_num);
	printf ("time  %s \n",time);
	if (a_file != NULL){
	    size = fwrite(img, len, 1, a_file);
	    fflush (a_file);
            close_memory_file (a_file);
	    return (size);
	}
	return (0);
}

int read_img (char  *img,int imgsize){
	int size;
	if (a_file != NULL){
    	        if (fseek(a_file, 0, SEEK_SET) >= 0){
			if (!feof (a_file)){
				size = fread(img, imgsize, 1, a_file);
				return size;
			}
			else {
				printf ("end of file");
				return 0;
			}
		}
               return (0);
	}
	return (0);
}

int open_memory_file_rd (unsigned short camera_num){
    char file_name [256];
    sprintf (file_name,"media/usb/camera%02d",camera_num);
    printf ("file name %s \n",file_name);
    if ((a_file = fopen (file_name, "r")) != NULL)
	    return 1;
    return 0;
}

