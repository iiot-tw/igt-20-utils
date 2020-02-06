#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define SYSFS_BTN_DIR "/sys/class/gpio"
#define SYSFS_LED_DIR "/sys/class/leds"
#define SYSFS_DIO_DIR "/sys/class/gpio"

#define MAX_BTN_NUM	2
#define MAX_LED_NUM	6
#define MAX_DI_NUM	4
#define MAX_DO_NUM	4
#define MAX_AI_NUM	16
#define MAX_BUF         64

const unsigned int g_iDiGpioPin[MAX_DI_NUM]={26,27,23,61};
const unsigned int g_iDoGpioPin[MAX_DO_NUM]={44,45,46,47};
const unsigned int g_iBtnGpioPin[MAX_BTN_NUM]={68,67};
const unsigned int g_iDoEnable=65;

int di_read(unsigned int line, unsigned int *value)
{
	int fd;
	char buf[MAX_BUF];
	char ch;

	snprintf(buf, sizeof(buf), SYSFS_DIO_DIR "/gpio%d/value",  g_iDiGpioPin[line]);

	fd = open(buf, O_RDONLY);
	if (fd < 0)
		return fd;

	read(fd, &ch, 1);

	if (ch != '0')
		*value = 1;
	else
		*value = 0;

	close(fd);
	return 0;
}

int do_enable(unsigned int enable)
{
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_DIO_DIR "/gpio%d/value",  g_iDoEnable);

	fd = open(buf, O_WRONLY);
	if (fd < 0)
		return fd;

	if (enable == 0)
		write(fd, "1", 2);
	else
		write(fd, "1", 2);
	
	close(fd);
	return 0;
}

int do_write(unsigned int line, unsigned int value)
{
	int fd;
	char buf[MAX_BUF];
	char ch;

	snprintf(buf, sizeof(buf), SYSFS_DIO_DIR "/gpio%d/value", g_iDoGpioPin[line]);
	fd = open(buf, O_WRONLY);
	if (fd < 0)
		return fd;

	if(value == 0)
		write(fd, "0", 2);
	else
		write(fd, "1", 2);

	close(fd);
	return 0;
}

int btn_read(unsigned int line, unsigned int *value)
{
        int fd;
        char buf[MAX_BUF];
        char ch;

        snprintf(buf, sizeof(buf), SYSFS_BTN_DIR "/gpio%d/value",  g_iBtnGpioPin[line]);
 
        fd = open(buf, O_RDONLY);
        if (fd < 0) {
			perror("btn_read");
			return fd;
        }
 
        read(fd, &ch, 1);

        if (ch == '0') {
                *value = 0;
        } else {
                *value = 1;
        }
 
        close(fd);
        return 0;
}

int led(unsigned int line, unsigned int value)
{
	int fd;
	char buf[MAX_BUF];

	snprintf(buf, sizeof(buf), SYSFS_LED_DIR "/igt20::usr%d/brightness", line);

	fd = open(buf, O_WRONLY);
	if (fd < 0)
		return fd;

	if (value == 0)
			write(fd, "0", 2);
	else
			write(fd, "1", 2);

	close(fd);
	return 0;
}

void main(void)
{
	int do_idx=3;
	int btn0, btn1;
	
	btn_read(0, &btn0);
	btn_read(1, &btn1);
	
	printf("BTN0:%d\nBTN1:%d\n", btn0, btn1);
	
	do_enable(1);
	do_write(do_idx,1);

//just make sure my relay have enough time to contact
	sleep(1);

	int idx,value;
	for(idx=0;idx<4;idx++){
		di_read(idx, &value);
		printf("DI%d: %d\n",idx, value);
	}
	
	do_write(do_idx,0);
	do_enable(0);
	
	for(idx=0;idx<6;idx++)
		led(idx, 1);

	sleep(3);
	for(idx=0;idx<6;idx++)
		led(idx, 0);
}
