#ifndef _ROBOT_COMM_H_
#define _ROBOT_COMM_H_
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <netdb.h> 
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>  
#include <ncurses.h>
#include <pthread.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <net/if.h>


//#define SERVER_PORT     4950 
#define MAXLINE 1024 

char* get_host_ip ();
int init_client ();
int msg_rcv_from_server (int sockfd,struct sockaddr_in *serveraddr,unsigned char *buffer, int len);
void msg_snd_to_dist (int sockfd,char *dist_name,int dist_port,char *msg,int len);
int msg_rcv_from_client(int sockfd, struct sockaddr_in *clientaddr,unsigned char *buffer,int buflen);
//void msg_snd (int sockfd,char *msg, int len);
//int msg_rcv (int sockfd);
int init_sock (char *local_addr, int local_port);
int get_ip4_addr (struct sockaddr_in *addr,char **ip4_addr);
int get_port (struct sockaddr_in *addr,int *port);
int msg_send_reply (int sockfd,struct sockaddr_in *clientaddr,char *msg, int len);
char *get_ip_address(const char *intf);
#endif // _ROBOT_COMM_H_
