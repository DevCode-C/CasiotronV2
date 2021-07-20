#include "app_clock.h"
#include "lcd.h"

#define CLOCK_IDLE          0U
#define CLOCK_SHOW          1U
#define CLOCK_SHOW_ALARM    2U
#define CLOCK_SET_DATA      3U 
#define CLOCK_ALARM_UP      4U 

#define TIME_TRANSITION     1000U

/**
 * @brief Verify the flags state and select the corresponding state 
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void clockIdle(void);

/**
 * @brief Show the actual time and date for LCD
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void showClock(void);

/**
 * @brief Show the active alarm when flag 'ALARMRTC' is equal to 'SET'
 *        
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void showAlarmUp(void);

/**
 * @brief Verify the found data type and set the correspondig state for set related information  
 *  
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void clockSetData(void);

/**
 * @brief Show the time when alarm goint to active, if not configuret, show "NO ALARM CONFIG"
 *
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void clockShowAlarm(void);

/**
 * @brief  RTC Time Update data
 * 
 * @param uint8_t hour, Decimal value of hour (i.e 0U - 23U)
 * 
 * @param uint8_t minutes, Decimal value of minutes (i.e 0U - 59U)
 * 
 * @param uint16_t seconds, Decimal value of seconds (i.e 0U - 59U)
 * 
 * @return NONE (VOID)
*/
void setTime(uint8_t hour, uint8_t minutes, uint16_t seconds);

/**
 * @brief  RTC Date Update data
 * 
 * @param uint8_t day, Decimal value of day (i.e 1U - 31U)
 * 
 * @param uint8_t mes, month, Decimal value of month (i.e 1U - 12U)
 * 
 * @param uint16_t year, Decimal value of year (i.e 0U - 9999U)
 * 
 * @return NONE (VOID)
*/
void setDate(uint8_t day, uint8_t month, uint16_t year);

/**
 * @brief  RTC Alarm Update data
 * 
 * @param uint8_t hour, Decimal value of hour (i.e 0U - 23U)
 * 
 * @param uint8_t minutes, Decimal value of minutes (i.e 0U - 59U)
 * 
 * @return NONE (VOID)
*/
void setAlarm(uint8_t hour, uint8_t minutes);


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

/**
 * @brief Make the configuration of the DATE information to display by LCD
 * 
 * @param char* buffer, *buffer,  Data storage 
 * 
 * @param RTC_DateTypeDef DateData, Structure, which contains the data related to DATE
 * 
 * @return NONE (VOID)
*/
void sprint_Date(char* buffer, RTC_DateTypeDef DateData);

/**
 * @brief  Make the configuration of the TIME information to display by LCD
 * 
 * @param char* buffer, *buffer,  Data storage 
 * 
 * @param RTC_TimeTypeDef TimeData, Structure, which contains the data related to TIME
 * 
 * @param uint8_t stars, If stars are equal to 1, the LCD show "*" in the edges
 * 
 * @return NONE (VOID)
*/
void sprint_Time(char* buffer,RTC_TimeTypeDef TimeData, uint8_t stars);

/**
 * @brief  Make the configuration of the ALARM information to display by LCD
 * 
 * @param char* buffer, *buffer,  Data storage 
 * 
 * @param RTC_AlarmTypeDef AlarmData, Structure, which contains the data related to ALARM
 * 
 * @return NONE (VOID)
*/
void sprint_Alarm(char* buffer, RTC_AlarmTypeDef AlarmData);

/**
 * @brief  LCD Module init
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void lcd_init(void);

/**
 * @brief  SPI Module init
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void spi_init(void);

/**
 * @brief  Zeller's congruence algorithm
 * 
 * @param uint8_t d, Decimal value of day (i.e 1U - 31U)
 * 
 * @param uint8_t m, Decimal value of month (i.e 1U - 12U)
 * 
 * @param uint16_t y, Decimal value of year (i.e 0U - 9999U)
 * 
 * @return uint8_t day of the week (i.e 0U - 6U)
*/
uint8_t dayOfWeek(uint8_t d, uint8_t m, uint16_t y);

typedef void (*clockSelection)(void);

const char * months[] = {" ","ENE","FEB","MAR","ABR","MAY","JUN","JUL","AGO","SEP","OCT","NOV","DIC"};
const char * days[] = {"Do","Lu","Ma","Mi","Ju","Vi","Sa"};
const char* nAlarm = "NO ALARM CONFIG ";

RTC_HandleTypeDef              RTC_InitStructure       = {0};
static RTC_TimeTypeDef         RTC_TImeConfig          = {0};
static RTC_DateTypeDef         RTC_DateConfig          = {0};
static RTC_AlarmTypeDef        RTC_AlarmConfig         = {0};
LCD_HandleTypeDef              lcd_display             = {0};
SPI_HandleTypeDef              spi_Handle              = {0};

static clockSelection clockSelectionFun[] = {clockIdle,showClock,clockShowAlarm,clockSetData,showAlarmUp};

static Serial_MsgTypeDef    SerialSet_Data;
extern QUEUE_HandleTypeDef  QueueSerialTx;
// extern void initialise_monitor_handles(void);

__IO ITStatus AlarmRTC               = RESET;
__IO ITStatus Alarm_Active           = RESET;
__IO static uint8_t clockState       = CLOCK_IDLE;

static uint16_t yearConversion  = 2000;
static uint32_t tick            = 0;

void clock_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // initialise_monitor_handles();
    // printf("\n");
    spi_init();
    lcd_init();
    
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStructure.Pin = GPIO_BUTTON_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIO_BUTTON_PORT,&GPIO_InitStructure);


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
    RTC_AlarmConfig.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
    RTC_AlarmConfig.AlarmTime.Hours = 00;
    RTC_AlarmConfig.AlarmTime.Minutes = 00;
    RTC_AlarmConfig.AlarmTime.Seconds = 00;
    RTC_AlarmConfig.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;
    HAL_RTC_SetAlarm_IT(&RTC_InitStructure,&RTC_AlarmConfig,RTC_FORMAT_BIN);
    HAL_RTC_DeactivateAlarm(&RTC_InitStructure,RTC_ALARM_A);
    
    tick = HAL_GetTick();
}

void clock_task(void)
{
    clockSelectionFun[clockState]();
}

void setTime(uint8_t hour, uint8_t minutes, uint16_t seconds)
{
    
    RTC_TImeConfig.Hours             = hour;
    RTC_TImeConfig.Minutes           = minutes;
    RTC_TImeConfig.Seconds           = seconds;
    HAL_RTC_SetTime(&RTC_InitStructure,&RTC_TImeConfig,RTC_FORMAT_BIN);
    
}

void setDate(uint8_t day, uint8_t month, uint16_t year)
{
    yearConversion = (year - (year%100)); 
    RTC_DateConfig.Date  = day;
    RTC_DateConfig.Month = month;
    RTC_DateConfig.Year  = (year%100);
    HAL_RTC_SetDate(&RTC_InitStructure,&RTC_DateConfig,RTC_FORMAT_BIN);
    
}

void setAlarm(uint8_t hour, uint8_t minutes)
{
    Alarm_Active = SET;
    RTC_AlarmConfig.Alarm = RTC_ALARM_A;
    RTC_AlarmConfig.AlarmTime.Hours = hour;
    RTC_AlarmConfig.AlarmTime.Minutes = minutes;
    RTC_AlarmConfig.AlarmTime.Seconds = 0;
    RTC_AlarmConfig.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;
    HAL_RTC_SetAlarm_IT(&RTC_InitStructure,&RTC_AlarmConfig,RTC_FORMAT_BIN);
}

void clockIdle(void)
{
    if (HAL_GetTick() - tick >= TIME_TRANSITION)
    {
        tick = HAL_GetTick();
        clockState = CLOCK_SHOW;
    }
    if (HAL_GPIO_ReadPin(GPIO_BUTTON_PORT,GPIO_BUTTON_PIN) == 0)
    {
        clockState = CLOCK_SHOW_ALARM;
    }
    if(AlarmRTC == SET)
    {
        clockState = CLOCK_ALARM_UP;
    }
    if(HIL_QUEUE_Read(&QueueSerialTx,&SerialSet_Data) == READ_OK)
    {
        clockState = CLOCK_SET_DATA;
    }
}

void showClock(void)
{
    uint8_t           buffer[17]    = {0};
    RTC_TimeTypeDef     gTime       = {0};
    RTC_DateTypeDef     gDate       = {0};
    
    HAL_RTC_GetTime(&RTC_InitStructure,&gTime,RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&RTC_InitStructure,&gDate,RTC_FORMAT_BIN);

    // MOD_LCD_SetCursor(&lcd_display,1,1); 
    sprint_Date((char*)buffer,gDate);
    MOD_LCD_String(&lcd_display,(char*)buffer);

    MOD_LCD_SetCursor(&lcd_display,2,1);
    sprint_Time((char*)buffer,gTime,0);
    MOD_LCD_String(&lcd_display,(char*)buffer);
    if (__HAL_RTC_ALARM_GET_IT_SOURCE(&RTC_InitStructure,RTC_ALARM_A))
    {
        MOD_LCD_SetCursor(&lcd_display,2,15);
        MOD_LCD_Data(&lcd_display,'A');
    }
    clockState = CLOCK_IDLE;
}

void showAlarmUp(void)
{
    static uint8_t time = 0;
    RTC_TimeTypeDef     gTime       = {0};
    RTC_DateTypeDef     gDate       = {0};
    uint8_t           buffer[17]    = {0};

    if (AlarmRTC == SET)
    {
        AlarmRTC = RESET;
        HAL_RTC_DeactivateAlarm(&RTC_InitStructure,RTC_ALARM_A);
    }

    MOD_LCD_SetCursor(&lcd_display,2,1);
    if (HAL_GetTick() - tick >= TIME_TRANSITION)
    {
        tick = HAL_GetTick();
        HAL_RTC_GetTime(&RTC_InitStructure,&gTime,RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&RTC_InitStructure,&gDate,RTC_FORMAT_BIN);
        if (time%2)
        {
            sprint_Time((char*)buffer,gTime,1);
        }
        else
        {
            sprint_Time((char*)buffer,gTime,0);
        }

        time++;
        MOD_LCD_String(&lcd_display,(char*)buffer);
    }
    
    if (!HAL_GPIO_ReadPin(GPIO_BUTTON_PORT,GPIO_BUTTON_PIN) || time > 59)
    {
        clockState = CLOCK_IDLE;
    }
           
}

void clockShowAlarm(void)
{
    
    uint8_t           buffer[17]    = {0};
    RTC_AlarmTypeDef    gAlarm      = {0};
    static uint8_t      flagButon   = 0;

    HAL_RTC_GetAlarm(&RTC_InitStructure,&gAlarm,RTC_ALARM_A,RTC_FORMAT_BIN);
    
    if (flagButon == 0 && !HAL_GPIO_ReadPin(GPIO_BUTTON_PORT,GPIO_BUTTON_PIN))
    {
        MOD_LCD_SetCursor(&lcd_display,2,1);
        if (__HAL_RTC_ALARM_GET_IT_SOURCE(&RTC_InitStructure,RTC_ALARM_A))
        {
            sprint_Alarm((char*)buffer,gAlarm);
            MOD_LCD_String(&lcd_display,(char*)buffer);
        }
        else
        {
            // MOD_LCD_SetCursor(&lcd_display,2,1);
            MOD_LCD_String(&lcd_display,(char*)nAlarm);
        }
        flagButon =1;
    }
    else if (!HAL_GPIO_ReadPin(GPIO_BUTTON_PORT,GPIO_BUTTON_PIN))
    {
        clockState = CLOCK_SHOW_ALARM;
    }
    else
    {
        flagButon = 0;
        clockState = CLOCK_IDLE;
    }
}

void clockSetData(void)
{
    if (SerialSet_Data.msg == TIME)
    {
        setTime(SerialSet_Data.param1,SerialSet_Data.param2,SerialSet_Data.param3);
    }
    else if (SerialSet_Data.msg == DATE)
    {
        setDate(SerialSet_Data.param1,SerialSet_Data.param2,SerialSet_Data.param3);
    }
    else if (SerialSet_Data.msg == ALARM)
    {
        setAlarm(SerialSet_Data.param1,SerialSet_Data.param2);
    }
    
    clockState = CLOCK_IDLE;
}

void spi_init(void)
{
    spi_Handle.Instance                  = SPI1;
    spi_Handle.Init.Mode                 = SPI_MODE_MASTER;
    spi_Handle.Init.BaudRatePrescaler    = SPI_BAUDRATEPRESCALER_256;   
    spi_Handle.Init.Direction            = SPI_DIRECTION_1LINE;
    spi_Handle.Init.CLKPhase             = SPI_PHASE_2EDGE;
    spi_Handle.Init.CLKPolarity          = SPI_POLARITY_LOW;
    spi_Handle.Init.CRCCalculation       = SPI_CRCCALCULATION_DISABLE;
    spi_Handle.Init.CRCPolynomial        = 7;
    spi_Handle.Init.DataSize             = SPI_DATASIZE_8BIT;
    spi_Handle.Init.FirstBit             = SPI_FIRSTBIT_MSB;
    spi_Handle.Init.NSS                  = SPI_NSS_SOFT;
    spi_Handle.Init.TIMode               = SPI_TIMODE_DISABLE;
    HAL_SPI_Init(&spi_Handle);
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

uint8_t dayOfWeek(uint8_t d, uint8_t m, uint16_t y)
{
    uint8_t a = ((14 - m)/12);
    y = y - a;
    m = m + (12*a) -2;

    d = (d + y + (y/4) - (y/100) + (y/400) + ((31*m)/12)) % 7;
    return d;
}


void DecToStr(uint8_t *buffer, int32_t val)
{
    uint8_t nElements = number_digits(val)+1;
    uint8_t bufferTemp[nElements];
    uint8_t i;
    if (val < 0)
    {   val *= (-1);
        for (i = 1; i <= nElements; i++)
        {
            bufferTemp[nElements - i] =(uint8_t) ((val % 10UL) + '0');
            val/=10;
        }
        bufferTemp[i - 1] = '\0';
        bufferTemp[0] = '-';
        strcpy((char *)buffer,(const char*)bufferTemp);
    }
    else if(val == 0)
    {
        strcpy((char *)buffer,"0\0");
    }
    else
    {
        for (i = 1; i <= nElements; i++)
        {
            bufferTemp[nElements - i] =(uint8_t) ((val % 10UL) + '0');
            val/=10;
        }
        bufferTemp[i - 1] = '\0';
        strcpy((char *)buffer,(const char*)&bufferTemp[1]);
    }

    
}

uint8_t number_digits(int32_t num)
{
    uint8_t count = 0;

    if (num < 0)
    {
        num *= (-1);
    }
    while(num > 0)
    {
        count++;
        num /= 10;
    }
    return count;
}

void sprint_Date(char* buffer, RTC_DateTypeDef DateData)
{
    char buffernum[5] = {0};
    uint8_t  dayweekSel  = 0;
    MOD_LCD_SetCursor(&lcd_display,1,1);
    MOD_LCD_Data(&lcd_display,' ');
    MOD_LCD_String(&lcd_display,(char*)months[DateData.Month]);
    MOD_LCD_Data(&lcd_display,',');
    DecToStr((uint8_t*)buffernum,DateData.Date);
    MOD_LCD_Data(&lcd_display,'0');
    if (strlen(buffernum) == 2)
    {
        MOD_LCD_SetCursor(&lcd_display,1,6);
    }
    MOD_LCD_String(&lcd_display,buffernum);
    CLEAR_BUFFER(buffernum);

    MOD_LCD_SetCursor(&lcd_display,1,9);
    DecToStr((uint8_t*)buffernum,DateData.Year + yearConversion);
    MOD_LCD_String(&lcd_display,buffernum);
    MOD_LCD_Data(&lcd_display,' ');
    dayweekSel = dayOfWeek(DateData.Date, DateData.Month,DateData.Year+yearConversion);
    MOD_LCD_String(&lcd_display,(char*)days[dayweekSel]);
    // uint8_t  dayweekSel  = 0;
    // strcat(bufferTemp," "); 
    // strcat(bufferTemp,months[DateData.Month]); 
    // strcat(bufferTemp,","); 
    // strcat(bufferTemp,"00");
    // DecToStr((uint8_t*)buffernum,DateData.Date);
    // if (strlen(buffernum) == 1)
    // {
    //     strcpy(&bufferTemp[6],buffernum);    
    // }
    // else
    // {
    //     strcpy(&bufferTemp[5],buffernum);
    // }
    // strcat(bufferTemp," ");
    // DecToStr((uint8_t*)buffernum,DateData.Year + yearConversion);
    // strcat(bufferTemp,buffernum);
    // strcat(bufferTemp," ");
    // dayweekSel = dayOfWeek(DateData.Date, DateData.Month,DateData.Year+yearConversion);
    // strcat(bufferTemp,days[dayweekSel]);
    // strcat(bufferTemp," ");
    // strcpy(buffer,bufferTemp);
}

void sprint_Time(char* buffer,RTC_TimeTypeDef TimeData, uint8_t stars)
{
    char bufferTemp[17] = {0};
    char buffernum[3] = {0};

    if (stars == 0)
    {
        strcat(bufferTemp,"    ");
        strcpy(&bufferTemp[12],"    ");
    }
    else
    {
        strcat(bufferTemp,"*** ");
        strcpy(&bufferTemp[12]," ***");
    }

    strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,TimeData.Hours);
    if (strlen(buffernum) == 1)
    {
        strcpy(&bufferTemp[5],buffernum);
    }
    else
    {
        strcpy(&bufferTemp[4],buffernum);
    }
    CLEAR_BUFFER(buffernum);
    strcat(bufferTemp,":");
    strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,TimeData.Minutes);
    if (strlen(buffernum) == 1)
    {
        strcpy(&bufferTemp[8],buffernum);
    }
    else
    {
        strcpy(&bufferTemp[7],buffernum);
    }
    CLEAR_BUFFER(buffernum);
    strcat(bufferTemp,":");
    strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,TimeData.Seconds);
    if (strlen(buffernum) == 1)
    {
        strcpy(&bufferTemp[11],buffernum);
    }
    else
    {
        strcpy(&bufferTemp[10],buffernum);
    }

    if (stars == 0)
    {
        strcat(bufferTemp,"    ");
    }
    else
    {
        strcat(bufferTemp," ***");
    }
    strcpy(buffer,bufferTemp);
}

void sprint_Alarm(char* buffer, RTC_AlarmTypeDef AlarmData)
{
    char bufferTemp[17] = {0};
    char buffernum[3] = {0};

    strcat(bufferTemp,"ALARM ");
    strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,AlarmData.AlarmTime.Hours);
    if (strlen(buffernum) == 1)
    {
        strcpy(&bufferTemp[7],buffernum);
    }
    else
    {
        strcpy(&bufferTemp[6],buffernum);
    }
    CLEAR_BUFFER(buffernum);
    strcat(bufferTemp,":");
    strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,AlarmData.AlarmTime.Minutes);
    if (strlen(buffernum) == 1)
    {
        strcpy(&bufferTemp[10],buffernum);
    }
    else
    {
        strcpy(&bufferTemp[9],buffernum);
    }
    CLEAR_BUFFER(buffernum);
    strcat(bufferTemp,":");
    strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,AlarmData.AlarmTime.Seconds);
    if (strlen(buffernum) == 1)
    {
        strcpy(&bufferTemp[13],buffernum);
    }
    else
    {
        strcpy(&bufferTemp[12],buffernum);
    }
    strcat(bufferTemp, " ");
    strcpy(buffer,bufferTemp);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    AlarmRTC = SET;
}