
#include "usbtoolslib.h"
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
#include <dirent.h>
#include <errno.h>

#define LINE_SIZE 128
#define MODEM_NUM 6
#define bool  unsigned char
#define  true   1
#define  false   0
bool old_serial = false;
// char obuf[2048];  /* Input buffer */
// char clean_buf[2048];  /* eanbuffer */

int read_file(char *file_name,char *devices[],int size);

unsigned char ctl_devices_read = 0; 
char *ctl_dev[MODEM_NUM];
char *gps_dev[MODEM_NUM];
//char *ctl_dev[MODEM_NUM] = {"/dev/ttyUSB2",  "/dev/ttyUSB5",  "/dev/ttyUSB8",
//                    "/dev/ttyUSB11", "/dev/ttyUSB14", "/dev/ttyUSB17"};
//char *gps_dev[MODEM_NUM] = {"/dev/ttyUSB1",  "/dev/ttyUSB4",  "/dev/ttyUSB7",
//                    "/dev/ttyUSB10", "/dev/ttyUSB13", "/dev/ttyUSB16"};

int exec_raw_cmd(char *dev, char *cmd, char *buf, int size) {
  int fd;
  fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1) {
    perror(dev);
    exit(-1);
  } else {
    // setvbuf(fd, NULL, _IONBF, 0);
    init_modem(fd);
    // flush_usb (fd);
    execute_raw_cmd(fd, cmd, buf, size); /* I - Serial port file */
    close(fd);
    return 0;
  }
}

int /* O - 0 = MODEM ok, -1 = MODEM bad */
 execute_raw_cmd(int fd, char *cmd, char *buf,
                   int size) { /* I - Serial port file */

  // char ibuf[255];  /* Input buffer */
  int nbytes; /* Number of bytes read */
  int tries;  /* Number of tries so far */
  // printf("?? START command to be excuted %s\n", cmd);
  for (tries = 0; tries < 5; tries++) {
    // printf (" <<<<< no of retries %d\n",tries);
    nbytes = write_usb(fd, cmd);
    if (nbytes < strlen(cmd)) {
      printf("error: no of bytes written %d %d\n", nbytes, (int)strlen(cmd));
      continue;
    }
    // printf("no of bytes written %d %d\n", nbytes, (int)strlen(cmd));
    nbytes = read_usb(fd, buf, size);
    // printf( "obuf <?<?start string cmd = <%s> [ %s ] end string <cmd %s>>?>?>
    // %d\n\n",cmd,obuf,cmd,nbytes);
  }
  return 0;
 }

// AT+CIMI
int get_imsi(char *buf, char *imsi) {
  int index;
  char line[LINE_SIZE];
  index = 0;
  int i = 0;
  while ((index = get_next_line_from_buf(buf, line, LINE_SIZE, index)) >= 0) {
     if (i==1){
      sscanf(line, "%s", imsi);
      return (0);
     }
     else
       i++;
  }
  return (-1);
}

int get_iccid(char *buf, char *iccid) {
  int index;
  char tmp[64];
  char line[LINE_SIZE];
  index = 0;
  while ((index = get_next_line_from_buf(buf, line, LINE_SIZE, index)) >= 0) {
    int i = isStringExist(line, "ICCID: ");
    // printf ("line = %s index %d\n",line,i);
    if (i >= 0) {
      sscanf(line, "%s %s", tmp, iccid);
      // printf ("get_imei: $$$$$$$MATCH line <<%s>> %s \n",line,imei);
      return (0);
    }
  }
  return (-1);
}
// at!gpsloc
//
// at+creg?


// at!gpsloc

int get_altitude(char *buf, char *alt) {
  int index;
  char tmp[64];
  char line[LINE_SIZE];
  index = 0;
  while ((index = get_next_line_from_buf(buf, line, LINE_SIZE, index)) >= 0) {
    int i = isStringExist(line, "Altitude:");
    // printf ("line = %s index %d\n",line,i);
    if (i >= 0) {
      sscanf(line, "%s %s", tmp, alt);
      // printf ("get_imei: $$$$$$$MATCH line <<%s>> %s \n",line,imei);
      return (0);
    }
  }
  return (-1);
}
// at!gpsloc

int get_lat(char *buf, char *lat) {
  int index;
  char tmp[64];
  char line[LINE_SIZE];
  index = 0;
  while ((index = get_next_line_from_buf(buf, line, LINE_SIZE, index)) >= 0) {
    int i = isStringExist(line, "Lat:");
    // printf ("line = %s index %d\n",line,i);
    if (i >= 0) {
      sscanf(line, "%s %s", tmp, lat);
      // printf ("get_imei: $$$$$$$MATCH line <<%s>> %s \n",line,imei);
      return (0);
    }
  }
  return (-1);
}
//
//
// at!gpsloc

int get_lon(char *buf, char *lon) {
  int index;
  char tmp[64];
  char line[LINE_SIZE];
  index = 0;
  while ((index = get_next_line_from_buf(buf, line, LINE_SIZE, index)) >= 0) {
    int i = isStringExist(line, "Lon:");
    // printf ("line = %s index %d\n",line,i);
    if (i >= 0) {
      sscanf(line, "%s %s", tmp, lon);
      // printf ("get_imei: $$$$$$$MATCH line <<%s>> %s \n",line,imei);
      return (0);
    }
  }
  return (-1);
}

//
// at+creg?

int get_creg_status(char *buf) {
  int index;
  int status = 0;
  char line[LINE_SIZE];
  index = 0;
  while ((index = get_next_line_from_buf(buf, line, LINE_SIZE, index)) >= 0) {
    int i = isStringExist(line, ",");
    // printf ("line = %s index %d\n",line,i);
    if (i >= 0) {
      sscanf(&line[i + 1], "%d", &status);
      // printf ("get_imei: $$$$$$$MATCH line <<%s>> %s \n",line,imei);
      return (status);
    }
  }
  return (-1);
}

// use cmd ati3
int get_revision(char *buf, char *revision) {
  int index;
  char line[LINE_SIZE];
  // printf ("get_imei\n %s \n",buf);
  index = 0;
  while ((index = get_next_line_from_buf(buf, line, LINE_SIZE, index)) >= 0) {
    int i = isStringExist(line, "Revision");
    // printf ("line = %s index %d\n",line,i);
    if (i >= 0) {
      // remove manufacture at index 16
      strcpy(revision, &line[9]);
      // printf ("get_imei: $$$$$$$MATCH line <<%s>> %s \n",line,imei);
      return (0);
    }
  }
  return (0);
}

// use cmd ati3
int get_model(char *buf, char *model) {
  int index;
  // char tmp [64];
  char line[LINE_SIZE];
  // printf ("get_imei\n %s \n",buf);
  index = 0;
  while ((index = get_next_line_from_buf(buf, line, LINE_SIZE, index)) >= 0) {
    int i = isStringExist(line, "Model");
    // printf ("line = %s index %d\n",line,i);
    if (i >= 0) {
      // sscanf (line,"%s %s",tmp,model);
      strcpy(model, &line[7]);
      // printf ("get_imei: $$$$$$$MATCH line <<%s>> %s \n",line,imei);
      return (0);
    }
  }
  return (0);
}

// use cmd ati3
int get_manufacturer(char *buf, char *manuf) {
  int index;
  char line[LINE_SIZE];
  // printf ("get_imei\n %s \n",buf);
  index = 0;
  while ((index = get_next_line_from_buf(buf, line, LINE_SIZE, index)) >= 0) {
    int i = isStringExist(line, "Manufacturer");
    printf("*******line = %s index %d\n", line, i);
    if (i >= 0) {
      // remove manufacture at index 14
      strcpy(manuf, &line[14]);
      // printf ("get_imei: $$$$$$$MATCH line <<%s>> %s \n",line,imei);
      return (0);
    }
  }
  return (0);
}

// use cmd ati3
int get_meid(char *buf, char *meid) {
  int index;
  char tmp[64];
  char line[LINE_SIZE];
  // printf ("get_imei\n %s \n",buf);
  index = 0;
  while ((index = get_next_line_from_buf(buf, line, LINE_SIZE, index)) >= 0) {
    int i = isStringExist(line, "MEID:");
    // printf ("line = %s index %d\n",line,i);
    if (i >= 0) {
      sscanf(line, "%s %s", tmp, meid);
      // printf ("get_imei: $$$$$$$MATCH line <<%s>> %s \n",line,imei);
      return (0);
    }
  }
  return (-1);
}

// use cmd ati3
int get_fsn(char *buf, char *fsn) {
  int index;
  char tmp[64];
  char line[LINE_SIZE];
  // printf ("get_fsn\n %s \n",buf);
  index = 0;
  while ((index = get_next_line_from_buf(buf, line, LINE_SIZE, index)) >= 0) {
    int i = isStringExist(line, "FSN:");
    // printf ("line = %s index %d\n",line,i);
    if (i >= 0) {
      sscanf(line, "%s %s", tmp, fsn);
      // printf ("get_imei: $$$$$$$MATCH line <<%s>> %s \n",line,imei);
      return (0);
    }
  }
  return (-1);
}

// use cmd ati3
int get_imei(char *buf, char *imei) {
  int index;
  char tmp[64];
  char line[LINE_SIZE];
  //printf ("usbtools:get_imei\n %s \n",buf);
  index = 0;
  while ((index = get_next_line_from_buf(buf, line, LINE_SIZE, index)) >= 0) {
    int i = isStringExist(line, "IMEI:");
    //printf ("line = %s index %d\n",line,i);
    if (i >= 0) {
      sscanf(line, "%s %s", tmp, imei);
      //printf ("get_imei: $$$$$$$MATCH line <<%s>> %s \n",line,imei);
      return (0);
    }
  }
  return (0);
}

// use cmd ati3
int get_lteband(char *buf, char *lteband) {
  int index;
  int blank = 5;
  char line[LINE_SIZE];
  // printf ("get_lte_band \n %s \n",buf);
  index = 0;
  while ((index = get_next_line_from_buf(buf, line, LINE_SIZE, index)) >= 0) {
    int i = isStringExist(line, "LTE band");
    // printf ("line = %s index %d\n",line,i);
    int len = strlen("LTE band");
    if (i >= 0) {
      // remove manufacture at index 16
      strncpy(lteband, &line[i + len + blank], 5);
      printf("lteband: $$$$$$$MATCH line <<%s>> %s \n", line, lteband);
      return (0);
    }
  }
  return (0);
}

// use cmd ati3
int get_ltebandwidth(char *buf, char *ltebandw) {
  int index;
  int blank = 5;
  char line[LINE_SIZE];
  // printf ("get_lte_band \n %s \n",buf);
  index = 0;
  while ((index = get_next_line_from_buf(buf, line, LINE_SIZE, index)) >= 0) {
    int i = isStringExist(line, "LTE bw");
    // printf ("line = %s index %d\n",line,i);
    int len = strlen("LTE bw");
    if (i >= 0) {
      // remove manufacture at index 16
      strncpy(ltebandw, &line[i + len + blank], 11);
      printf("ltebandw: $$$$$$$MATCH line <<%s>> %s \n", line, ltebandw);
      return (0);
    }
  }
  return (0);
}

// use cmd ati3
int get_temperature(char *buf, char *temp) {
  int index;
  int blank = 2;
  char line[LINE_SIZE];
  // printf ("get_lte_band \n %s \n",buf);
  index = 0;
  while ((index = get_next_line_from_buf(buf, line, LINE_SIZE, index)) >= 0) {
    int i = isStringExist(line, "Temperature");
    // printf ("line = %s index %d\n",line,i);
    int len = strlen("Temperature");
    if (i >= 0) {
      // remove manufacture at index 16
      strncpy(temp, &line[i + len + blank], 8);
      printf("Temperature: $$$$$$$MATCH line <<%s>> %s \n", line, temp);
      return (0);
    }
  }
  return (0);
}

int get_emmstate(char *buf, char *emmstate) {
  int index;
  int blank = 2;
  char line[LINE_SIZE];
  // printf ("get_lte_band \n %s \n",buf);
  index = 0;
  while ((index = get_next_line_from_buf(buf, line, LINE_SIZE, index)) >= 0) {
    int i = isStringExist(line, "EMM state");
    // printf ("line = %s index %d\n",line,i);
    int len = strlen("EMM state");
    if (i >= 0) {
      // remove manufacture at index 16
      strncpy(emmstate, &line[i + len + blank], 40);
      printf("EMM state: $$$$$$$MATCH line <<%s>> %s \n", line, emmstate);
      return (0);
    }
  }
  return (0);
}

int get_next_line_from_buf(char *buf, char *line, int linesize,
                           int start_index) {
  int j = 0;
  int i = 0;
  for (i = start_index; i < strlen(buf); i++) {
    if (buf[i] != '\n' && buf[i] != '\r') {
      // printf ("call next line%d\n",i);
      line[j] = buf[i];
      // putc (buf[i],stdout);
      fflush(stdout);
      j++;
    } else {
      break;
    }
  }
  line[j] = '\0';
  // putc ('>',stdout);
  fflush(stdout);
  if (i < strlen(buf))
    while ((buf[i] == '\n' || buf[i] == '\r') && i < strlen(buf))
      i++;
  // printf ("index in fuction %d\n",i);
  if (i > 0) {
    // printf ("< %d stlen %d> ",i,strlen (buf));
    if (i < strlen(buf))
      return i;
    return -1;
  }
  printf("< -1 > ");
  return (-1);
}
int isStringExist(char *str, char *sub) {

  char *p1, *p2, *p3;
  int i = 0, j = 0, flag = 0;
  p1 = str;
  p2 = sub;

  for (i = 0; i < strlen(str); i++) {
    if (*p1 == *p2) {
      p3 = p1;
      for (j = 0; j < strlen(sub); j++) {
        if (*p3 == *p2) {
          p3++;
          p2++;
        } else
          break;
      }
      p2 = sub;
      if (j == strlen(sub)) {
        flag = 1;
        // printf("\nSubstring found at index : %d\n",i);
        break;
      }
    }
    p1++;
  }
  if (flag == 0) {
    // printf("Substring NOT found %s\n",sub);
    return -1;
  } else {
    // printf("Substring is found %s index %d %c %c \n",sub);
    return (i);
  }
}

// remove /n /r from the output.
int clean_output(char *obuf, char *clean_buf, int start_index, int end_index) {
  int i = 0;
  char line[128];
  int j = 0;
  int k = 0;
  int l = 0;
  i = start_index;
  j = 0;
  while (i < end_index) {
    if (obuf[i] != '\n' && obuf[i] != '\r') {
      line[j] = obuf[i];
      j++;
      i++;
      // printf (" i %d j %d \n",i,j);
      continue;
    } else {
      while (obuf[i] == '\n' || obuf[i] == '\r')
        i++;
    }
    line[j++] = '\n';
    line[j++] = '\0';
    // fprintf (stderr,"line is <%s> \n",line);
    // fflush (stderr);
    k = 0;
    while (k < strlen(line)) {
      clean_buf[l] = line[k];
      l++;
      k++;
    }
    j = 0;
    // printf ("start line %s\n",line);
  }
  k = 0;
  line[j++] = '\n';
  line[j++] = '\0';
  while (k < strlen(line)) {
    clean_buf[l] = line[k];
    l++;
    k++;
  }
  // printf ("end start line %s\n",line);
  clean_buf[i - start_index] = '\0';
  // printf ("clean_output: last index %d \n",i-start_index);
  return 0;
}

int read_usb(int fd, char *tmp_buf, int size) {

  int nbytes = 0;
  char c;
  // nbytes = read(fd, obuf,sizeof obuf);
  while (read(fd, &c, sizeof(char)) > 0 && nbytes < size) {

    // printf("%c",c);
    tmp_buf[nbytes] = c;
    nbytes++;
    usleep(500);
  }
  tmp_buf[nbytes + 1] = '\0';
  // printf( "obuf ''''%s''' %d\n\n",obuf,nbytes);
  return (nbytes);
}

int write_usb(int fd, char *cmd) {

  int nbytes = 0;
  /*
  for (int i=0; i< sizeof (cmd);i++){
   c = cmd [i];
   n  = write (fd, &c, sizeof(char));
   nbytes += n;
   //usleep(500);
  }
  */
  nbytes = write(fd, cmd, strlen(cmd));
  // fsync (fd);
  return (nbytes);
};

int /* O - 0 = MODEM ok, -1 = MODEM bad */
    execute_at_cmd(int fd, char *cmd, char *clean_buf,
                   int size) { /* I - Serial port file */
  // char ibuf[255];  /* Input buffer */
  int nbytes; /* Number of bytes read */
  int tries;  /* Number of tries so far */
  char acmd[128];
  char tmp_buf[2048]; /* eanbuffer */
  // printf("?? START command to be excuted %s\n", cmd);
  for (tries = 0; tries < 5; tries++) {
    // printf (" <<<<< no of retries %d\n",tries);
    nbytes = write_usb(fd, cmd);
    if (nbytes < strlen(cmd)) {
      printf("error: no of bytes written %d %d\n", nbytes, (int)strlen(cmd));
      continue;
    }
    // printf("no of bytes written %d %d\n", nbytes, (int)strlen(cmd));
    nbytes = read_usb(fd, tmp_buf, size);
    // printf( "obuf <?<?start string cmd = <%s> [ %s ] end string <cmd %s>>?>?>
    // %d\n\n",cmd,obuf,cmd,nbytes);
    strcpy(acmd, cmd);
    acmd[strlen(cmd) - 1] = '\0';
    int start_index = isStringExist(tmp_buf, acmd);
    if (start_index < 0)
      return -1;
    // printf ("*****index where found %d cmd %s %s\n",start_index,acmd,cmd);
    int end_index = isStringExist(&tmp_buf[start_index], "OK");
    if (end_index >= 0) {
      end_index = end_index + start_index;
      // printf ("*****OK found end index %d  OK\n",end_index);
      /* nul terminate the string and see if we got an OK response */
      // if (strncmp(obuf, "OK", 2) == 0)
      // printf ("??END command to be excuted cmd %s %s  rest
      // %s\n\n\n",acmd,cmd, &obuf [end_index]);
      if (start_index >= 0) {
        clean_output(tmp_buf, clean_buf, start_index, end_index + 2);
        // printf ("<< clean buffer stindex %d \n [ %s ] \n endindex
        // %d\n",start_index,clean_buf,end_index);
      }
      return (0);
    } else {
      sprintf(clean_buf, "ERROR: info not available\n");
      //printf("<< ERROR finding info from at \n");
    }
    return -1;
  }
  return (-1);
}

bool
setup_port(int fd,int baud, int data_bits, int stop_bits, bool parity, bool hardware_control)
{
	// Check file descriptor
	if(!isatty(fd))
	{
		fprintf(stderr, "\nERROR: file descriptor %d is NOT a serial port\n", fd);
		return false;
	}

	// Read file descritor configuration
	struct termios  config;
	if(tcgetattr(fd, &config) < 0)
	{
		fprintf(stderr, "\nERROR: could not read configuration of fd %d\n", fd);
		return false;
	}

	// Input flags - Turn off input processing
	// convert break to null byte, no CR to NL translation,
	// no NL to CR translation, don't mark parity errors or breaks
	// no input parity check, don't strip high bit off,
	// no XON/XOFF software flow control
	config.c_iflag &= ~(IGNBRK | BRKINT | ICRNL |
						INLCR | PARMRK | INPCK | ISTRIP | IXON);

	// Output flags - Turn off output processing
	// no CR to NL translation, no NL to CR-NL translation,
	// no NL to CR translation, no column 0 CR suppression,
	// no Ctrl-D suppression, no fill characters, no case mapping,
	// no local output processing
	config.c_oflag &= ~(OCRNL | ONLCR | ONLRET |
						 ONOCR | OFILL | OPOST);

	#ifdef OLCUC
		config.c_oflag &= ~OLCUC;
	#endif

	#ifdef ONOEOT
		config.c_oflag &= ~ONOEOT;
	#endif

	// No line processing:
	// echo off, echo newline off, canonical mode off,
	// extended input processing off, signal chars off
	config.c_lflag &= ~(ECHO | ECHONL | ICANON | IEXTEN | ISIG);

	// Turn off character processing
	// clear current char size mask, no parity checking,
	// no output processing, force 8 bit input
	config.c_cflag &= ~(CSIZE | PARENB);
	config.c_cflag |= (CS8);

	// One input byte is enough to return from read()
	// Inter-character timer off
	config.c_cc[VMIN]  = 1;
	config.c_cc[VTIME] = 10; // was 0

	// Get the current options for the port
	////struct termios options;
	////tcgetattr(fd, &options);

	// Apply baudrate
	switch (baud)
	{
		case 1200:
			if (cfsetispeed(&config, B1200) < 0 || cfsetospeed(&config, B1200) < 0)
			{
				fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
				return false;
			}
			break;
		case 1800:
			cfsetispeed(&config, B1800);
			cfsetospeed(&config, B1800);
			break;
		case 9600:
			cfsetispeed(&config, B9600);
			cfsetospeed(&config, B9600);
			break;
		case 19200:
			cfsetispeed(&config, B19200);
			cfsetospeed(&config, B19200);
			break;
		case 38400:
			if (cfsetispeed(&config, B38400) < 0 || cfsetospeed(&config, B38400) < 0)
			{
				fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
				return false;
			}
			break;
		case 57600:
			if (cfsetispeed(&config, B57600) < 0 || cfsetospeed(&config, B57600) < 0)
			{
				fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
				return false;
			}
			break;
		case 115200:
			if (cfsetispeed(&config, B115200) < 0 || cfsetospeed(&config, B115200) < 0)
			{
				fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
				return false;
			}
			break;

		// These two non-standard (by the 70'ties ) rates are fully supported on
		// current Debian and Mac OS versions (tested since 2010).
		case 460800:
			if (cfsetispeed(&config, B460800) < 0 || cfsetospeed(&config, B460800) < 0)
			{
				fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
				return false;
			}
			break;
		case 921600:
			if (cfsetispeed(&config, B921600) < 0 || cfsetospeed(&config, B921600) < 0)
			{
				fprintf(stderr, "\nERROR: Could not set desired baud rate of %d Baud\n", baud);
				return false;
			}
			break;
		default:
			fprintf(stderr, "ERROR: Desired baud rate %d could not be set, aborting.\n", baud);
			return false;

			break;
	}

	// Finally, apply the configuration
	//if(tcsetattr(fd, TCSAFLUSH, &config) < 0)
	if(tcsetattr(fd, TCSANOW, &config) < 0)
	{
		fprintf(stderr, "\nERROR: could not set configuration of fd %d\n", fd);
		return false;
	}

	// Done!
	return true;
}



int init_modem(int fd) { 
       setup_port(fd,115200, 8, 1, false, false);
       return 0;
}

int read_gps_port(FILE *fp, char *obuf, int size) {

  int nbytes = 0;
  char c;
  // read command
   //printf ("read device gps ++++ size %d\n",size);
   /*
  if (fgets(obuf, size, fp) != NULL){
      // writing content to stdout 
      fflush (fp);
      puts(obuf);
   }
   */
  while (((c = fgetc(fp)) != EOF) && nbytes < size-1) {
	  fflush (fp);
	  usleep (1);
	  if (c != '\r' && c != '\n'){
    		//printf ("%c",c);
    		obuf[nbytes] = c;
    		nbytes++;
	  }
	  else
	  {
	     break;
	  }
  }
  obuf[nbytes + 1] = '\0';
  return (0);
}

int get_gps_devices (){
   int status = 0;
    DIR* dir = opendir("/dev/serial/");
    if (dir){
    //Directory exists. 
    	closedir(dir);
    } else if (ENOENT == errno) {
       fprintf (stderr,"error:  device is not plugged in, plug device and try again. dir /dev/serial not exist\n");
       return -1;
    }
  int fd = open("/dev/ttyUSB2", O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1) {
       fprintf (stderr,"error:  device is not plugged in, plug device and try again. dev /dev/ttyUSB2 not exist \n");
       return -1;
    }
  close (fd);
  status = system ("find_devices.sh -port > /tmp/gps_devices.txt");
  if (status < 0)
  {
	    fprintf  (stderr,"usbtools:get_devices:error reading gps devices status %d\n",status);
	    fflush (stderr);
	    return -1;
  };
  if (read_file ("/tmp/gps_devices.txt",gps_dev,MODEM_NUM) < 0)
    {
	    fprintf  (stderr,"usbtools:get_devices:error reading gps devices ");
	    fflush (stderr);
            system ("rm /tmp/gps_devices.txt");
	    return -1;
    } 
  //system ("rm /tmp/gps_devices.txt");
  return 0;
}



FILE *open_gps(int modem_num) {
  FILE *fp;
  if (get_gps_devices () >=0 ){
  	fp = fopen(gps_dev[modem_num - 1], "r");
	printf ("DEBUG GPS devise %s \n",gps_dev[modem_num-1]);
  	if (fp == NULL) {
    	printf("iusbtools:read_gps:sq_ReadGPS %s", gps_dev[modem_num - 1]);
    	perror("Error while opening the file");
    	return NULL;
	}
      return (fp);
  }
  return NULL;
}

void close_gps (FILE *fp){
	fclose (fp);
}

int read_gps(int modem_num, char *buf, int size) {
  FILE *fp;
  if (get_gps_devices () >=0 ){
  	fp = fopen(gps_dev[modem_num - 1], "r");
	printf ("DEBUG GPS devise %s \n",gps_dev[modem_num-1]);
  	if (fp == NULL) {
    	printf("iusbtools:read_gps:sq_ReadGPS %s", gps_dev[modem_num - 1]);
    	perror("Error while opening the file");
    	return -1;
	}
      read_gps_port(fp, buf, size);
     fclose(fp);
     return 0;
  }
  return -1;
}


int exec_cmd_ext(char *dev, char *cmd, char *buf, int size) {
  int fd;
  fd = open(dev, O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1) {
    perror(dev);
    exit(-1);
  } else {
    // setvbuf(fd, NULL, _IONBF, 0);
    init_modem(fd);
    // flush_usb (fd);
    execute_at_cmd(fd, cmd, buf, size); /* I - Serial port file */
    close(fd);
    return 0;
  }
}

int read_file(char *file_name,char *devices[],int size)
{
 FILE *fp;
   char str[2024];
   if (ctl_devices_read)
      return 0;
   /* opening file for reading */
   fp = fopen(file_name , "r");
   if(fp == NULL) {
      fprintf (stderr,"usbtools:read_file: error reading [%s]\n",file_name);
      fflush (stderr);
      return(-1);
   }
   int i = 0;
   while( fgets (str, 2024, fp)!=NULL ) {
      // writing content to stdout 
       int len = strlen (str);
       str [len -1] = '\0';
       if (i < size)
         devices [i] = strdup (str);
       else
          fprintf (stderr,"usbtools:read_file: input %d is greater than array %d\n",i,size);
        fprintf (stderr, "usbtools:read_file:device name %s \n",devices[i]);
	i++;
   }
   fclose(fp);
   if (strstr (devices[0],"/dev/ttyUSB")== NULL)
	   return -1;
    ctl_devices_read = 1;   
   return(0);
}


int get_ctl_devices (){
   int status = 0;
    DIR* dir = opendir("/dev/serial/");
    if (dir){
    //Directory exists. 
    	closedir(dir);
    } else if (ENOENT == errno) {
       fprintf (stderr,"error:  device is not plugged in, plug device and try again. dir /dev/serial not exist\n");
       fflush (stderr);
       return -1;
    }
  int fd = open("/dev/ttyUSB2", O_RDWR | O_NOCTTY | O_NDELAY);
  if (fd == -1) {
       fprintf (stderr,"error: device is not plugged in, plug device and try again. dev /dev/ttyUSB2 no exist \n");
       fflush (stderr);
       return -1;
    }
  close (fd);

  status = system ("find_devices.sh > /tmp/ctl_devices.txt");
  if (status < 0)
  {
	    fprintf  (stderr,"usbtools:get_devices:error reading ctl devices status %d\n",status);
	    fflush (stderr);
	    return -1;
  };
  if (read_file ("/tmp/ctl_devices.txt",ctl_dev,MODEM_NUM) < 0)
    {
	    fprintf  (stderr,"usbtools:get_devices:error reading ctl devices ");
	    fflush (stderr);
            system ("rm /tmp/ctl_devices.txt");
	    return -1;
    } 
  system ("rm /tmp/ctl_devices.txt");
  return 0;
}

int exec_cmd(int modem_num, char *cmd, char *buf, int size) {

  if (get_ctl_devices () >= 0)
  {
  	exec_cmd_ext(ctl_dev[modem_num - 1], "at\r", buf, size);
  	// printf("after  cmd1\n");
  	exec_cmd_ext(ctl_dev[modem_num - 1], "at\r", buf, size);
  	// printf("after  cmdi2\n");
  	sleep(1);
  	exec_cmd_ext(ctl_dev[modem_num - 1], cmd, buf, size);
  	// printf("after  cmd3\n");
  	sleep(1);
  	// exec_cmd_ext (ctl_dev[modem_num-1],cmd,buf,size);
  	// printf ("after  cmdi4\n");
  	// sleep (1);
	return 0;
  }
  return -1;
}


char* get_ipaddr(char *interface)
{
 int fd;
 struct ifreq ifr;

 fd = socket(AF_INET, SOCK_DGRAM, 0);

 /* I want to get an IPv4 IP address */
 ifr.ifr_addr.sa_family = AF_INET;

 /* I want IP address attached to "eth0" */
 strncpy(ifr.ifr_name, interface, IFNAMSIZ-1);

 ioctl(fd, SIOCGIFADDR, &ifr);

 close(fd);

 /* display result */
 //printf("%s\n", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
 return (strdup(inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr)));

}

void get_interfaces (char *interface[]){
	struct ifaddrs *addrs,*tmp;

	getifaddrs(&addrs);
	tmp = addrs;
        int i = 0;
	while (tmp)
	{
    		if (tmp->ifa_addr && tmp->ifa_addr->sa_family == AF_PACKET){
                 if (strlen (tmp->ifa_name) > 8){
			interface [i] = strdup (tmp->ifa_name); 
			//printf ("ifaddress %s addr %s \n",tmp->ifa_name,get_ipaddr(interface[i]));
			i ++;
		 }
		}
            tmp = tmp->ifa_next;
	}

	freeifaddrs(addrs);
        int j = 0; 
	char *stmp = NULL;
	int n = MODEM_NUM;
        for (i = 0; i < n-1; i++){       
       		// Last i elements are already in place    
       		for (j = 0; j < n-i-1; j++){  
			if (interface[j] != NULL && interface[j+1] != NULL){
           			if (strcmp (interface[j],interface[j+1]) > 0){ 
              				stmp = interface [j];
	      				interface[j] = interface[j+1];
	      				interface [j+1] = stmp;
	   			}
      		 	}
		}
	}

	//for (i=0;i<n;i++)
        	//printf("%s\n", intfs[i]);

}
