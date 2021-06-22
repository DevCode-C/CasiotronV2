#include "stm32f0xx.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "app_bsp.h"
#include "stm32f070xb.h"
#include "stm32f0xx_hal_conf.h"
#include "string.h"
#include "stm32f0xx_hal.h"

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/

const uint8_t* msgOK    = {(const uint8_t*)"OK\r\n"};
const uint8_t* msgError = {(const uint8_t*)"ERROR\r\n"};

const uint8_t* comando[3] = {(const uint8_t*)"AT+TIME",
                             (const uint8_t*)"AT+DATE",
                             (const uint8_t*)"AT+ALARM"};

UART_HandleTypeDef  UartHandle              = {0};
RTC_HandleTypeDef   RTC_InitStructure       = {0};
RTC_TimeTypeDef 	RTC_TImeConfig          = {0};
RTC_DateTypeDef 	RTC_DateConfig          = {0};
RTC_AlarmTypeDef 	RTC_AlarmConfig         = {0};

__IO ITStatus uartState     = RESET;
__IO ITStatus status        = RESET;
__IO ITStatus AlarmRTC      = RESET;

uint8_t RxByte;
uint8_t RxBuffer[30];
uint8_t BufferTemp[30];
uint32_t tick= 0;

void UART_Init(void);
void RTC_Init(void);
void showClock(void);
void showAlarm(void);
extern void initialise_monitor_handles(void);

int main( void )
{
    initialise_monitor_handles();
    printf("\n");

    HAL_Init( );
    UART_Init();
    RTC_Init();
    tick = HAL_GetTick();

    strcpy((char*)BufferTemp,(const char*)RxBuffer);
    for (; ;)
    {
        if (status == SET)
        {
            status = RESET;
            HAL_UART_Transmit_IT(&UartHandle,(uint8_t*)RxBuffer,strlen((const char*)RxBuffer));
        }

        if (AlarmRTC == SET )
        {
            showAlarm();
        }
        else
        {
            showClock();
        }  
    } 
    return 0u;
}


void UART_Init()
{
    UartHandle.Instance             = USART2;
    UartHandle.Init.BaudRate        = 115200;
    UartHandle.Init.WordLength      = UART_WORDLENGTH_8B;
    UartHandle.Init.StopBits        = UART_STOPBITS_1;
    UartHandle.Init.Parity          = UART_PARITY_NONE;
    UartHandle.Init.HwFlowCtl       = UART_HWCONTROL_NONE;
    UartHandle.Init.Mode            = UART_MODE_TX_RX;
    UartHandle.Init.OverSampling    = UART_OVERSAMPLING_16;

    HAL_UART_Init(&UartHandle);
    HAL_UART_Receive_IT(&UartHandle,&RxByte,1);
}

void RTC_Init(void)
{
    __HAL_RCC_RTC_ENABLE();
    RTC_InitStructure.Instance              = RTC;
    RTC_InitStructure.Init.HourFormat       = RTC_HOURFORMAT_24;
    RTC_InitStructure.Init.AsynchPrediv     = 127;
    RTC_InitStructure.Init.SynchPrediv      = 255;
    RTC_InitStructure.Init.OutPut           = RTC_OUTPUT_DISABLE;
    RTC_InitStructure.Init.OutPutPolarity   = RTC_OUTPUT_POLARITY_HIGH;
    RTC_InitStructure.Init.OutPutType       = RTC_OUTPUT_TYPE_OPENDRAIN;
    HAL_RTC_Init(&RTC_InitStructure);

    RTC_TImeConfig.Hours = 05;
    RTC_TImeConfig.Minutes = 15;
    RTC_TImeConfig.Seconds = 30;
    HAL_RTC_SetTime(&RTC_InitStructure,&RTC_TImeConfig,RTC_FORMAT_BIN);

    RTC_DateConfig.Date = 22;
    RTC_DateConfig.Month = RTC_MONTH_JUNE;
    RTC_DateConfig.Year = 21;
    HAL_RTC_SetDate(&RTC_InitStructure,&RTC_DateConfig,RTC_FORMAT_BIN);

    RTC_AlarmConfig.Alarm = RTC_ALARM_A;
    RTC_AlarmConfig.AlarmDateWeekDay = 22;
    RTC_AlarmConfig.AlarmTime.Hours = 05;
    RTC_AlarmConfig.AlarmTime.Minutes = 15;
    RTC_AlarmConfig.AlarmTime.Seconds = 40;
    RTC_AlarmConfig.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;
    HAL_RTC_SetAlarm_IT(&RTC_InitStructure,&RTC_AlarmConfig,RTC_FORMAT_BIN);
}

void showClock(void)
{
    if (HAL_GetTick() - tick > 1000)
        {
            tick = HAL_GetTick();
            HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_5);
            HAL_RTC_GetTime(&RTC_InitStructure,&RTC_TImeConfig,RTC_FORMAT_BIN);
            HAL_RTC_GetDate(&RTC_InitStructure,&RTC_DateConfig,RTC_FORMAT_BIN);
            printf("%02d:%02d:%02d - ",RTC_TImeConfig.Hours,RTC_TImeConfig.Minutes,RTC_TImeConfig.Seconds);
            printf("%02d/%02d/%04d\n",RTC_DateConfig.Date,RTC_DateConfig.Month,RTC_DateConfig.Year+2000);
        }

        if (!HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13))
        {
            HAL_RTC_GetAlarm(&RTC_InitStructure,&RTC_AlarmConfig,RTC_ALARM_A,RTC_FORMAT_BIN);
            printf("Alarm - %02d:%02d:%02d\n",RTC_AlarmConfig.AlarmTime.Hours,RTC_AlarmConfig.AlarmTime.Minutes,\
                    RTC_AlarmConfig.AlarmTime.Seconds);
            while (!HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13));
            
        }
}

void showAlarm(void)
{
    AlarmRTC = RESET;
    uint8_t time = 1;
    uint32_t timeTick = HAL_GetTick();
    for ( ; ;)
    {
        if (HAL_GetTick() - timeTick > 1000)
        {
            timeTick = HAL_GetTick();
            time++;
            printf("ALARM!!\n");
        }
        if (!HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13) || time > 59)
        {
            break;
        }   
    }
}

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    uartState = SET;
    memset(RxBuffer,0,sizeof(RxBuffer));
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    static uint32_t i = 0;
    RxBuffer[i] = RxByte;
    i++;
    if(RxBuffer[i-1] == '\r')
    {
        status = SET;
        i=0;
    }
    HAL_UART_Receive_IT(&UartHandle,&RxByte,1);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    AlarmRTC = SET;
}