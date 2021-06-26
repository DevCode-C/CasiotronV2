#include "app_clock.h"

RTC_HandleTypeDef              RTC_InitStructure       = {0};
static RTC_TimeTypeDef         RTC_TImeConfig          = {0};
static RTC_DateTypeDef         RTC_DateConfig          = {0};
static RTC_AlarmTypeDef        RTC_AlarmConfig         = {0};
LCD_HandleTypeDef              lcd_display             = {0};

extern Serial_MsgTypeDef SerialTranferData;
extern SPI_HandleTypeDef spi_Handle;

__IO ITStatus AlarmRTC               = RESET;
__IO ITStatus Alarm_Active           = RESET;
__IO static uint8_t clockState       = CLOCK_IDLE;

static uint16_t yearConversion  = 2000;
static uint32_t tick            = 0;

void clock_init(void)
{
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
    RTC_AlarmConfig.AlarmDateWeekDay = 00;
    RTC_AlarmConfig.AlarmTime.Hours = 00;
    RTC_AlarmConfig.AlarmTime.Minutes = 00;
    RTC_AlarmConfig.AlarmTime.Seconds = 00;
    RTC_AlarmConfig.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;
    HAL_RTC_SetAlarm_IT(&RTC_InitStructure,&RTC_AlarmConfig,RTC_FORMAT_BIN);
    HAL_RTC_DeactivateAlarm(&RTC_InitStructure,RTC_ALARM_A);

    lcd_init();
    tick = HAL_GetTick();
}

void clock_task(void)
{
    uint8_t           buffer[16] = {0};
    RTC_AlarmTypeDef    gAlarm = {0};

    switch (clockState)
    {
        case CLOCK_IDLE:
            if (HAL_GetTick() - tick >= TIME_TRANSITION)
            {
                tick = HAL_GetTick();
                clockState = CLOCK_SHOW;
            }
            if (!HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13))
            {
                clockState = CLOCK_SHOW_ALARM;
            }
            else if(AlarmRTC == SET)
            {
                clockState = CLOCK_ALARM_UP;
            }
            else if(SerialTranferData.msg != NONE)
            {
                clockState = CLOCK_SET_DATA;
            }
            break;

        case CLOCK_SHOW:
            showClock();
            clockState = CLOCK_IDLE;
            break;

        case CLOCK_SHOW_ALARM:
            LCD_CLEAR(&lcd_display);
            HAL_RTC_GetAlarm(&RTC_InitStructure,&gAlarm,RTC_ALARM_A,RTC_FORMAT_BIN);
            printf("Alarm - %02d:%02d:%02d\n",gAlarm.AlarmTime.Hours, gAlarm.AlarmTime.Minutes, gAlarm.AlarmTime.Seconds);
            sprintf((char*)buffer,"%02d:%02d:%02d",gAlarm.AlarmTime.Hours, gAlarm.AlarmTime.Minutes, gAlarm.AlarmTime.Seconds);
            MOD_LCD_String(&lcd_display,(char*)buffer);
            while (!HAL_GPIO_ReadPin(GPIOC,GPIO_PIN_13));
            LCD_CLEAR(&lcd_display);
            clockState = CLOCK_IDLE;
            break;

        case CLOCK_SET_DATA:
            if (SerialTranferData.msg == TIME)
            {
                SerialTranferData.msg = NONE;
                setTime(SerialTranferData.param1,SerialTranferData.param2,SerialTranferData.param3);
            }
            else if (SerialTranferData.msg == DATE)
            {
                SerialTranferData.msg = NONE;
                setDate(SerialTranferData.param1,SerialTranferData.param2,SerialTranferData.param3);
            }
            else if (SerialTranferData.msg == ALARM)
            {
                SerialTranferData.msg = NONE;
                setAlarm(SerialTranferData.param1,SerialTranferData.param2);
            }
            
            clockState = CLOCK_IDLE;
            break;

        case CLOCK_ALARM_UP:
            showAlarm();
            clockState = CLOCK_IDLE;
            break;

        default:
            break;
    }
}

HAL_StatusTypeDef setTime(uint8_t hour, uint8_t minutes, uint16_t seconds)
{
    HAL_StatusTypeDef   flag    = HAL_ERROR;

    if ((hour < 24) && (minutes < 60) && (seconds < 60))
    {
        flag = HAL_OK;    
    }

    if (flag == HAL_OK)
    {
        RTC_TImeConfig.Hours             = hour;
        RTC_TImeConfig.Minutes           = minutes;
        RTC_TImeConfig.Seconds           = seconds;
        HAL_RTC_SetTime(&RTC_InitStructure,&RTC_TImeConfig,RTC_FORMAT_BIN);
    }
    
    return flag;
}

HAL_StatusTypeDef setDate(uint8_t day, uint8_t month, uint16_t year)
{
    HAL_StatusTypeDef   flag    = HAL_ERROR;

    if ((day <= 30) && (month <= 12) && (year <= 9999))
    {
        flag = HAL_OK;
    }
    
    if (flag == HAL_OK)
    {
        yearConversion = (year - (year%100)); 

        RTC_DateConfig.Date  = day;
        RTC_DateConfig.Month = month;
        RTC_DateConfig.Year  = (year%100);
        HAL_RTC_SetDate(&RTC_InitStructure,&RTC_DateConfig,RTC_FORMAT_BIN);
    }
    
    return flag;
}

HAL_StatusTypeDef setAlarm(uint8_t hour, uint8_t minutes)
{
    HAL_StatusTypeDef   flag    = HAL_ERROR;

    if (hour < 24 && minutes < 59)
    {
        flag = HAL_OK;
    }
    
    if (flag == HAL_OK)
    {
        Alarm_Active = SET;
        RTC_AlarmConfig.Alarm = RTC_ALARM_A;
        RTC_AlarmConfig.AlarmDateWeekDay = RTC_DateConfig.Date;
        RTC_AlarmConfig.AlarmTime.Hours = hour;
        RTC_AlarmConfig.AlarmTime.Minutes = minutes;
        RTC_AlarmConfig.AlarmTime.Seconds = 0;
        RTC_AlarmConfig.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;
        HAL_RTC_SetAlarm_IT(&RTC_InitStructure,&RTC_AlarmConfig,RTC_FORMAT_BIN);
    }
    

    return flag;
}

void showClock(void)
{
    uint8_t           buffer[16] = {0};
    RTC_TimeTypeDef     gTime  = {0};
    RTC_DateTypeDef     gDate  = {0};
    
    HAL_RTC_GetTime(&RTC_InitStructure,&gTime,RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&RTC_InitStructure,&gDate,RTC_FORMAT_BIN);
    printf("%02d:%02d:%02d - ",gTime.Hours, gTime.Minutes, gTime.Seconds);
    printf("%02d/%02d/%04d\n",gDate.Date, gDate.Month, gDate.Year+yearConversion);
    sprintf((char*)buffer,"%02d:%02d:%02d",gTime.Hours, gTime.Minutes, gTime.Seconds);
    MOD_LCD_SetCursor(&lcd_display,2,5);
    MOD_LCD_String(&lcd_display,(char*)buffer);
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
            HAL_Delay(1);
            break;
        }
           
    }
}

void lcd_init(void)
{
    lcd_display.SpiHandler = &spi_Handle;
    lcd_display.CsPort     = LCD_PORT;
    lcd_display.RsPort     = LCD_PORT;
    lcd_display.RstPort    = LCD_PORT;
    lcd_display.CsPin      = LCD_CS;
    lcd_display.RsPin      = LCD_RS;
    lcd_display.RstPin     = LCD_RST;

    MOD_LCD_Init(&lcd_display);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    AlarmRTC = SET;
}