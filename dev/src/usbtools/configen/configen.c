
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
#include <net/if.h>
#include <sys/types.h>
#include <ifaddrs.h>

#define MODEM_NUM  6
#define ITER 3
#define bool unsigned char
#define false 0
#define true 1
char *intfs [MODEM_NUM];
char *intf [128];


void print_header (FILE *fp){
  fprintf (fp,"#\n");
  fprintf (fp,"#This is the SqwaQ generation Configuration File.\n");
  fprintf (fp,"#\n");
  fprintf (fp,"#  This file should eventually be available through:\n");
  fprintf (fp,"#\n");
  fprintf (fp,"#       /etc/sqwaq/sq_config.conf\n");
  fprintf (fp,"#\n");
  fprintf (fp,"#\n");
  fprintf (fp,"#  For the 'operator' field, we need to use the strings as broadcast \n");
  fprintf (fp,"#  by the carriers as heard by the modems via the AT+COPS=? \n");
  fprintf (fp,"#\n");
  fprintf (fp,"#  Currently, there are supported:\n");
  fprintf (fp,"#\n");
  fprintf (fp,"#               AT&T\n");
  fprintf (fp,"#               Verizon Wireless\n");
  fprintf (fp,"#               T-Mobile \n");
  fprintf (fp,"#               Sprint\n");
  fprintf (fp,"#\n");
  fprintf (fp,"# Please note... spaces are NOT squashed afte the = sign.  Be careful \n");
  fprintf (fp,"# with your true/false values \n");
  fprintf (fp,"#\n");
  fprintf (fp,"#\n");
  fprintf (fp,"## general information\n");
  fprintf (fp,"#\n");
  fprintf (fp,"#\n");
  fprintf (fp,"[general]\n");
  fprintf (fp,"tag=SqwaQ Board 4\n");
  fprintf (fp,"num_modems=6\n");
  fprintf (fp,"max_net_ins=10i\n");
  fprintf (fp,"num_retry=5\n");
  fprintf (fp,"monitor_frequency=1\n");
  fprintf (fp,"monitor_data_size=0.3\n");


}


int config_procedure (FILE *fp,int modem_num, char *oper, char *apn){
	 char buf [1024];
	 int size = 1024;
	 char cmd [128];
	 //char imsi [128];
	 char imei [128];
	 char iccid [128];
	 char fsn [128];
	 bool found = false;

	 sprintf (cmd,"ATE1 \r");
         if (exec_cmd (modem_num,cmd,buf,size) < 0){
		 return -1;
	 }
	 sprintf (cmd,"ATI3 \r");
	 for (int i = 0;i< ITER;i++){
         	exec_cmd (modem_num,cmd,buf,size);
		sleep (1);
	        //sprintf (cmd,"AT \r");
         	//exec_cmd (modem_num,cmd,buf,size);
		//sleep (1);
         	exec_cmd (modem_num,cmd,buf,size);
	 	if (get_imei(buf,imei) < 0){
			continue;
		}
		else{
			found = true;
			break;
		}
	 }
	 if (found){
	 	fprintf (fp,"define(`MODULEIMEI%02d',`%s')\n",modem_num,imei);
		fflush (fp);
	 }
	 else{
	   fprintf (fp,"define(`MODULEIMEI%02d',`%s')\n",modem_num,"undefined");
           fflush (fp);
	 }
	 //printf ("after imei \n");
	 get_fsn(buf,fsn);
	 fprintf (fp,"define(`MODULEFSN%02d',`%s')\n",modem_num,fsn);
	 sleep (1);
	 found = false;
	 sprintf (cmd,"AT+ICCID \r");
	 for (int i = 0;i< ITER;i++){
         	exec_cmd (modem_num,cmd,buf,size);
	 	if (get_iccid(buf,iccid) < 0){
			continue;
		}
		else{
			found = true;
			break;
		}
	 }
	 if (found){
	 	fprintf (fp,"define(`ICCID%02d',`%s')\n",modem_num,iccid);
		fflush (fp);
	 }
	 else{
	   fprintf (fp,"define(`ICCID%02d',`%s')\n",modem_num,"undefined");
           fflush (fp);
	 }
	 //printf ("after imei \n");
	 if (intfs[modem_num-1] != NULL){
	 	fprintf (fp,"define(`INTEF%02d',`%s')\n",modem_num,intfs[modem_num-1]);
		fflush (fp);
	 }
	 else
	 {
	 	fprintf (fp,"define(`INTEF%02d',`%s')\n",modem_num,"undefined interface");
		fflush (fp);
	 }
	 sleep (1);
	 fprintf (stderr,"modem %d is completed with IMEI %s \n",modem_num,imei);
    return 0;
}

void help (){

 fprintf (stderr, "-help                 # help function\n");
 fprintf (stderr, "-tag <tag value>      # the board tag value\n");

}

int main(int argc, char *argv[]) {
	int i = 0;
	char tag [32] = "undefined"; //VzW 
	
        //printf ("argc %d \n",argc);
	// AT+CGDCONT=1,"IP","syniverseus","0.0.0.0",0,0,0,0
	//
	//
	
        while  (i<argc){
       //printf (" index %d %s\n",i,argv[i]);    
       		if (!strcmp ("-tag",argv[i])){
	       		strcpy (tag,argv[i+1]);
	       		i ++;
	      		continue;
		}
       		if (!strcmp ("-help",argv[i])){
			help ();
			exit (0);
		}
       		i++;
     	  }
        fprintf (stdout,"define(`BOARDTAG',`%s')\n",tag);
       get_interfaces (intfs);
       //print_header (stderr);
     for (int i = 0;i< MODEM_NUM ;i++)
       if (config_procedure (stdout,i+1,"Verizon","vzwinternet") < 0){
	       exit (0);
       }

	      
};
