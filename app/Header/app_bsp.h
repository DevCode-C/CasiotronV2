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

#define CS_EEPROM       GPIO_PIN_10
#define EEPROM_PORT     GPIOB

//PIN ALERT
#define GPIO_PIN_ALERT  GPIO_PIN_3
#define GPIO_PORT_ALERT GPIOC

#define NVIC_PRIORITY_HIGHEST               0UL
#define NVIC_PRIORITY_HIGH                  1UL
#define NVIC_PRIORITY_LOW                   2UL
#define NVIC_PRIORITY_LOWEST                3UL

#define NVIC_SUBPRIORITY_HIGHEST            0UL
#define NVIC_SUBPRIORITY_HIGH               1UL
#define NVIC_SUBPRIORITY_LOW                2UL
#define NVIC_SUBPRIORITY_LOWEST             3UL

#define WRITE_OK                            1UL
#define WRITE_ERROR                         0UL
#define READ_OK                             1UL
#define READ_ERROR                          0UL

#define ELEMENTS_IN_BUFFER                  0UL

#define USER_SET                            1UL
#define USER_RESET                          0UL

#define NEXT_BYTE                           8UL

#define CLEAR_BUFFER(buffer)     memset((buffer),0UL,sizeof((buffer)))

#define COMPLEMENT_2s_To_UINT8(value) (0xFFU & (256U - (value)))   
#define UINT8_To_COMPLEMENT_2s(value) (0xFFU & ((~(value)) + 1U))  


typedef struct _serial_MsgTypedef
{
    uint8_t msg;        //Tipo de mensaje
    uint8_t param1;     //Hora o dia
    uint8_t param2;     //minutos o mes
    uint16_t param3;    // segundos o years

} Serial_MsgTypeDef;

typedef struct _memory_MsgTypedef
{
    Serial_MsgTypeDef data[2];
    uint16_t temperature_data;
} Memory_MsgTypeDef;

/*Privates extern variables---------------------------------------------*/
extern QUEUE_HandleTypeDef  QueueSerialTx;
extern QUEUE_HandleTypeDef QueueSerialBlink;

extern QUEUE_HandleTypeDef QueueSerialMemoryRx;
extern QUEUE_HandleTypeDef QueueMemoryData;

extern MEMORY_HandleTypeDef memoryTaskHandle;

extern uint32_t memory_Counter;
extern uint8_t memory_alarm_FLag;
extern uint8_t waiting_Data;

/*----------------------------------------------------------------------*/
void USART2_IRQHandler(void);
void RTC_IRQHandler(void);

/**
 * @brief  Conversion of decimal values to character ASCCI and store up in buffer
 * 
 * @param uint8_t *buffer,  Data storage 
 * 
 * @param int32_t val, Decimal value 
 * 
 * @return NONE (VOID)
*/
void DecToStr(uint8_t *buffer, int32_t val);

/**
 * @brief  Verify the numbers of digit characters of a decimal value
 *  
 * @param int32_t num, Decimal value
 * 
 * @return uint8_t, Number of digit characters 
*/
uint8_t number_digits(int32_t num);

#endif

