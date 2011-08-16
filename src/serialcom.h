#ifndef __SERIALCOM_H__
#define __SERIALCOM_H__

#ifndef ERROR
#define ERROR 1
#endif

#ifndef OK
#define OK 0
#endif

#define BAUDRATE B38400

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <unistd.h>
#include <sys/signal.h>
#include <sys/types.h>

int serialcom_init(void);
int serialcom_init_old(int * fd);
int serialcom_read(int * fd, char * ch);
int serialcom_write(int * fd, char * str);
int serialcom_put(int * fd, char ch);

#endif /* __SERIALCOM_H__ */
