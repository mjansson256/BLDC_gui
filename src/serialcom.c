#include "serialcom.h"


#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE 1
#endif

volatile int STOP = FALSE;

void signal_handler_IO(int status);
int wait_flag = TRUE;

int serialcom_init(const char * device)
{
	int fd, c, res;
	struct termios oldtio,newtio;
	struct sigaction saio;
	char buf[255];
	//~ const char * device = "/dev/ttyUSB0";

	/* definition of signal handler */
	/* TRUE while no signal received */
	/* definition of signal action */

	/* open the device to be non−blocking (read will return immediatly) */
	fd = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if(fd < 0) {
		perror(device);
		exit(-1);
	}
	/* install the signal handler before making the device asynchronous */
	saio.sa_handler = signal_handler_IO;
	//~ saio.sa_mask = 0;
	saio.sa_flags = 0;
	saio.sa_restorer = NULL;
	sigaction(SIGIO,&saio,NULL);
	/* allow the process to receive SIGIO */
	fcntl(fd, F_SETOWN, getpid());
	/* Make the file descriptor asynchronous (the manual page says only
	O_APPEND and O_NONBLOCK, will work with F_SETFL...) */
	fcntl(fd, F_SETFL, FASYNC);
	tcgetattr(fd,&oldtio); /* save current port settings */
	/* set new port settings for canonical input processing */
	newtio.c_cflag = BAUDRATE | CRTSCTS | CS8 | CLOCAL | CREAD;
	newtio.c_iflag = IGNPAR | ICRNL;
	newtio.c_oflag = 0;
	newtio.c_lflag = ICANON;
	newtio.c_cc[VMIN]=1;
	newtio.c_cc[VTIME]=0;
	tcflush(fd, TCIFLUSH);
	tcsetattr(fd,TCSANOW,&newtio);
	/* loop while waiting for input. normally we would do something
	useful here */
	//~ while(STOP == FALSE) {
		//~ printf(".\n");usleep(100000);
		//~ /* after receiving SIGIO, wait_flag = FALSE, input is available
		//~ and can be read */
		//~ if (wait_flag == FALSE) {
			//~ res = read(fd, buf, 255);
			//~ buf[res] = 0;
			//~ printf(":%s:%d\n", buf, res);
		//~ //	if (res == 1) STOP = TRUE; /* stop loop if only a CR was input */
			//~ wait_flag = TRUE;
			//~ /* wait for new input */
		//~ }
	//~ }
	/* restore old port settings */
	tcsetattr(fd,TCSANOW,&oldtio);

	return fd;
}

void signal_handler_IO(int status)
{
	//~ printf("SIGIO\n");
	wait_flag = FALSE;
}

int serialcom_init_old(int * fd, const char * device)
{
	int rc;
	struct termios options;
	//~ const char * device = "/dev/ttyUSB0";

	rc = OK;

	*fd = open(device, O_RDWR | O_NOCTTY | O_NDELAY );
	if(*fd == -1) {
	  printf( "failed to open port\n" );
	  return ERROR;
	}

	//Set no delay when reading from serial buffer
	fcntl(*fd, F_SETFL, FNDELAY);

	//Get the current options for the port.
    tcgetattr(*fd, &options);

	//Set the baud rates to 38400...
	cfsetispeed(&options, B38400);
	cfsetospeed(&options, B38400);

	//Enable the receiver and set local mode...
    options.c_cflag |= (CLOCAL | CREAD);

	//Set the new options for the port...
	tcsetattr(*fd, TCSANOW, &options);

	//8N1
	options.c_cflag &= ~PARENB;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS8;

	//Disable hardware flow control
	//~ options.c_cflag &= ~CNEW_RTSCTS;

	return rc;
}

void serialcom_destroy(int * fd)
{
	close(*fd);
	*fd = NULL;	//No dangling pointers.
}


int serialcom_read(int * fd, char * ch)
{
	int n;
	n = read(*fd, ch, 1);
	if(n == 1) {
		return OK;
	} else {
		return ERROR;
	}
}

int serialcom_write(int * fd, char * str)
{
	int n;
	if(*fd) {
		n = write(*fd, str, strlen(str));
		if(n < strlen(str)) {
			printf("Oh, error sending %i bytes. Only sent %i bytes\n", strlen(str), n);
		}
	}
	return n;
}

int serialcom_put(int * fd, char ch)
{
	int n;
	n = write(*fd, &ch, 1);
	if(n != 1) {
		printf("Oh, error sending 1 byte. Only sent %i byte(s)\n", n);
	}
	return n;
}
