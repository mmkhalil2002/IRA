
#include "robot_comm.h" 

//int sockfd;
//char *server_name = "10.200.79.184";  
char *server_name = NULL;  
//char *client_name = NULL;  
//



/*C program to get IP Address of Linux Computer System.*/
char *get_ip_address(const char *intf)
{
    char *ip_address = malloc (25);
    int fd;
    struct ifreq ifr;

    /*AF_INET - to define network interface IPv4*/
    /*Creating soket for it.*/
    fd = socket(AF_INET, SOCK_DGRAM, 0);

    /*AF_INET - to define IPv4 Address type.*/
    ifr.ifr_addr.sa_family = AF_INET;

    /*eth0 - define the ifr_name - port name
    where network attached.*/
    memcpy(ifr.ifr_name, intf, IFNAMSIZ - 1);

    /*Accessing network interface information by
    passing address using ioctl.*/
    ioctl(fd, SIOCGIFADDR, &ifr);

    /*closing fd*/
    close(fd);
                     /*Extract IP Address*/
    strcpy(ip_address, inet_ntoa(((struct sockaddr_in*)&ifr.ifr_addr)->sin_addr));

    printf("System IP Address is: %s\n", ip_address);

   printf("System IP Address is: %s\n", ip_address);

    return ip_address;
}

char* get_host_ip (){
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    char *addr;

    getifaddrs (&ifap);
    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr->sa_family==AF_INET) {
            sa = (struct sockaddr_in *) ifa->ifa_addr;
            addr = inet_ntoa(sa->sin_addr);
	    if (strcmp ("lo",ifa->ifa_name) != 0){
               printf("Interface: %s\tAddress: %s \n", ifa->ifa_name, addr);
	       return (strdup (addr));
	    }
        }
    }

    freeifaddrs(ifap);
    return 0;
}

int get_ip4_addr (struct sockaddr_in *addr,char **ip4_addr){
        *ip4_addr = strdup (inet_ntoa(addr->sin_addr));
	//printf ("addr %s \n",*ip4_addr);
	return (0);
}

int get_port (struct sockaddr_in *addr,int *port){
        *port =  ntohs(addr->sin_port);
	//printf ("port %d \n", *port);
	return (0);
 }


int msg_send_reply (int sockfd,struct sockaddr_in *clientaddr,char *msg, int len){

    //struct sockaddr_in     clientaddr; 
    //memset(&clientaddr, 0, sizeof(clientaddr)); 
    //int sockfd = init_sock(src_addr,src_port);  
   //printf(" addr %s port %d \n",dst_addr,dst_port); 
    // Filling server information 
    sendto(sockfd, (const char *)msg, len, 
        MSG_CONFIRM, (const struct sockaddr *) clientaddr,  
            sizeof(*clientaddr)); 
      printf("ACK message sent. %u\n",msg[0]); 
      return 0;
}



int init_sock (char *local_addr, int local_port){
    //int s_sockfd;
    struct sockaddr_in sockaddr; 
    int option = 1;
    int sockfd;
    // Creating socket file descriptor 
    if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } 
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option)); 
    memset(&sockaddr, 0, sizeof(sockaddr)); 
      
    // Filling server information 
    sockaddr.sin_family    = AF_INET; // IPv4 
    sockaddr.sin_addr.s_addr = inet_addr(local_addr);      
    sockaddr.sin_port = htons(local_port); 
      
    // Bind the socket with the server address 
    if ( bind(sockfd, (const struct sockaddr *)&sockaddr, sizeof(sockaddr)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
     return (sockfd); 
}


int msg_rcv_from_server (int sockfd,struct sockaddr_in *serveraddr,unsigned char *buffer, int len){

   // struct sockaddr_in     servaddr; 
    socklen_t  addrlen; 
    memset(serveraddr, 0, sizeof(struct sockaddr_in)); 
    int n; 
    addrlen = sizeof (struct sockaddr_in);
    n = recvfrom(sockfd, (char *)buffer, len,  
                MSG_WAITALL, (struct sockaddr *) &serveraddr, 
                &addrlen); 
    return n;

}

void msg_snd_to_dist (int sockfd,char *dist_name,int dist_port,char *msg,int len){

    struct sockaddr_in     distaddr; 
    socklen_t  addrlen; 
    memset(&distaddr, 0, sizeof(distaddr)); 
    // Filling server information 
    distaddr.sin_family = AF_INET; 
    distaddr.sin_port = htons(dist_port); 
    distaddr.sin_addr.s_addr = inet_addr(dist_name);      
    addrlen = sizeof (struct sockaddr_in);
      
    sendto(sockfd, (const char *)msg, len, 
        MSG_CONFIRM, (const struct sockaddr *) &distaddr,  
            addrlen); 
      //printf("Hello message sent.\n"); 
}

int msg_rcv_from_client(int sockfd, struct sockaddr_in *clientaddr,unsigned char *buffer,int buflen){

    //struct sockaddr_in clientaddr;
    int n=0; 
    socklen_t  len; 
    memset(clientaddr, 0, sizeof(struct sockaddr_in)); 
    //int sockfd = init_sock(src_addr,src_port);  
    len = sizeof (struct sockaddr_in);
    n = recvfrom(sockfd, (char *)buffer, buflen,  
                MSG_WAITALL, (struct sockaddr *) clientaddr, 
                &len); 
    return n;
}


