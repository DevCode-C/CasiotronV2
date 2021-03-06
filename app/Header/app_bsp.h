#ifndef BSP_H_
#define BSP_H_
#include "stm32f0xx.h"
#include "stm32f0xx_hal_conf.h"
#include <stdint.h>
#include "stdio.h"
#include <stdlib.h>
#include "stm32f070xb.h"
#include "string.h"
#include "ctype.h"
#include "stm32f0xx_hal.h"
#include "stm32f0xx_hal_i2c.h"
#include "lcd.h"
#include "temp.h"
#include "stm32f0xx_hal_tim.h"
#include "queue.h"
#include "eeprom.h"


#define NONE    0U
#define TIME    1U
#define DATE    2U
#define ALARM   3U
#define BLINK   4U
#define TEMP    5U

#define MEMORY_DUMP         6U
#define MEMORY_TIME_LOG     7U

#define LCD_PORT        GPIOC   //LCD PORT
#define LCD_PINES       GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2  //LCD Pines
#define LCD_CS          GPIO_PIN_0  //PIN activado en bajo para la LCD
#define LCD_RS          GPIO_PIN_1  // Selector data/command
#define LCD_RST         GPIO_PIN_2  //Reset del LCD, activado en bajo

#define GPIO_LED_PIN_BOARD  GPIO_PIN_5  //Led de la tarjeta
#define GPIO_LED_PORT_BOARD GPIOA       //Puerto del led de la tarjeta

#define GPIO_BUTTON_PIN     GPIO_PIN_13 
#define GPIO_BUTTON_PORT    GPIOC

#define UART_PINES      GPIO_PIN_2 | GPIO_PIN_3 //UART Pines
#define UART_PORT       GPIOA

#define SPI_PINES       GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5;  //SPI Pines
#define SPI_PORT        GPIOB

#define I2C_PINES       GPIO_PIN_6 | GPIO_PIN_7 //I2C Pines
#define I2C_PORT        GPIOB

#define EEPROM_CS       GPIO_PIN_10
#define EEPROM_PORT     GPIOB

//PIN ALERT
#define GPIO_PIN_ALERT  GPIO_PIN_3
#define GPIO_PORT_ALERT GPIOC

#define NVIC_PRIORITY_HIGHEST               0U
#define NVIC_PRIORITY_HIGH                  1U
#define NVIC_PRIORITY_LOW                   2U
#define NVIC_PRIORITY_LOWEST                3U

#define NVIC_SUBPRIORITY_HIGHEST            0U
#define NVIC_SUBPRIORITY_HIGH               1U
#define NVIC_SUBPRIORITY_LOW                2U
#define NVIC_SUBPRIORITY_LOWEST             3U

#define WRITE_OK                            1U
#define WRITE_ERROR                         0U
#define READ_OK                             1U
#define READ_ERROR                          0U

#define ELEMENTS_IN_BUFFER                  0U

#define USER_SET                            1U
#define USER_RESET                          0U

#define NEXT_BYTE                           8U

#define CLEAR_BUFFER(buffer)     memset((buffer),0,sizeof((buffer)))

#define COMPLEMENT_2s_To_UINT8(value) (0xFFU & (256U - (value)))   
#define UINT8_To_COMPLEMENT_2s(value) (0xFFU & ((~(value)) + 1U))  


typedef struct _serial_MsgTypedef
{
    uint8_t msg;        //Tipo de mensaje
    uint8_t param1;     //Hora o dia
    uint8_t param2;     //minutos o mes
    uint16_t param3;    // segundos o years

} Serial_MsgTypeDef;

extern QUEUE_HandleTypeDef  QueueSerialTx;
extern QUEUE_HandleTypeDef QueueSerialBlink;
extern EEPROM_HandleTypeDef eeprom_Handle;

void USART2_IRQHandler(void);
void RTC_IRQHandler(void);

#endif

