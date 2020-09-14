#include <stdint.h>
#include <stdio.h>
#include <igt.h>
#include <unistd.h>

#define AI_CH   1

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
