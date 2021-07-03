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

#define LCD_PORT        GPIOC   //LCD PORT
#define LCD_PINES       GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2  //LCD Pines
#define LCD_CS          GPIO_PIN_0  //PIN activado en bajo para la LCD
#define LCD_RS          GPIO_PIN_1  // Selector data/command
#define LCD_RST         GPIO_PIN_2  //Reset del LCD, activado en bajo

typedef struct _serial_MsgTypedef
{
    uint8_t msg;        //Tipo de mensaje
    uint8_t param1;     //Hora o dia
    uint8_t param2;     //minutos o mes
    uint16_t param3;    // segundos o years

} Serial_MsgTypeDef;

void USART2_IRQHandler(void);
void RTC_IRQHandler(void);
void SPI1_IRQHandler(void);

#endif

