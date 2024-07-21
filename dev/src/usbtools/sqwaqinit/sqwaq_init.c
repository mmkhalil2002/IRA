
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
#include "SysMonitor/sysmonitorlib.h" 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <fcntl.h> 
#include <termios.h>
#include <unistd.h>
#include "USBTools/usbtoolslib.h"

#define MODEM_NUM  6

void init_procedure (int modem_num){
	 char buf [1024];
	 int size = 1024;
	 char cmd [128];
	 sprintf (cmd,"ATI3 \r");
         exec_cmd (modem_num,cmd,buf,size);
	 sleep (2);
	 sprintf (cmd,"AT!ENTERCND=\"A710\" \r");
         exec_cmd (modem_num,cmd,buf,size);
	 cmd [strlen (cmd)-1]='\0';
	 printf ("exec [%s] \n",cmd);
	 sprintf (cmd,"AT!USBCOMP=1,1,10D \r");
         exec_cmd (modem_num,cmd,buf,size);
	 cmd [strlen (cmd)-1]='\0';
	 printf ("exec [%s] \n",cmd);
	 sprintf (cmd,"AT!RESET \r");
         exec_cmd (modem_num,cmd,buf,size);
	 cmd [strlen (cmd)-1]='\0';
	 printf ("exec [%s] \n",cmd);
}

int main(int argc, char *argv[]) {
	int modem_num = 2;
	int i = 0;
	
        while  (i<argc){
       //printf (" index %d %s\n",i,argv[i]);    
       		if (!strcmp ("-m",argv[i])){
	 		i ++;
	 		modem_num = atoi (argv[i]);
	 		//printf ("-m is issued %d %d arg %s\n",modem,i,argv[i]);
         		if (modem_num < 0 || modem_num > MODEM_NUM){
				 fprintf (stderr, "illegal modem # \n");
				 fprintf (stderr, "modem # <1-%d> \n",MODEM_NUM);
	 		}
	 		i ++;
		 	continue;
         	}
       		i++;
     	  }
    
     if (modem_num != 0)
	  init_procedure (modem_num);
     else{
	     for (int i = 0;i< MODEM_NUM;i++)
	       init_procedure (i+1);
     }

	      
};
