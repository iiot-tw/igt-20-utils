#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>

#define FATAL {printf("error\n"); return -1;}

#define GPIO0_START_ADDR 0x44E07000
#define GPIO0_END_ADDR   0x44E09000
#define GPIO1_START_ADDR 0x4804C000
#define GPIO1_END_ADDR   0x4804e000
#define GPIO2_START_ADDR 0x481AC000
#define GPIO2_END_ADDR   0x481AE000
#define GPIO3_START_ADDR 0x481AE000
#define GPIO3_END_ADDR   0x481B0000

#define GPIO_OFFSET_DATAIN      0x138
#define MAP_SIZE (GPIO0_END_ADDR - GPIO0_START_ADDR)
#define MAP_MASK (MAP_SIZE - 1)

int readgpio(unsigned int gpio_index, unsigned long *readval)
{
     int fd;
     void *map_base, *virt_addr;
     off_t gpio_addr[4]={GPIO0_START_ADDR, GPIO1_START_ADDR,
                         GPIO2_START_ADDR, GPIO3_START_ADDR}, target;

     //unsigned long read_result;

     target=gpio_addr[gpio_index] + GPIO_OFFSET_DATAIN;

     if (readval == NULL) FATAL;

     if((fd = open("/dev/mem", O_RDWR | O_SYNC)) == -1) FATAL;

     map_base = mmap(0, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, target & ~MAP_MASK);
     if(map_base == (void *) -1) FATAL;

     virt_addr = map_base + (target & MAP_MASK);

     *readval = *((unsigned long *) virt_addr);

     if(munmap(map_base, MAP_SIZE) == -1) FATAL;
     close(fd);
     return 0;
}
// for IGT-33/-34
#define BTN0    8
#define BTN1    9
#define DI0     15
#define DI1     14
#define DO0     10
#define DO1     11
#define DO2     23
#define DO3     22
#define DO4     13
#define DO5     12

void main(void)
{
    unsigned long value;
    unsigned long igt33btn=0;
    unsigned long igt33di=0;
    unsigned long igt33do=0;

    readgpio(2, &value);
    igt33btn |= (value & (1<<BTN0)) >> BTN0 | (value & (1<<BTN1)) >> (BTN1-1);
    igt33do |=  (value & (1<<DO0))  >> DO0  | (value & (1<<DO1)) >> (DO1-1) |
               (value & (1<<DO2))  >> (DO2-2)  | (value & (1<<DO3)) >> (DO3-3);

    readgpio(1, &value);
    igt33do |=  (value & (1<<DO4))  >> (DO4-4) | (value & (1<<DO5)) >> (DO5-5);
    igt33di |=  (value & (1<<DI0))  >> DI0  | (value & (1<<DI1)) >> (DI1-1);

    printf("BTN=%08x\n", igt33btn);
    printf("DO=%08x\n", igt33do);
    printf("DI=%08x\n", igt33di);

}
