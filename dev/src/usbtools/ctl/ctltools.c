
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


int main(int argc, char *argv[]) {
	int modem_num = 1;
	char buf [1024];
	char cmd [128];
	char tmp [128];
	int size = 1024;
        //printf ("argc %d \n",argc);
	 sprintf (cmd,"ati3 \r");
         exec_cmd (modem_num,cmd,buf,size);
	 printf ("ati3 cmd = %s \n",buf);
	 sprintf (cmd,"at!gstatus? \r");
         exec_cmd (modem_num,cmd,buf,size);
         printf ("gstatus result  %s \n",buf);
         get_lteband (buf,tmp);
         printf ("lte band  [%s] \n",tmp);
         get_ltebandwidth (buf,tmp);
         printf ("lte band w [%s] \n",tmp);
         get_temperature (buf,tmp);
         printf ("temprature [%s] \n",tmp);
   
	      
}
