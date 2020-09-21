#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>

#define SYSFS_BTN_DIR "/sys/class/gpio"
#define SYSFS_DIO_DIR "/sys/class/gpio"

#define MAX_BTN_NUM	2
#define MAX_LED_NUM	6

//#define	MODEL	IGT-20
//#define	MODEL	IGT-22

/* todo: add IGT-33*/
//const unsigned int g_iDiGpioPin[MAX_DI_NUM]={26,27,23,61};
//const unsigned int g_iDoGpioPin[MAX_DO_NUM]={44,45,46,47};
//const unsigned int g_iBtnGpioPin[MAX_BTN_NUM]={72,73};
//const unsigned int g_iDoEnable=65;

#if MODEL == IGT-20
  #define SYSFS_LED_DIR "/sys/class/leds/igt20::"
  #define MAX_DI_NUM	4
  #define MAX_DO_NUM	4
  #define DO_EN_VAL		0
//IGT-20/IGT-21
const unsigned int g_iDiGpioPin[MAX_DI_NUM]={26,27,23,61};
const unsigned int g_iDoGpioPin[MAX_DO_NUM]={44,45,46,47};
const unsigned int g_iBtnGpioPin[MAX_BTN_NUM]={68,67};
const unsigned int g_iDoEnGpioPin=65;
#elif MODEL == IGT-22
//IGT-22
  #define SYSFS_LED_DIR "/sys/class/leds/igt22::"
  #define MAX_DI_NUM	8
  #define MAX_DO_NUM	8
  #define DO_EN_VAL		1
const unsigned int g_iDiGpioPin[MAX_DI_NUM]={116,20,115,114,51,52,49,50};
const unsigned int g_iDoGpioPin[MAX_DO_NUM]={111,110,112,113,117,104,7,103};
const unsigned int g_iBtnGpioPin[MAX_BTN_NUM]={4,5};
const unsigned int g_iDoEnGpioPin=48;
const unsigned int g_iDoEn=1;
#else
  #error Unsupported model or model not defined
#endif

//#define MAX_AI_NUM	16
#define MAX_BUF         64


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

	snprintf(buf, sizeof(buf), SYSFS_DIO_DIR "/gpio%d/value",  g_iDoEnGpioPin);

	fd = open(buf, O_WRONLY);
	if (fd < 0)
		return fd;

#if MODEL == IGT-20
	if (enable == 1)
		write(fd, "0", 2);
	else
		write(fd, "1", 2);
#else
	if (enable == 1)
		write(fd, "1", 2);
	else
		write(fd, "0", 2);
#endif	
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

//        snprintf(buf, sizeof(buf), SYSFS_BTN_DIR "/gpio%d/value",  g_iBtnGpioPin[line]);
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

	snprintf(buf, sizeof(buf), SYSFS_LED_DIR "usr%d/brightness", line);

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
	int idx_di,idx_do, idx_led;
	int value;
	int btn0, btn1;
	
	btn_read(0, &btn0);
	btn_read(1, &btn1);
	
	printf("BTN0:%d\nBTN1:%d\n", btn0, btn1);

	for(idx_led=0;idx_led<6;idx_led++)
		led(idx_led,1);

	sleep(3);

	for(idx_led=0;idx_led<6;idx_led++)
		led(idx_led,0);

//enable DO output
	do_enable(1);

	for(idx_do=0;idx_do<MAX_DO_NUM;idx_do++){
		do_write(idx_do,1);
//just make sure my relay have enough time to contact
		sleep(1);
		printf("DI: ");
		for(idx_di=0;idx_di<MAX_DI_NUM;idx_di++){
			di_read(idx_di, &value);
			printf("%d", value);
		}
		printf("\n");
	}

//disable DO output
	do_enable(0);
	sleep(1);
	do_enable(1);

	sleep(1);
	for(idx_do=0;idx_do<MAX_DO_NUM;idx_do++)
		do_write(idx_do,0);

	do_enable(0);
}
