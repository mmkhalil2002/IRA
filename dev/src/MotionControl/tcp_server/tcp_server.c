#include "robot_common.h"
#include "robot_motion.h"
#include "robot_comm.h"
#include "robot_sensors.h"
#include "robot_memory_mgt.h"
#include <pthread.h>

#define DIST_DELAY      250
#define INFINIT         40 // cm
#define SERVER_PORT     4950 
#define CLIENT_PORT     4951 

#define F_TRIG 10 // Green front
#define F_ECHO 11 //Yellow front

#define SL_TRIG 28 // Green  side
#define SL_ECHO 29 //Yellow side

#define B_TRIG 26 // Green  back
#define B_ECHO 27 //Yellow  back

#define SR_TRIG 24 // Green  R side
#define SR_ECHO 25 //Yellow  R side

#define FRONT_COLLISION_DIST         10    //cm
#define LEFT_COLLISION_DIST         10    //cm
#define RIGHT_COLLISION_DIST         10    //cm
#define COLLISION_ROTATE_ANGLE       5   // example repeat 5 timws
#define COLLISION_SPEED             130   
#define COLLISION_GOBACK_DELAY      250   
#define COLLISION_GOFORARD_DELAY    250   

#define DELAY_TURN   500
#define NUM_CLIENTS  100

unsigned int room_no=0;
int f_dist=INFINIT, b_dist=INFINIT,sr_dist=INFINIT,sl_dist=INFINIT;
int speed = INIT_SPEED;
int motion_port = 4099;
bool client_status [NUM_CLIENTS];
bool reverse_continue = true;
bool replay_continue = true;

//	char *client_name = "10.200.79.184";

/*
const int in0 = 0; // pin 11
const int in2 = 2; // pin 13
const int in3 = 3; // pin 15
const int in4 = 4; // pin 16

void go_left_test (){

	digitalWrite(in0, HIGH);
 	digitalWrite(in2, LOW);
	digitalWrite(in3, LOW);
	digitalWrite(in4, HIGH);
	delay (1);
    }
    */



void send_reply (int sockfd,struct sockaddr_in *clientaddr ){
      char ackbuf [128];
      sprintf (ackbuf,"%u",ACK);
      char *ip4_addr = NULL;
      int port = CLIENT_PORT;
      get_ip4_addr (clientaddr,&ip4_addr);
      get_port (clientaddr,&port);
      //printf ("main: addr %s port %d \n",ip4_addr,port);
      //msg_send (buf,sockfd,ip4_addr,port);      
      msg_send_reply (sockfd,clientaddr,ackbuf,1);      
}



void *check_dist_SR(void *vargp){ 
	printf ("CHECK SR \n");
	while (1){
  	 sr_dist=get_dist_CM(SR_TRIG,SR_ECHO);
         //printf("SR Distance: %dcm\n", sr_dist);
   	 delay(DIST_DELAY);
	}
}

void *check_dist_B(void *vargp){ 
	printf ("CHECK B \n");
	while (1){
  	 b_dist=get_dist_CM(B_TRIG,B_ECHO);
         //printf(" Back Distance: %dcm\n", b_dist);
   	 delay(DIST_DELAY);
	}
}

void *check_dist_SL(void *vargp){ 
	printf ("CHECK SL \n");
	while (1){
  	 sl_dist=get_dist_CM(SL_TRIG,SL_ECHO);
         //printf("SL Distance: %dcm\n", sl_dist);
   	 delay(DIST_DELAY);
	}
}

void *check_dist_F(void *vargp){ 
	printf ("CHECK F \n");
	while (1){
  	 f_dist=get_dist_CM(F_TRIG,F_ECHO);
         //printf("F Distance: %dcm\n", f_dist);
   	 delay(DIST_DELAY);
	}
}

int left_count = 0;
int right_count = 0;
void *reverse_motion(void *vargp){ 
        MemoryCellType cell;
        reverse_open_memory_file_rd (room_no);
	printf (" REVERSE MOTION \n");
        right_count = 0;
         left_count = 0;
	replay_continue = false;
	printf ("reverse motion flag %d \n",reverse_continue);
	while (reverse_continue){
	   int ret = reverse_read_cell (&cell);
	   if (!ret)
	   {
		   stop ();
		   break;
	   }
          printf ("CELL INFO  duration %d action type %x speed %d \n",cell.actionDuration,cell.actionType,cell.speed);
	  int cmd = cell.actionType;
	  int speed = cell.speed;
	  int duration = cell.actionDuration;
          switch( cmd )
           {
            case MV_FRWD:
		left_count = 0;
		right_count = 0;
                go_back (speed);
                printf ("reverse: MV BACKWORD %x\n",cmd);
		delay (duration);
            break;
            case MV_BKWD:
		left_count = 0;
		right_count = 0;
                go_forward (speed);
                printf ("reverse:MV FORWRD  %x\n",cmd);
		delay (duration);
            break;

            case MV_LEFT:
                go_right (speed);
                printf ("reverse: move left  %x ******  counter %d\n",cmd,right_count ++);
		delay (DELAY_TURN);
            break;
            case MV_RGHT:
                go_left (speed);
		delay (DELAY_TURN);
                printf ("reverse: move right  %x *****  counter %d\n",cmd,left_count ++);
            break;
            case SPEED:
                ctr_speed (speed);
                printf ("reverse:speed up down CMD %x  SPEED %d\n",cmd,speed);
            break;
            case STOP:
                stop ();
                printf ("reverse:stop  %x\n",cmd);
            break;
            default:
                //stop ();
                printf ("bad CMD  %x\n",cmd);
            break;
        }
    }
  right_count = 0;
  left_count = 0;
  return 0;
}



int indx = 0;
void *collision_control(void *vargp){ 
   	printf ("collision control \n");
	while (1){
	if (b_dist < 10){
              printf("****************  Back Distance: index %d  %dcm\n", indx,b_dist);
	      b_dist = INFINIT;
           }		
	if (f_dist < FRONT_COLLISION_DIST){
              printf("**************** Forw Distance: index %d  %dcm\n", indx,f_dist);
              go_back (COLLISION_SPEED);
	      delay (COLLISION_GOBACK_DELAY);
	      stop ();
	      if (sl_dist <= LEFT_COLLISION_DIST && sr_dist >=  INFINIT){ 
		 // rotate 45 degrees
	          for (int i=0; i < COLLISION_ROTATE_ANGLE;i++){
                      go_right (COLLISION_SPEED);
	          }
                 go_forward (speed);
		 while (1){

		    if (sl_dist <= LEFT_COLLISION_DIST && f_dist > FRONT_COLLISION_DIST){
	               delay (COLLISION_GOFORARD_DELAY);
		       continue;
		    }

		    if (sl_dist >= INFINIT){
	                   stop ();
		           // rotate 45 degrees
	                   for (int i=0; i < COLLISION_ROTATE_ANGLE;i++){
                  		 go_left (COLLISION_SPEED);
	          	   }
                 	  go_forward (speed);
	                  f_dist = INFINIT;
			  break;
			}

		    if ( f_dist <= FRONT_COLLISION_DIST){
	                stop ();
		        break;
		    }
		 }
	      } 
	      if (sr_dist <= LEFT_COLLISION_DIST && sl_dist >=  INFINIT){ 
		 // rotate 45 degrees
	          for (int i=0; i < COLLISION_ROTATE_ANGLE;i++){
                      go_left (COLLISION_SPEED);
	          }
                 go_forward (speed);
		 while (1){

		    if (sr_dist <= RIGHT_COLLISION_DIST && f_dist > FRONT_COLLISION_DIST){
	               delay (COLLISION_GOFORARD_DELAY);
		       continue;
		    }

		    if (sr_dist >= INFINIT){
	                   stop ();
		           // rotate 45 degrees
	                   for (int i=0; i < COLLISION_ROTATE_ANGLE;i++){
                  		 go_right (COLLISION_SPEED);
	          	   }
                 	  go_forward (speed);
	                  f_dist = INFINIT;
			  break;
			}

		    if ( f_dist <= FRONT_COLLISION_DIST){
	                stop ();
		        break;
		    }
		 }
           }		
	if (sl_dist < 10){
              printf("*************** SL Distance: index %d  %dcm\n",indx, sl_dist);
	      sl_dist = INFINIT;
	}
	if (sr_dist < 10){
              printf("************** SR Distance: index %d %dcm\n", indx,sr_dist);
	      sr_dist = INFINIT;
	}
        indx ++;
	      //usleep (50);
	      delay (10);
	}
    }
}

void *replay_motion(void *vargp){ 
        MemoryCellType cell;
        open_memory_file_rd (room_no);
	printf (" REPLAY MOTION \n");
        right_count = 0;
   	left_count = 0;
	reverse_continue = false;
	printf ("replay motion flag %d \n",replay_continue);
	while (replay_continue){
	   int ret = read_cell (&cell);
	   printf (" retrurn value read %d\n",ret);
	   if (!ret)
		   break;
          printf ("CELL INFO  duration %d action type %x speed %d \n",cell.actionDuration,cell.actionType,cell.speed);
	  int cmd = cell.actionType;
	  int speed = cell.speed;
	  int duration = cell.actionDuration;
          switch( cmd )
           {
            case MV_FRWD:
		left_count = 0;
		right_count = 0;
                go_forward (speed);
                printf ("replay: MV FRWD %x\n",cmd);
		delay (duration);
            break;
            case MV_BKWD:
		left_count = 0;
		right_count = 0;
                go_back (speed);
                printf ("replay:iMV BCWD  %x\n",cmd);
		delay (duration);
            break;

            case MV_LEFT:
                go_left (speed);
		delay (DELAY_TURN);
                printf ("replay: move left  %x *******  counter %d\n",cmd,left_count++);
            break;
            case MV_RGHT:
                go_right (speed);
		delay (DELAY_TURN);
                printf ("replay: move right  %x ******   counter %d\n",cmd,right_count++);
            break;
            case SPEED:
                ctr_speed (speed);
                printf ("replay:speed up down CMD %x  SPEED %d\n",cmd,speed);
            break;
            case STOP:
                stop ();
                printf ("replay:stop  %x\n",cmd);
            break;
            default:
                //stop ();
                printf ("bad CMD  %x\n",cmd);
            break;
        }
    }
   left_count = 0;
   right_count = 0;
  return 0;
}



//void *control_motion(void *vargp){ 
//

void *exec_motion(void *ptr){ 

    int socket = *(int *)ptr;
    char buffer[4];
    int bytes;
    fprintf (stderr, "<<<<<<< recv ctr data >>>>>>>>>>>>>>\n"); fflush (stderr);

      while(client_status[socket])
        {
            bytes = recv(socket, buffer, 4, MSG_DONTWAIT);
            usleep (100);
            //printf ("<<<<<<<<<  exec motion %d %x   %x \n", bytes,buffer[0], buffer [1]);
            if (bytes < 0){
                        //std::cerr << "<<< ctr error receiving from client >>>" << std::endl;
                        //fflush (stderr);
                        //break;
              }
             else if (bytes > 0){
             int cmd = buffer[0];
            	printf ("cmd  %x\n",cmd);
       		 switch( cmd )
                  {
            		case MV_FRWD:
                		go_forward (speed);
                		printf ("move forward  %x\n",cmd);
           			 break;

            		case MV_BKWD:
                		go_back (speed);
                		printf ("move back  %x\n",cmd);
            		break;
           		 case MV_LEFT:
                		go_left (speed);
                		printf ("move left  %x\n",cmd);
            		break;
            		case MV_RGHT:
                		go_right (speed);
                		printf ("move right  %x\n",cmd);
            		break;
			 case F_CAMERA_LFT:
  		              f_camera_left ();
			      printf (" front camera left %x \n",cmd);
            		break;
            		case F_CAMERA_RGT:
                	 	f_camera_right ();
			      printf (" front camera right %x \n",cmd);
            		break;
           		 case F_CAMERA_UP:
                        	f_camera_up ();
			      printf (" front camera up %x \n",cmd);
            		break;
            		case F_CAMERA_DN:
                		f_camera_dn ();
			      printf (" front camera down %x \n",cmd);
            		break;
            		case B_CAMERA_LFT:
                		b_camera_left ();
			        printf (" back camera left %x \n",cmd);
            		break;
            		case B_CAMERA_RGT:
                		b_camera_right ();
			        printf (" back camera right %x \n",cmd);
            		break;
            		case B_CAMERA_UP:
                		b_camera_up ();
			        printf (" back camera up %x \n",cmd);
            		break;
			 case B_CAMERA_DN:
       				b_camera_dn ();
			        printf (" back camera down %x \n",cmd);
            		break;
            		case ROOM_NO:
                		printf ("buf read %d\n",buffer[1]);
                		room_no = buffer [1];
                		printf ("ROOM NO CMD %x  ROOMNO %d\n",cmd,room_no);
			break;
            		case START_RECORD:
                		start_recording (room_no);
                		printf ("START RECORD %d\n",room_no);
			break;
           		 case END_RECORD:
                		stop_recording ();
                		printf ("END RECORD %d\n",room_no);
			break;
            		case UP_SPEED:
            		case DN_SPEED:
           		 {
               			 unsigned int t_speed=0;
                		t_speed = buffer [1];
               			 ctr_speed (t_speed);
               			 printf ("speed up down CMD %x  SPEED %d\n",cmd,t_speed);
				speed = t_speed;
            		};
            		break;
            		case STOP:
                		stop ();
                		printf ("stop  %x\n",cmd);
            		break;
            		case REPLAY_MOTION:
	    		{
	        		pthread_t replay_motion_id=0; 
				replay_continue = true;
	       	         	 pthread_create(&replay_motion_id, NULL, replay_motion, NULL); 
	        		pthread_join(replay_motion_id, NULL); 
	    		}
            		break;
           		 case RESTART:
			   system ("shutdown -fr now");
			   break;
           		 case REVERSE_MOTION:
	    		{
	        		pthread_t reverse_motion_id=0; 
				reverse_continue = true;
	       			 pthread_create(&reverse_motion_id, NULL, reverse_motion, NULL); 
	        		pthread_join(reverse_motion_id, NULL); 
	    		}
           		 break;


           		 default:
                	//stop ();
               		 printf ("bad CMD  %x\n",cmd);
           		 break;
        	}

	     }
    }
   right_count = 0;
   left_count = 0;
   printf ( "<<<<<<<  moton control  quitting...>>>>>>>\n");
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
        fprintf (stderr," disconnected \n");
        return -1; // disconnected, I'm guessing this is definitely WRONG.
    }
    else if(result > 0 && FD_ISSET(sockfd, &fd))
    {
        fprintf (stderr," bytes available \n");
        return 0; // bytes available. successful
    }
    else if(!result)
     {
        fprintf (stderr," timeout \n" );
        return -1; // timeout
     }

    fprintf (stderr," select call failed \n"); 
    return -1; // select() call failed.
}


void sig_handler(int signo)
{
  stop ();
  printf ("received signal \n");
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

                        }
                 }
         }
    }
  if (signo == SIGINT)
  {
    printf("received SIGINT\n");
    exit (0);
  }
}


void *motion_server (void *ptr)
{

   int                 localSocket;
    struct  sockaddr_in localAddr,
                        remoteAddr;

    int remoteSocket = 0;
        // A long long wait so that we can easily issue a signal to this process
    int addrLen = sizeof(struct sockaddr_in);
     pthread_t motion_thread;

    localSocket = socket(AF_INET , SOCK_STREAM , 0);
    printf ( " local socket %d " , localSocket );
    if (localSocket == -1){
         perror("socket() call failed!!");
    }
    printf (" local socket id %d \n",localSocket);

    char *addr = get_ip_address ("eth0");
    if (addr == 0)
       return NULL;

    localAddr.sin_family = AF_INET;
    //localAddr.sin_addr.s_addr = INADDR_ANY;
    localAddr.sin_addr.s_addr =  inet_addr(addr);
    localAddr.sin_port = htons( motion_port );

    if( bind(localSocket,(struct sockaddr *)&localAddr , sizeof(localAddr)) < 0) {
         perror("Can't bind() socket");
         exit(1);
    }

    // Listening
    listen(localSocket , 3);

   printf ( "Waiting for connections. Server Port: %d\n", motion_port);

    //accept connection from an incoming client
    motor_init ();
    while(1){
        printf (" before ctr accept \n");
        remoteSocket = accept(localSocket, (struct sockaddr *)&remoteAddr, (socklen_t*)&addrLen);
        printf ("****************  remote ctr socket %d\n", remoteSocket);

        if (remoteSocket < 0) {
            perror("accept failed!");
            exit(1);
        }

        client_status[remoteSocket] = true;
        printf ("ctr Connection accepted \n");
        pthread_create(&motion_thread, NULL, exec_motion, &remoteSocket);
        //recv_ctr_data(&remoteSocket);
    }

    // wait for threads to finish up
    pthread_join(motion_thread, NULL);
    return 0;
}


int main(int argc, char* argv[])
{
	pthread_t control_motion_id=0; 
	//pthread_t sl_dist_id=0; 
	//pthread_t sr_dist_id=0; 
	//pthread_t b_dist_id=0; 
	pthread_t f_dist_id=0; 
	//pthread_t collision_control_id=0; 
        int c=0;
        while ((c = getopt (argc, argv, "p")) != -1)
          {
            switch (c)
             {
                case 'p':
                  motion_port = atoi (optarg);
                   printf ("motion port %d \n",  motion_port);
                 break;
                case ':':       /* -p without operand */
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


    	setup_dist_sensor(F_TRIG,F_ECHO);
    	setup_dist_sensor(B_TRIG,B_ECHO);
    	setup_dist_sensor(SL_TRIG,SL_ECHO);
    	setup_dist_sensor(SR_TRIG,SR_ECHO);
        //control_motion(NULL); 
	printf("Before Thread\n"); 
	pthread_create(&control_motion_id, NULL, motion_server, NULL); 
	pthread_create(&f_dist_id, NULL, check_dist_F, NULL); 
	//pthread_create(&b_dist_id, NULL, check_dist_B, NULL); 
	//pthread_create(&sl_dist_id, NULL, check_dist_SL, NULL); 
	//pthread_create(&sr_dist_id, NULL, check_dist_SR, NULL); 
	//pthread_create(&collision_control_id, NULL, collision_control, NULL); 
	pthread_join(f_dist_id, NULL); 
	//pthread_join(b_dist_id, NULL); 
	//pthread_join(sl_dist_id, NULL); 
	//pthread_join(sr_dist_id, NULL); 
	//pthread_join(collision_control_id, NULL); 
	pthread_join(control_motion_id, NULL); 
	printf("After Thread\n"); 
	exit(0);
} 
