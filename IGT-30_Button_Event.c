#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <poll.h>

#define SYSFS_BTN_DIR "/sys/class/gpio"
#define MAX_BTN_NUM	2

#if defined IGT_30
const unsigned int g_iBtnGpioPin[MAX_BTN_NUM]={72,73}; /*IGT-30*/
#elif defined IGT_20
const unsigned int g_iBtnGpioPin[MAX_BTN_NUM]={68,67}; /*IGT-20*/
#else
#error "model not defined"
#endif

#define POLL_TIMEOUT (1 * 1000) /* 3 seconds */
#define MAX_BUF 64

int IGT_BTN_EdgeCreate(unsigned int line, char *edge)
{
        int fd;
        char buf[MAX_BUF];

        snprintf(buf, sizeof(buf), SYSFS_BTN_DIR "/gpio%d/edge",  g_iBtnGpioPin[line]);
 
        fd = open(buf, O_WRONLY);
        if (fd < 0) {
                perror("gpio/set-edge");
                return fd;
        }
 
        write(fd, edge, strlen(edge) + 1); 
        close(fd);
		
        snprintf(buf, sizeof(buf), SYSFS_BTN_DIR "/gpio%d/value",  g_iBtnGpioPin[line]);
 
        fd = open(buf, O_RDONLY | O_NONBLOCK );
        if ( fd < 0) {
                perror("gpio/fd_open");
				return fd;
        }
		
		return fd;
}

int IGT_BTN_EdgeClose(int fd)
{
        return close(fd);
}

/****************************************************************
 * Main
 ****************************************************************/
int main(void)
{
	struct pollfd fdset[MAX_BTN_NUM];
	int nfds = MAX_BTN_NUM;
	int btn_fd[MAX_BTN_NUM], event_num;

	int i;
	char buf[MAX_BUF];

	for (i=0; i<MAX_BTN_NUM; i++)
	{
		btn_fd[i] = IGT_BTN_EdgeCreate(i, "both" /*falling, rising*/);
		if (btn_fd[i]<0){
			printf("failed to create edge\n");
			exit(-2);
		}
	}

	printf("press CTRL-C to exit.\n");

	while (1) {
		memset((void*)fdset, 0, sizeof(fdset));

		fdset[0].fd = btn_fd[0];
		fdset[0].events = POLLPRI;

		fdset[1].fd = btn_fd[1];
		fdset[1].events = POLLPRI;

		event_num = poll(fdset, nfds, POLL_TIMEOUT);      

		if (event_num < 0) {
			printf("failed to poll\n");
			break;
		}
		else if ( 0 == event_num ) {
			//timeout and nothing happened
			printf(".");
		}
		else {
			for (i=0; i<MAX_BTN_NUM; i++){
				if (fdset[i].revents & POLLPRI) {
					lseek(fdset[i].fd, 0, SEEK_SET);
					read(fdset[i].fd, buf, MAX_BUF);
					if ( buf[0] == '0'){
						printf("Buttn %d pressed\n", i);
					}
					else{
						printf("Buttn %d released\n", i);
					}
				}
			}
		}
	
		fflush(stdout);
	}

	for(i=0; i<MAX_BTN_NUM; i++)
		IGT_BTN_EdgeClose(btn_fd[i]);

	return 0;
}
