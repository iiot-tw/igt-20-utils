
/*
 * Neousys IGT-20 Modbus Server Sample Code
 * Copyright © 2018 Raymond Hsu <mingchao.hsu@gmail.com>
 *
 * Modified from https://github.com/stephane/libmodbus/blob/master/tests/random-test-server.c
 * Copyright © 2008-2014 Stéphane Raimbault <stephane.raimbault@gmail.com>
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <stdio.h>
#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <stdlib.h>
#include <errno.h>

#include <modbus/modbus.h>


//#define MODBUS_SERIAL_DEV           "/dev/ttyS2"
#define MODBUS_SERIAL_BAUDRATE      9600    /* 9600, 38400, 115200, ... */
#define MODBUS_SERIAL_PARITY        'N'     /* 'N', 'E', or 'O' */
#define MODBUS_SERIAL_DATABITS      8       /* 5, 6, 7, or 8 */
#define MODBUS_SERIAL_STOPBITS      1       /* 1 or 2 */
#define MODBUS_DEVICE_ID            1


/**********************************************
* Neousys IGT-20 DI Wrapper
***********************************************/
#include <fcntl.h>
#define SYSFS_DIO_DIR "/sys/class/gpio"
#define MAX_BUF 64
#define MAX_BTN_NUM     2

#define IGT33   33
#define IGT20   20

#if MODEL == IGT20
#undef MODEL
#define MODEL           "IGT-20"
#define MAX_DI_NUM      4
#define MAX_DO_NUM      4
const unsigned int g_iDiGpioPin[MAX_DI_NUM]={26,27,23,61};
const unsigned int g_iBtnGpioPin[MAX_BTN_NUM]={68,67};
const unsigned int g_iDoGpioPin[MAX_DO_NUM]={44,45,46,47};
#elif MODEL == IGT33
#undef MODEL
#define MODEL           "IGT-33"
#define MAX_DI_NUM      2
#define MAX_DO_NUM      6
#define MAX_AI_NUM      8
const unsigned int g_iDiGpioPin[MAX_DI_NUM]={47,46};
const unsigned int g_iBtnGpioPin[MAX_BTN_NUM]={72,73};
const unsigned int g_iDoGpioPin[MAX_DO_NUM]={74,75,87,86,45,44};
#else
  #error Unsupported model or model not defined
#endif

int IGT_DI_ReadLine(unsigned int line, uint8_t *value)
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
int IGT_BTN_ReadLine(unsigned int line, uint8_t *value)
{
        int fd;
        char buf[MAX_BUF];
        char ch;

        snprintf(buf, sizeof(buf), SYSFS_DIO_DIR "/gpio%d/value",  g_iBtnGpioPin[line]);

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
int IGT_DO_WriteLine(unsigned int line, uint8_t value)
{
        int fd, len;
        char buf[MAX_BUF];

        len = snprintf(buf, sizeof(buf), SYSFS_DIO_DIR "/gpio%d/value", g_iDoGpioPin[line]);

        fd = open(buf, O_WRONLY);
        if (fd < 0) {
                perror("gpio/set-value");
                return fd;
        }

        if (value)
                write(fd, "1", 2);
        else
                write(fd, "0", 2);

        close(fd);
        return 0;
}

int main(void)
{
    int s = -1;
    int verbose=1;
    modbus_t *ctx;
    modbus_mapping_t *mb_mapping;
    uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];

#if defined MODBUS_SERIAL_DEV
    ctx = modbus_new_rtu(MODBUS_SERIAL_DEV,
            MODBUS_SERIAL_BAUDRATE,
            MODBUS_SERIAL_PARITY,
            MODBUS_SERIAL_DATABITS,
            MODBUS_SERIAL_STOPBITS);
    printf("starting Modbus RTU server for %s at %s\n", MODEL, MODBUS_SERIAL_DEV);
#else
    ctx = modbus_new_tcp("127.0.0.1", 502);
    printf("starting Modbus TCP server for %s at port 502\n", MODEL);
#endif

    if (ctx == NULL) {
        fprintf(stderr, "Unable to create the libmodbus context\n");
        exit(-1);
    }

    /* set slave device ID */
    modbus_set_slave(ctx, MODBUS_DEVICE_ID);

    /*modbus_set_debug(ctx, TRUE);*/

    mb_mapping = modbus_mapping_new( \
                                    MAX_DO_NUM,                 /*# of coil*/ \
                                    MAX_DI_NUM+MAX_BTN_NUM, /*# of contact*/ \
                                    0, \
                                    0);

    if (mb_mapping == NULL) {
        fprintf(stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

#if defined MODBUS_SERIAL_DEV
    modbus_connect(ctx);
#else
    s = modbus_tcp_listen(ctx, 1);
    if (s == -1){
        fprintf(stderr, "Failed to create socket: %s\n",
                modbus_strerror(errno));

        modbus_free(ctx);
        return -2;
    }
    if(verbose) printf("wait for connection\n");
    modbus_tcp_accept(ctx, &s);
    if(verbose) printf("Connected\n");
#endif

    while(1) {
        int rc;

        rc = modbus_receive(ctx, query);

        if (rc > 0) {
/* rc is the query size */
          IGT_DI_ReadLine(0,&(mb_mapping->tab_input_bits[0]));
          IGT_DI_ReadLine(1,&(mb_mapping->tab_input_bits[1]));
          IGT_DI_ReadLine(2,&(mb_mapping->tab_input_bits[2]));
          IGT_DI_ReadLine(3,&(mb_mapping->tab_input_bits[3]));
          IGT_BTN_ReadLine(0,&(mb_mapping->tab_input_bits[4]));
          IGT_BTN_ReadLine(1,&(mb_mapping->tab_input_bits[5]));

          modbus_reply(ctx, query, rc, mb_mapping);

          //reading mb_mapping->tab_bits[x] here will get the output value from the master
          IGT_DO_WriteLine(0,mb_mapping->tab_bits[0]);
          IGT_DO_WriteLine(1,mb_mapping->tab_bits[1]);
          IGT_DO_WriteLine(2,mb_mapping->tab_bits[2]);
          IGT_DO_WriteLine(3,mb_mapping->tab_bits[3]);

        } else if (rc == -1) {
            /* Connection closed by the client or error */
            break;
        }
    }

    printf("Quit the loop: %s\n", modbus_strerror(errno));

    if (s != -1) {
        close(s);
    }
    modbus_mapping_free(mb_mapping);
    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
