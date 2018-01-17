/*
 * Neousys IGT-20 Modbus TCP Server Sample Code
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

/**********************************************
* Neousys IGT-20 DI Wrapper
***********************************************/
#include <fcntl.h>
#define SYSFS_DIO_DIR "/sys/class/gpio"
#define MAX_BUF 64
#define MAX_DI_NUM	4
const unsigned int g_iDiGpioPin[MAX_DI_NUM]={26,27,23,61};

int IGT_DI_ReadLine(unsigned int line, unsigned int *value)
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

int main(void)
{
    int s = -1;
    modbus_t *ctx;
    modbus_mapping_t *mb_mapping;

    ctx = modbus_new_tcp("127.0.0.1", 502);
    /*modbus_set_debug(ctx, TRUE);*/

    mb_mapping = modbus_mapping_new( \
					0,			/*# of coil*/ \
				    MAX_DI_NUM, /*# of contact*/ \
				    0, \
				    0);

    if (mb_mapping == NULL) {
        fprintf(stderr, "Failed to allocate the mapping: %s\n",
                modbus_strerror(errno));
        modbus_free(ctx);
        return -1;
    }

    int val;

    s = modbus_tcp_listen(ctx, 1);
    modbus_tcp_accept(ctx, &s);


    for (;;) {
        uint8_t query[MODBUS_TCP_MAX_ADU_LENGTH];
        int rc;

        rc = modbus_receive(ctx, query);
        if (rc > 0) {
			/* rc is the query size */
		   
			IGT_DI_ReadLine(0,&val);
			mb_mapping->tab_input_bits[0]=val;
			IGT_DI_ReadLine(1,&val);
			mb_mapping->tab_input_bits[1]=val;
			IGT_DI_ReadLine(2,&val);
			mb_mapping->tab_input_bits[2]=val;
			IGT_DI_ReadLine(3,&val);
			mb_mapping->tab_input_bits[3]=val;

            modbus_reply(ctx, query, rc, mb_mapping);
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
