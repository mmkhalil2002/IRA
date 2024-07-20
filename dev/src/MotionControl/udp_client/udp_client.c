
#include "robot_common.h"
#include "robot_comm.h"
#include "robot_motion.h"
#define SERVER_PORT     4950 
#define CLIENT_PORT     4951 
int sockfd =0;
unsigned long speed = 40;
unsigned char speed_flag = false;
unsigned char room_flag = false;
char buf [128];

int main() { 

    //struct sockaddr_in     servaddr; 
    struct sockaddr_in serveraddr;
    unsigned char recv_buf [128];
    unsigned char cmd = STOP;
    char *client_name = NULL;  
    char *server_name = NULL;  
    server_name = get_host_ip ();  
    printf ("check1 \n");
    client_name = get_host_ip ();  
    printf ("server name %s client name %s \n",server_name,client_name);
    sockfd = init_sock (client_name, CLIENT_PORT);
    int ch,c;
    initscr();
    cbreak();
    noecho();
    ch = 's';
    while( ch != 'q')
    {
        switch( c = getch() )
        {
            case 'f':
		speed_flag = false;
		room_flag  = false;
	        printf ("enter char %c\n",c);
                cmd = MV_FRWD;
            break;

            case 'b':
		speed_flag = false;
		room_flag  = false;
	        printf ("enter char %c\n",c);
                cmd = MV_BKWD;
            break;

            case 'l':
		speed_flag = false;
		room_flag  = false;
	        printf ("enter char %c\n",c);
                cmd = MV_LEFT;
            break;
            case 'r':   // right turn
		speed_flag = false;
		room_flag  = false;
	        printf ("enter char %c\n",c);
                cmd = MV_RGHT;
            break;
            case 'k':   //  start record
		speed_flag = false;
		room_flag  = false;
	        printf ("enter char %c\n",c);
                cmd = START_RECORD;
            break;
            case 'y':   // replay motion 
		speed_flag = false;
		room_flag  = false;
	        printf ("enter char %c\n",c);
                cmd = REPLAY_MOTION;
		break;
            case 'v':   // reverse motion
		speed_flag = false;
		room_flag  = false;
	        printf ("enter char %c\n",c);
                cmd = REVERSE_MOTION;
		break;
            case 't':   //  terminate record
		speed_flag = false;
		room_flag  = false;
	        printf ("enter char %c\n",c);
                cmd = END_RECORD;
            break;
            case 'u':
	    {
	       speed_flag = true;
	       room_flag = false;
	       if (speed < 255){
		       speed += 20;
		       if (speed > 255)
			       speed = 255;
	          };
	        printf ("enter char %c speed %ld\n",c,speed);
                cmd = UP_SPEED;
	    }
            break;
            case 'd':
	       speed_flag = true;
	       room_flag = false;
	       speed -= 20;
	       if (speed < INIT_SPEED)
		       speed = INIT_SPEED;
	        printf ("enter char %c speed %ld\n",c, speed);
                cmd = DN_SPEED;
            break;
            case 's':
	       speed_flag = true;
	       room_flag = false;
	        printf ("enter char %c\n",c);
                cmd = STOP;
            break;

            case 'o':  //select room
	    	room_flag = true;
	    	speed_flag = false;
            	cmd = ROOM_NO;
            break;
            default:
	        room_flag = false;
	        speed_flag = false;
	        //printf ("enter char %c\n",c);
                cmd = STOP;                 
            break;
        }
	sprintf (buf,"%c",cmd);
	printf ("char sent %u \n",buf[0]);
        msg_snd_to_dist (sockfd,server_name,SERVER_PORT,buf,1);
	if (speed_flag){
	   unsigned long speed_n = htonl(speed);
           msg_snd_to_dist (sockfd,server_name,SERVER_PORT,(char *)&speed_n,4);
	}
	if (room_flag){
	    unsigned long room_no;
            buf [0] = getch(); 
	    buf [1] = '\0';
	    room_no = atoi (buf);
	    room_flag = false;
	   room_no = htonl(room_no);
            msg_snd_to_dist (sockfd,server_name,SERVER_PORT,(char *)&room_no,4);
	    printf ("ROOM NUM %ld NO %s\n",room_no,buf);
	}
        msg_rcv_from_server (sockfd,&serveraddr,recv_buf, 1);
    	//msg_rcv_ext (sockfd,&clientaddr,recv_buf,1);      
        printw("char= %u %u\n",c,cmd);
    }
    endwin();
}  
