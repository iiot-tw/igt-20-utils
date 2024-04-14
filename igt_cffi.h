/*
    This file is modified from the original igt.h file for the python cffi module
*/

// IGT_AI_Config.analog_type
//
#define  IGT_AI_TYPE_VOLTAGE     0X1000
#define  IGT_AI_TYPE_CURRENT     0X2000

// IGT_AI_Config.input_style
//
#define  IGT_AI_VOLTAGE_STYLE_DEFAULT      0x1001     //0~10V
#define  IGT_AI_VOLTAGE_STYLE_FULLSCALE    0x1002     //-10~10V
#define  IGT_AI_VOLTAGE_STYLE_HALFSCALE    0x1003     //-5~5V

#define  IGT_AI_CURRENT_STYLE_DEFAULT      0x2001     //4~20mA
#define  IGT_AI_CURRENT_STYLE_FULLSCALE    0x2002     //0~20mA


int32_t IGT_AI_Config(
    uint32_t analog_type,
    uint32_t input_style,
    uint32_t response_timeout // in us
);

int32_t IGT_AI_Enable(void);

int32_t IGT_AI_ReadlineF64(
    uint16_t ch,
    double* f64
);

int32_t IGT_AI_ReadlineU16(
    uint16_t ch,
    uint16_t* u16
);

int32_t IGT_AI_ReadlineI16(
    uint16_t ch,
    int16_t* i16
);

int32_t IGT_AI_Disable(void);
