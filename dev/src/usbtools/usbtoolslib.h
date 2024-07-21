#include <errno.h>
#include <fcntl.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>  //For standard things
#include <stdlib.h> //malloc
#include <string.h> //strlen
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

int get_next_line_from_buf(char *, char *, int, int);
int isStringExist(char *, char *);
int clean_output(char *, char *, int, int);
int read_usb(int, char *, int);
int write_usb(int, char *);
int execute_at_cmd(int, char *, char *, int); /* I - Serial port file */
int init_modem(int);
int read_gps_port(FILE *, char *, int);
int read_gps(int, char *, int);
int exec_cmd_ext(char *, char *, char *, int);
int exec_cmd(int, char *, char *, int);
int exec_raw_cmd(char *dev, char *cmd, char *buf, int size); 
int  execute_raw_cmd(int fd, char *cmd, char *buf,int size);
int isStringExist(char *, char *);
int get_imei(char *, char *);
int get_fsn(char *, char *);
int get_meid(char *, char *);
int get_manufacturer(char *, char *);
int get_model(char *, char *);
int get_revision(char *, char *);
int get_imsi(char *, char *);
int get_iccid(char *, char *);
int get_creg_status(char *);
char* get_ipaddr(char *interface);
int get_lteband(char *buf, char *lteband);
int get_ltebandwidth(char *buf, char *ltebandw);
int get_temperature(char *buf, char *temp) ;
void get_interfaces (char *interface[]);
FILE *open_gps(int modem_num);
void close_gps (FILE *fp);
