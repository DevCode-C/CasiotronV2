#ifndef _BSP_H_
#define _BSP_H_
#include "stm32f0xx.h"
#include "stm32f0xx_hal_conf.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "stm32f070xb.h"
#include "string.h"
#include "ctype.h"
#include "stm32f0xx_hal.h"

#define NONE    0U
#define TIME    1U
#define DATE    2U
#define ALARM   3U

typedef struct _serial_MsgTypedef
{
    uint8_t msg;        //Tipo de mensaje
    uint8_t param1;     //Hora o dia
    uint8_t param2;     //minutos o mes
    uint16_t param3;    // segundos o years

} Serial_MsgTypeDef;


void USART2_IRQHandler(void);


#endif

