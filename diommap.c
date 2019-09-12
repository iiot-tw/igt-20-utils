#include <stdlib.h>
#include <fcntl.h>
#include <stdio.h>
#include <time.h>
#include <sys/mman.h>
#include <unistd.h>

struct timespec diff(struct timespec start, struct timespec end);
#define GPIO0_START_ADDR 0x44E07000
#define GPIO0_END_ADDR   0x44E09000
#define GPIO1_START_ADDR 0x4804C000
#define GPIO1_END_ADDR   0x4804e000
#define GPIO2_START_ADDR 0x481AC000
#define GPIO2_END_ADDR   0x481AE000
#define GPIO3_START_ADDR 0x481AE000
#define GPIO3_END_ADDR   0x481B0000

#define GPIO_SIZE (GPIO1_END_ADDR - GPIO1_START_ADDR)
#define GPIO_DATAIN 0x138
#define GPIO_SETDATAOUT 0x194
#define GPIO_CLEARDATAOUT 0x190

#define DO0 (1<<10)
#define DO1 (1<<11)

#define DI0 (1<<15)
#define DI1 (1<<14)
#define DI2 (1<<13)
#define DI3 (1<<12)

volatile void *gpio_addr;
volatile unsigned int *gpio_datain_addr;
volatile unsigned int *gpio_setdataout_addr;
volatile unsigned int *gpio_cleardataout_addr;

unsigned long gpio_group_start;

int main(int argc, char *argv[])
{
    struct timespec time1, time2;
    int temp,fd;
    unsigned long value;

    int set=0,clear=0;
    if (argc >= 2)
    {
        if(argv[1][0] != '0')
            set=1;
        else
            clear=1;
        gpio_group_start = GPIO2_START_ADDR;
    }
    else
    {
        gpio_group_start = GPIO1_START_ADDR;
    }

    if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) exit;

//while(1)
{
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    gpio_addr = mmap(0, GPIO_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, gpio_group_start);
    if(gpio_addr == (void *) -1) exit;

    gpio_datain_addr =  gpio_addr + GPIO_DATAIN;
    gpio_setdataout_addr = gpio_addr + GPIO_SETDATAOUT;
    gpio_cleardataout_addr = gpio_addr + GPIO_CLEARDATAOUT;

    //read
    value = *((unsigned long *) gpio_datain_addr);
    printf("read value=%x \n",value);

    if (set)
    {
      //set
      *gpio_setdataout_addr = DO1 | DO0;

      value = *((unsigned long *) gpio_datain_addr);
      printf("read value=%x \n",value);
    }
    else if(clear)
    {
      //clear
      *gpio_cleardataout_addr = DO1 | DO0;

      value = *((unsigned long *) gpio_datain_addr);
      printf("read value=%x \n",value);
    }

    if(munmap((void*)gpio_addr, GPIO_SIZE) == -1) exit;

    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
}

    close(fd);



    return 0;
}

struct timespec diff(struct timespec start, struct timespec end)
{
    struct timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}
