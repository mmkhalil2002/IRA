
#include <errno.h>
#include <stdio.h>           //For standard things
#include <stdlib.h>          //malloc
#include <string.h>          //strlen
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include <sys/ipc.h> 
#include <sys/msg.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <fcntl.h> 
#include <termios.h>
#include <unistd.h>
#include "USBTools/usbtoolslib.h"
#include "USBTools/gps.h"
#define MODEM_NUM 6


int main(int argc, char *argv[]) {
	int modem_num = 1;
	char buf [140]={'\0'};;
	int size = 140;
	int index = 0;
	int i = 0;
	struct Packet loc;
	//struct Position curPos, target;
	clearPacket(&loc);

        //printf ("argc %d \n",argc);
        if (argc == 1){
        	fprintf (stderr, "help: select parameters\n");
		fprintf (stderr, "    -m  modem # <1-%d>\n",MODEM_NUM);
		exit (0);
     	}
     	while  (i<argc){
       		//printf (" \nindex %d %s\n",i,argv[i]);    
       		if (!strcmp ("-m",argv[i])){
			 i ++;
		 	modem_num = atoi (argv[i]);
			 if (modem_num < 0 || modem_num > MODEM_NUM){
				 fprintf (stderr, "illegal modem # \n");
				 fprintf (stderr, "modem # <1-%d> \n",MODEM_NUM);
			 }
	 		//printf ("-m is issued %d %d arg %s\n",modem_num,i,argv[i]);
	 		i ++;
	 		continue;
                }
		i++;
     	}
	fprintf (stderr,"GPS Info at modem %d\n\n",modem_num);
//
//$GPRMC,225032.00,A,3300.095424,N,09636.698196,W,0.0,295.1,040820,3.5,E,A*29index 422
//
	//FILE *fp = open_gps (modem_num);
        unsigned char initialPosRead = 0;		
	struct Position initPos, curPos;
	for (int i=1;;i++)
	{
	   printf ("\nindex %d \n",i);
	   printf ("GPS");
	   //FILE *fp = open_gps (modem_num);
           printf ("check 1\n");	
	   FILE *fp = open_gps (modem_num);
           printf ("check 2");	
           read_gps_port(fp,buf,size);
	   //close_gps (fp);
	   int j =0;
	   while (buf [j] != '\0'){
	   printf ("%c",buf[j]);
	   j++;
	   }
	   printf (" ENDGPS");
	   int status = parseGPS(buf, &loc);
        //print distance in KM, miles, and feet
	
	   if (status >= 0){
		if (!initialPosRead){
	   		setPosition(&initPos, loc.lat, loc.lon);
		        printf ("init (lat %f lon %f) \n",loc.lat,loc.lon); 
	  	 	initialPosRead = 1;
         	 }
		else{
	   	   setPosition(&curPos, loc.lat, loc.lon);
		   printf ("cur (lat %f lon %f) \n",loc.lat,loc.lon); 
		}

		printf ("init (lat %Lf lon %Lf)  cur (lat %Lf lon %Lf) \n",initPos.lat,initPos.lon,curPos.lat,curPos.lon); 
		float dist = getDistanceKM(&initPos, &curPos);
		printf("distance KM:%f\n", dist);
		dist = dist_km(&initPos, &curPos);
		printf("distance KM:%f\n", dist);
		dist = distance_km(&initPos, &curPos);
		printf("distance KM:%f\n", dist);
                printf ("check 1\n");	
		dist = getDistanceMiles(&initPos, &curPos);
		printf("distance Miles:%f\n", dist);
		dist = dist_miles(&initPos, &curPos);
		printf("distance miles:%f\n", dist);
		dist = distance_miles(&initPos, &curPos);
		printf("distance miles:%f\n", dist);
	
                printf ("check 2\n");	
		dist = getDistanceFeet(&initPos, &curPos);
		printf("distance Feet:%f\n", dist);
		dist = dist_feet(&initPos, &curPos);
		printf("distance feet:%f\n", dist);
		dist = distance_feet(&initPos, &curPos);
		printf("distance feet:%f\n", dist);
                printf ("check 2\n");	
	   }
	   usleep (30);
            printf ("check 3\n");	
	   close_gps (fp);
            printf ("check 4\n");	
	}
        index = 0;
	/*
         while ((index = get_next_line_from_buf(buf, line, 128, index)) >= 0){
		printf ("%s \n",line);
          }
	printf ("\n\n");
	if(parseGPS(buf, &loc)>=0) //the parser will return -1 if the packet is not a full and valid GPS packet.
	{
		printData(&loc);
	}
	else
	{
		printf("not a valid packet\n");
	}
	*/
}
