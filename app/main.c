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

const char* msgOK    = {"OK\r\n"};
const char* msgError = {"ERROR\r\n"};

const char *comando_AT[] = {"AT+TIME" , "AT+DATE" , "AT+ALARM"};

UART_HandleTypeDef      UartHandle              = {0};
RTC_HandleTypeDef       RTC_InitStructure       = {0};

__IO ITStatus uartState     = RESET;
__IO ITStatus status        = RESET;
__IO ITStatus AlarmRTC      = RESET;

uint8_t RxByte;
uint8_t RxBuffer[30];
uint8_t BufferTemp[30];
uint32_t tick= 0;
uint16_t yearConversion = 2000;

void UART_Init(void);
void RTC_Init(void);
void showClock(void);
void showAlarm(void);
void rtcTask(void);
void serialTask(void);

HAL_StatusTypeDef setTime(uint8_t hour, uint8_t minutes, uint8_t seconds);
HAL_StatusTypeDef setDate(uint8_t day, uint8_t month, uint8_t year);
HAL_StatusTypeDef setAlarm(uint8_t hour, uint8_t minutes);

extern void initialise_monitor_handles(void);

int main( void )
{
    initialise_monitor_handles();
    printf("\n");

    HAL_Init( );
    UART_Init();
    RTC_Init();
    tick = HAL_GetTick();
    
    for (; ;)
    {
        serialTask();
        rtcTask();
 
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
    RTC_TimeTypeDef RTC_TImeConfig          = {0};
    RTC_DateTypeDef RTC_DateConfig          = {0};
    RTC_AlarmTypeDef RTC_AlarmConfig        = {0};
    __HAL_RCC_RTC_ENABLE();
    RTC_InitStructure.Instance              = RTC;
    RTC_InitStructure.Init.HourFormat       = RTC_HOURFORMAT_24;
    RTC_InitStructure.Init.AsynchPrediv     = 127;
    RTC_InitStructure.Init.SynchPrediv      = 255;
    RTC_InitStructure.Init.OutPut           = RTC_OUTPUT_DISABLE;
    HAL_RTC_Init(&RTC_InitStructure);

    RTC_TImeConfig.Hours = 05;
    RTC_TImeConfig.Minutes = 15;
    RTC_TImeConfig.Seconds = 30;
    RTC_TImeConfig.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    RTC_TImeConfig.StoreOperation = RTC_STOREOPERATION_RESET;
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

void serialTask(void)
{
    char *InpuyComand, *parametro;

    HAL_StatusTypeDef msgOutput = HAL_ERROR;
    
    int8_t sel          = -1;
    uint8_t hour_day    = 0;
    uint8_t min_month   = 0;
    uint16_t sec_year   = 0;

    if (status == SET)
        {
            status = RESET;
            memcpy((char*)BufferTemp,(const char*)RxBuffer,strlen((const char*)RxBuffer));

            InpuyComand = strtok((char*)RxBuffer, "=" );

            for (uint8_t i = 0; i < 3; i++)
            {
                if (strcmp(InpuyComand, comando_AT[i]) == 0)
                {
                    HAL_UART_Transmit_IT(&UartHandle,(uint8_t*)comando_AT[i],strlen(comando_AT[i]));
                    msgOutput = HAL_OK;
                    sel = i;
                    break;   
                }
            }
            
            switch (sel)
            {
            case 0:
                parametro = strtok(NULL, "," );
                hour_day = atoi(parametro);

                parametro = strtok(NULL, "," );
                min_month = atoi(parametro);

                parametro = strtok(NULL, "," );
                sec_year  = atoi(parametro);
                msgOutput = setTime(hour_day, min_month, sec_year);
                break;
            case 1:
                parametro = strtok(NULL, "," );
                hour_day = atoi(parametro);

                parametro = strtok(NULL, "," );
                min_month = atoi(parametro);

                parametro = strtok(NULL, "," );
                sec_year  = atoi(parametro);
                msgOutput = setDate(hour_day, min_month, sec_year);
                break;
            case 2:
                parametro = strtok(NULL, "," );
                hour_day = atoi(parametro);

                parametro = strtok(NULL, "," );
                min_month = atoi(parametro);
                msgOutput = setAlarm(hour_day, min_month);
                break;
            case -1:
                msgOutput = HAL_ERROR;
                break;
            }

            if (msgOutput == HAL_ERROR)
            {
                HAL_UART_Transmit_IT(&UartHandle,(uint8_t*)msgError,strlen(msgError));
            }
            else
            {
                HAL_UART_Transmit_IT(&UartHandle,(uint8_t*)msgOK,strlen(msgOK));
            }
            
            
                   
    }
}


HAL_StatusTypeDef setTime(uint8_t hour, uint8_t minutes, uint8_t seconds)
{
    HAL_StatusTypeDef   flag    = HAL_ERROR;
    RTC_TimeTypeDef     sTime   = {0};

    if ((hour < 24) && (minutes < 60) && (seconds < 60))
    {
        flag = HAL_OK;    
    }

    if (flag == HAL_OK)
    {
        sTime.Hours             = hour;
        sTime.Minutes           = minutes;
        sTime.Seconds           = seconds;
        sTime.DayLightSaving    = RTC_DAYLIGHTSAVING_NONE;
        sTime.StoreOperation    = RTC_STOREOPERATION_RESET;
        HAL_RTC_SetTime(&RTC_InitStructure,&sTime,RTC_FORMAT_BIN);
    }
    
    return flag;
}

HAL_StatusTypeDef setDate(uint8_t day, uint8_t month, uint8_t year)
{
    HAL_StatusTypeDef   flag    = HAL_ERROR;
    RTC_DateTypeDef     sDate   = {0};

    if ((day <= 30) && (month <= 12) && (year <= 9999))
    {
        flag = HAL_OK;
    }
    
    if (flag == HAL_OK)
    {
        sDate.Date  = day;
        sDate.Month = month;
        sDate.Year  = year;
        HAL_RTC_SetDate(&RTC_InitStructure,&sDate,RTC_FORMAT_BIN);
    }
    
    return flag;
}

HAL_StatusTypeDef setAlarm(uint8_t hour, uint8_t minutes)
{
    HAL_StatusTypeDef   flag    = HAL_ERROR;
    RTC_AlarmTypeDef    sAlarm  = {0};

    if (hour < 24 && minutes < 59)
    {
        flag = HAL_OK;
    }
    
    if (flag == HAL_OK)
    {
        sAlarm.Alarm = RTC_ALARM_A;
        sAlarm.AlarmTime.Hours = hour;
        sAlarm.AlarmTime.Minutes = minutes;
        sAlarm.AlarmTime.Seconds = 0;
        sAlarm.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;
        HAL_RTC_SetAlarm_IT(&RTC_InitStructure,&sAlarm,RTC_FORMAT_BIN);
    }
    

    return flag;
}

void showClock(void)
{
    RTC_TimeTypeDef     gTime  = {0};
    RTC_DateTypeDef     gDate  = {0};
    RTC_AlarmTypeDef    gAlarm = {0};

    if (HAL_GetTick() - tick > 1000)
        {
            tick = HAL_GetTick();
            HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_5);
            HAL_RTC_GetTime(&RTC_InitStructure,&gTime,RTC_FORMAT_BIN);
            HAL_RTC_GetDate(&RTC_InitStructure,&gDate,RTC_FORMAT_BIN);
            printf("%02d:%02d:%02d - ",gTime.Hours, gTime.Minutes, gTime.Seconds);
            printf("%02d/%02d/%04d\n",gDate.Date, gDate.Month, gDate.Year+yearConversion);
        }

        if (!HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13))
        {
            HAL_RTC_GetAlarm(&RTC_InitStructure,&gAlarm,RTC_ALARM_A,RTC_FORMAT_BIN);
            printf("Alarm - %02d:%02d:%02d\n",gAlarm.AlarmTime.Hours, gAlarm.AlarmTime.Minutes, gAlarm.AlarmTime.Seconds);
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

void rtcTask(void)
{
    if (AlarmRTC == SET )
    {
        showAlarm();
    }
    else
    {
        showClock();
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
        RxBuffer[i-1] = '\0';
        status = SET;
        i=0;
    }
    HAL_UART_Receive_IT(&UartHandle,&RxByte,1);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    AlarmRTC = SET;
}