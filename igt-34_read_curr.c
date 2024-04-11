/* This script is created as an EXAMPLE for specific purpose on Neousys IGT series.

   THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
   NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
   NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
   A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. THE AUTHORS SHALL NOT, UNDER ANY
   CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
   DAMAGES, FOR ANY REASON WHATSOEVER.

built with: gcc igt-34_read_curr.c -ligt -o igt-34_read_curr
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
        IGT_AI_TYPE_CURRENT,
        IGT_AI_CURRENT_STYLE_DEFAULT,
        10000 /*response timeout*/);
    printf("Config: %d\n", ret);
    ret = IGT_AI_Enable();
    if (ret !=0 ){
        printf("failed to enable:%d\n", ret);
        return ret;
    }
    while (1) {
        ret=IGT_AI_ReadlineF64(AI_CH, &fAI0);
        printf("Read#%d: %05.5fmA (ret=%d) (#%04d)\n", AI_CH, fAI0, ret, ++icount);
        usleep(500000);
    }
    ret = IGT_AI_Disable();
    printf("Disable: %d\n", ret);
    return 0;
}
