
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
#include "boot.h"
#include <net/if.h>
#include <sys/types.h>
#include <ifaddrs.h>
#include <unistd.h>
#include <string.h> /* for strncpy */

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <net/if.h>
#include <arpa/inet.h>
//#include "USBTools/usbtoolslib.h"

#define MODEM_NUM  6
//char apn [128]="syniverseus";
//char apn [128]="iot.aer.net";
//char apn [128]="VZWINTERNET";
char apn [128]="internet.curiosity.sprint.com";

char *apns [MODEM_NUM];
char *mccmnc [MODEM_NUM];
char *interface [MODEM_NUM];



void boot_procedure (int modem_num, char mcc[],char mnc[]){
	 char buf [1024];
	 int size = 1024;
	 char cmd [128];
	 sprintf (cmd,"ATE1 \r");
         exec_cmd (modem_num,cmd,buf,size);
	 printf ("RESP [ %s ] \n",buf);
	 printf ("START BOOT PRCEDURE DFOR MODEM %d \n",modem_num);
	 sprintf (cmd,"ATI3 \r");
         exec_cmd (modem_num,cmd,buf,size);
	 printf ("RESP [ %s ] \n",buf);
	 sleep (1);
	 sprintf (cmd,"AT+CIMI \r");
         exec_cmd (modem_num,cmd,buf,size);
	 printf ("RESP [ %s ] \n",buf);
	 sleep (1);
	 sprintf (cmd,"AT!BAND=7 \r");
         exec_cmd (modem_num,cmd,buf,size);
	 printf ("RESP [ %s ] \n",buf);
	 sleep (1);
	 sprintf (cmd,"AT!BAND? \r");
         exec_cmd (modem_num,cmd,buf,size);
	 printf ("RESP [ %s ] \n",buf);
	 sleep (1);
	 sprintf (cmd,"AT+COPS=4,2,\"%s%s\" \r",mcc,mnc);
         exec_cmd (modem_num,cmd,buf,size);
	 printf ("RESP [ %s ] \n",buf);
	 sleep (1);
	 sprintf (cmd,"AT+CGDCONT=1,\"IP\",\"%s\",\"0.0.0.0\",0,0,0,0 \r",apn);   // add \r
         exec_cmd (modem_num,cmd,buf,size);
	 printf ("RESP [ %s ] \n",buf);
	 sleep (1);
	 sprintf (cmd,"AT+CREG=1\r");
         exec_cmd (modem_num,cmd,buf,size);
	 printf ("RESP [ %s ] \n",buf);
	 sleep (1);
	 sprintf (cmd,"AT+CREG?\r");
         exec_cmd (modem_num,cmd,buf,size);
	 printf ("RESP [ %s ] \n",buf);
	 sleep (1);
	 int s = get_creg_status (buf);
	 if (s == 1){
		 printf ("SUCCESS: devise is registered at home\n");
	 }
	 else if (s == 2){
		 printf ("SUCCESS: not registered but try to search for an operator to register withe\n");
	 }
	 else
  		 printf ("SUCCESS: devise is registered \n");
	 sprintf (cmd,"AT+CGPADDR \r");
         exec_cmd (modem_num,cmd,buf,size);
	 printf ("RESP [ %s ] \n",buf);
	 sleep (2);
	 printf ("data establishment \n");
	 sprintf (cmd,"AT!SCACT=1,1 \r");
         exec_cmd (modem_num,cmd,buf,size);
	 sleep (1);
	 printf ("RESP SCACT[ %s ] \n",buf);
	 sleep (1);
	 sprintf (cmd,"AT!SCACT? \r");
         exec_cmd (modem_num,cmd,buf,size);
	 printf ("RESP [ %s ] \n",buf);
	 sleep (1);
	 sprintf (cmd,"ip link set dev %s down",interface [modem_num-1]);
	 system (cmd);
	 fprintf (stderr,"interface [%s] cmd (%s) \n",interface[modem_num-1],cmd);
	 fflush (stderr);
	 sprintf (cmd,"ip link set dev %s up",interface [modem_num-1]);
	 system (cmd);
	 fprintf (stderr,"cmd (%s) \n",cmd);
	 fflush (stderr);
	 sprintf (cmd,"route delete default");
	 printf ("CMD %s \n",cmd);
	 system (cmd);
	 sprintf (cmd,"route add -net default dev %s",interface[modem_num-1]);
	 printf ("CMD %s \n",cmd);
	 system (cmd);
	 sprintf (cmd,"dhclient -d -4 %s",interface [modem_num-1]);
	 system (cmd);
	 fprintf (stderr,"procedure completed \n");
	 fflush (stderr);
}

void help (){

 fprintf (stderr, "-help                 # help function\n");
 fprintf (stderr, "-m <module#i>         # module number from 1-6\n");
 fprintf (stderr, "-mcc <mcc value>      # mcc value example 310\n");
 fprintf (stderr, "-mcc <mnc value>      # mnc value example 882\n");
 fprintf (stderr, "-apn <apn value>      # apn value example vzwinternet\n");

}

int main(int argc, char *argv[]) {
	int modem_num = 2;
	int i = 0;
	//char mcc [32] = "310"; // att
	//char mnc [32] = "410"; // att
	char mcc [32] = "311"; // sprint aerie
	char mnc [32] = "882"; // sprint aerie
	//char mcc [32] = "311"; //VzW 
	//char mnc [32] = "480"; //VzW 
	
        //printf ("argc %d \n",argc);
	// AT+CGDCONT=1,"IP","syniverseus","0.0.0.0",0,0,0,0
	//
	//
        for (int i=0;i<MODEM_NUM;i++){
		interface [i] = NULL;
	}	
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
      	 	//printf (" new parm %d %s\n",i,argv[i]);    
       		if (!strcmp ("-mcc",argv[i])){
	       		strcpy (mcc,argv[i+1]);
	       		i ++;
	      		continue;
		}
       		if (!strcmp ("-mnc",argv[i])){
	       		strcpy (mnc,argv[i+1]);
	       		i ++;
	      		continue;
	 	}
       		if (!strcmp ("-apn",argv[i])){
	       		strcpy (apn,argv[i+1]);
	       		i ++;
	      		continue;
	 	}
       		if (!strcmp ("-help",argv[i])){
			help ();
			exit (0);
		}
       		i++;
     	  }

	fprintf (stderr,"mcc %s mnc %s apn %s \n",mcc,mnc,apn);
	fflush (stderr);
    
     //read_file ("interface.info",interface);
     get_interfaces (interface);
     for (i=0;i<MODEM_NUM;i++){
	     if (interface[i]){
	     	fprintf (stderr,"interface # %d intf [%s] iaddr [%s]\n",i,interface[i],get_ipaddr(interface [i]));
	     	fflush (stderr);
	     }
     }
     //init_config ();
     //get_apn (apns);
     //get_mccmnc (mccmnc);
     //get_interface (interface);
     if (modem_num != 0)
	  boot_procedure (modem_num,mcc,mnc);
     else{
	     for (int i = 0;i< MODEM_NUM;i++)
	       boot_procedure (i+1,mcc,mnc);
     }

	      
};
