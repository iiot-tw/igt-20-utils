/* This script is created as an EXAMPLE for specific purpose on Neousys IGT series.

   It works only when eth0 is configured in /etc/network/interfaces
   Note that some network manager, such as connman, will ignore the configuration
   in /etc/network/interface and make this script not work as expected.

   THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
   NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
   NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE AUTHORS SHALL NOT, UNDER ANY
   CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
   DAMAGES, FOR ANY REASON WHATSOEVER.

built with: gcc igt-33_read_volt.c -o igt-33_read_volt -l igt
*/
#include <stdint.h>
#include <stdio.h>
#include <igt.h>
#include <unistd.h>

#define AI_CH   0

int main(void) {
    double fAI0;
    int32_t ret;
    int32_t icount=0;

    ret = IGT_AI_Config(
        IGT_AI_TYPE_VOLTAGE,
        IGT_AI_VOLTAGE_STYLE_DEFAULT,
        10000 /*response timeout*/);
    printf("Config: %d\n", ret);
    ret = IGT_AI_Enable();
    if (ret !=0 ){
        printf("failed to enable:%d\n", ret);
        return ret;
    }
    while (1) {
        ret=IGT_AI_ReadlineF64(AI_CH, &fAI0);
        printf("Read#%d: %d (%05.5f) (#%04d)\n", AI_CH, ret , fAI0, ++icount);
        usleep(500000);
    }
    ret = IGT_AI_Disable();
    printf("Disable: %d\n", ret);
    return 0;
}
