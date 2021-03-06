#include "app_clock.h"
#include "lcd.h"
#include "queue.h"
#include "temp.h"

#define CLOCK_IDLE          0U
#define CLOCK_SHOW          1U
#define CLOCK_SHOW_ALARM    2U
#define CLOCK_SET_DATA      3U 
#define CLOCK_ALARM_UP      4U

#define CLOCK_STORE_DATA    5U

#define TIME_TICK_TRANSITION     20U
#define I2C_TEMP_SENSOR_TIMMING  0x10815E89

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

void clock_Store_data(void);

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
 * @brief  Actualiza la informacion del TEMP Alarm
 * 
 * @param uint8_t lower, Limite inferior 
 * 
 * @param uint8_t upper, Limite superior
 * 
 * @return NONE (VOID)
*/
void setTemp(uint8_t lower, uint8_t upper);

void SetTimeLog(uint16_t timeLog);

void memory_crateLog(uint8_t *bufferMemory);

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
 * @return NONE (VOID)
*/
void sprint_Time(char* buffer,RTC_TimeTypeDef TimeData);

/**
 * @brief  Realiza la configuracion de la informacion de TIME-ALARM para mostrar por LCD
 * 
 * @param char* buffer, Buffer donde se gurada la informacion
 * 
 * @param RTC_TimeTypeDef TimeData, Estrucutra que contiene los datos que se mostraran 
 * 
 * @param uint8_t stars, If stars are equal to 1, the LCD show "*" in the edges
 * 
 * @return NONE (VOID)
*/
void sprint_TimeAlarm(char* buffer,RTC_TimeTypeDef TimeData, uint8_t stars);

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
 * @brief  Inicializacion del modulo I2C
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void i2c_init(void);

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

/*
    Typedef Functions
*/
typedef void (*clockSelection)(void);

/*
    Private variables
*/
const char * months[] = {" ","ENE","FEB","MAR","ABR","MAY","JUN","JUL","AGO","SEP","OCT","NOV","DIC"};
const char * days[] = {"Do","Lu","Ma","Mi","Ju","Vi","Sa"};
const char* nAlarm = "NO ALARMS CONFIG ";
const char* DateRTC_memory      = {"D:"};
const char* TimeRTC_memory      = {"T:"};
const char* Temperature_memory  = {"TP:"};

RTC_HandleTypeDef              RTC_InitStructure       = {0};
static RTC_TimeTypeDef         RTC_TImeConfig          = {0};
static RTC_DateTypeDef         RTC_DateConfig          = {0};
static RTC_AlarmTypeDef        RTC_AlarmConfig         = {0};
LCD_HandleTypeDef              lcd_display             = {0};
SPI_HandleTypeDef              spi_Handle              = {0};
I2C_HandleTypeDef              i2c_Handle              = {0};
TEMP_HandleTypeDef             temp_Handle             = {0};
EEPROM_HandleTypeDef           eeprom_Handle           = {0};

static clockSelection clockSelectionFun[] = {clockIdle,showClock,clockShowAlarm,clockSetData,showAlarmUp,clock_Store_data};

static uint16_t yearConversion  = 2000U;
static uint32_t tickTime        = 0UL;
static uint32_t tickTimeLog     = 0UL;
static uint32_t tickTimeLogMod  = 10UL;
static Serial_MsgTypeDef    SerialSet_Data;

__IO ITStatus AlarmRTC               = RESET; // Flag interrupt RTC
__IO ITStatus Alarm_Active           = RESET;
__IO ITStatus Alarm_TEMP             = RESET;
__IO ITStatus Alarm_TEMP_Active      = RESET; // Flag interrupt TEMP
__IO ITStatus EEPROM_TimeLog_Act     = RESET; // Flag interrupt TEMP
__IO static uint8_t clockState       = CLOCK_IDLE;

/*
    extern variables
*/
// extern void initialise_monitor_handles(void);

void clock_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    // initialise_monitor_handles();
    // printf("\n");
    spi_init();
    i2c_init();
    lcd_init();
    temp_Handle.I2cHandler = &i2c_Handle;
    MOD_TEMP_Init(&temp_Handle);
    eeprom_Handle.SpiHandler = &spi_Handle;
    eeprom_Init(&eeprom_Handle);

    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStructure.Pin = GPIO_BUTTON_PIN;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_PULLUP;
    HAL_GPIO_Init(GPIO_BUTTON_PORT,&GPIO_InitStructure);


    __HAL_RCC_RTC_ENABLE();
    RTC_InitStructure.Instance              = RTC;
    RTC_InitStructure.Init.HourFormat       = RTC_HOURFORMAT_24;
    RTC_InitStructure.Init.AsynchPrediv     = 127U;
    RTC_InitStructure.Init.SynchPrediv      = 255U;
    RTC_InitStructure.Init.OutPut           = RTC_OUTPUT_DISABLE;
    HAL_RTC_Init(&RTC_InitStructure);

    RTC_TImeConfig.Hours = 5U;
    RTC_TImeConfig.Minutes = 15U;
    RTC_TImeConfig.Seconds = 30U;
    RTC_TImeConfig.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
    RTC_TImeConfig.StoreOperation = RTC_STOREOPERATION_RESET;
    HAL_RTC_SetTime(&RTC_InitStructure,&RTC_TImeConfig,RTC_FORMAT_BIN);

    RTC_DateConfig.Date = 22U;
    RTC_DateConfig.Month = RTC_MONTH_JUNE;
    RTC_DateConfig.Year = 21U;
    HAL_RTC_SetDate(&RTC_InitStructure,&RTC_DateConfig,RTC_FORMAT_BIN);

    RTC_AlarmConfig.Alarm = RTC_ALARM_A;
    RTC_AlarmConfig.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_WEEKDAY;
    RTC_AlarmConfig.AlarmTime.Hours = USER_RESET;
    RTC_AlarmConfig.AlarmTime.Minutes = USER_RESET;
    RTC_AlarmConfig.AlarmTime.Seconds = USER_RESET;
    RTC_AlarmConfig.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;
    HAL_RTC_SetAlarm_IT(&RTC_InitStructure,&RTC_AlarmConfig,RTC_FORMAT_BIN);
    HAL_RTC_DeactivateAlarm(&RTC_InitStructure,RTC_ALARM_A);
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
    yearConversion = (year - (year%100U)); 
    RTC_DateConfig.Date  = day;
    RTC_DateConfig.Month = month;
    RTC_DateConfig.Year  = (year%100U);
    HAL_RTC_SetDate(&RTC_InitStructure,&RTC_DateConfig,RTC_FORMAT_BIN);
    
}

void setAlarm(uint8_t hour, uint8_t minutes)
{
    Alarm_Active = SET;
    RTC_AlarmConfig.Alarm = RTC_ALARM_A;
    RTC_AlarmConfig.AlarmTime.Hours = hour;
    RTC_AlarmConfig.AlarmTime.Minutes = minutes;
    RTC_AlarmConfig.AlarmTime.Seconds = USER_RESET;
    RTC_AlarmConfig.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;
    HAL_RTC_SetAlarm_IT(&RTC_InitStructure,&RTC_AlarmConfig,RTC_FORMAT_BIN);
}

void setTemp(uint8_t lower, uint8_t upper)
{
    MOD_TEMP_SetAlarms(&temp_Handle,CONVERTION_TEMP_REG(lower),CONVERTION_TEMP_REG(upper)); 
    HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
    Alarm_TEMP = SET;
}

void SetTimeLog(uint16_t timeLog)
{
    tickTimeLogMod = timeLog;
}

void clockIdle(void)
{
    tickTime++;
    if ((tickTime % TIME_TICK_TRANSITION) == USER_RESET)
    {
        clockState = CLOCK_SHOW;
    }
    if (HAL_GPIO_ReadPin(GPIO_BUTTON_PORT,GPIO_BUTTON_PIN) == USER_RESET)
    {
        clockState = CLOCK_SHOW_ALARM;
    }
    if (EEPROM_TimeLog_Act == SET)
    {
        EEPROM_TimeLog_Act = RESET;
        clockState = CLOCK_STORE_DATA;
        // (void) printf("Memory Write\n");
    }
    if((AlarmRTC == SET) || (Alarm_TEMP_Active == SET))
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
    tickTime++;
    tickTimeLog++;
    uint8_t           buffer[17]    = {0};
    RTC_TimeTypeDef     gTime       = {0};
    RTC_DateTypeDef     gDate       = {0};
    
    HAL_RTC_GetTime(&RTC_InitStructure,&gTime,RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&RTC_InitStructure,&gDate,RTC_FORMAT_BIN);

    sprint_Date((char*)buffer,gDate);
    MOD_LCD_String(&lcd_display,(char*)buffer);

    MOD_LCD_SetCursor(&lcd_display,2U,1U);
    sprint_Time((char*)buffer,gTime);
    MOD_LCD_String(&lcd_display,(char*)buffer);
    if (__HAL_RTC_ALARM_GET_IT_SOURCE(&RTC_InitStructure,RTC_ALARM_A) == SET)
    {
        MOD_LCD_SetCursor(&lcd_display,2U,15U);
        MOD_LCD_Data(&lcd_display,'A');
    }
    if (Alarm_TEMP == SET)
    {
        MOD_LCD_SetCursor(&lcd_display,2U,16U);
        MOD_LCD_Data(&lcd_display,'T');
    }
    if ((tickTimeLog % tickTimeLogMod) == 0UL)
    {
        EEPROM_TimeLog_Act = SET;
    }
    
    clockState = CLOCK_IDLE; 
}

void showAlarmUp(void)
{
    static uint8_t time = 0;
    RTC_TimeTypeDef     gTime       = {0};
    RTC_DateTypeDef     gDate       = {0};
    uint8_t           buffer[17]    = {0};

    if ((AlarmRTC == SET) && (Alarm_Active == SET))
    {
        Alarm_Active = RESET;
        AlarmRTC = RESET;
        setAlarm(0U,0U);
        HAL_RTC_DeactivateAlarm(&RTC_InitStructure,RTC_ALARM_A);
    }
    if ((Alarm_TEMP_Active == SET) && (Alarm_TEMP == SET)) 
    {
        Alarm_TEMP_Active = RESET;
        Alarm_TEMP = RESET;
        MOD_TEMP_DisableAlarm(&temp_Handle);
        HAL_NVIC_DisableIRQ(EXTI2_3_IRQn);
    }

    MOD_LCD_SetCursor(&lcd_display,2U,1U);
    if ((tickTime % TIME_TICK_TRANSITION) == USER_RESET)
    {
        HAL_RTC_GetTime(&RTC_InitStructure,&gTime,RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&RTC_InitStructure,&gDate,RTC_FORMAT_BIN);
        if ( (time%2U) != 0UL )
        {
            sprint_TimeAlarm((char*)buffer,gTime,USER_SET);
        }
        else
        {
            sprint_TimeAlarm((char*)buffer,gTime,USER_RESET);
        }

        time++;
        MOD_LCD_String(&lcd_display,(char*)buffer);
    }
    
    if ((HAL_GPIO_ReadPin(GPIO_BUTTON_PORT,GPIO_BUTTON_PIN) == USER_RESET) || (time > 59U))
    {
        clockState = CLOCK_IDLE;
    }
    tickTimeLog++;
    tickTime++;
    if ((tickTimeLog % tickTimeLogMod) == 0UL)
    {
        EEPROM_TimeLog_Act = SET;
    }      
}

void clockShowAlarm(void)
{
    tickTime++;
    uint8_t           buffer[17]    = {0};
    RTC_AlarmTypeDef    gAlarm      = {0};
    static uint8_t      flagButon   = 0;

    HAL_RTC_GetAlarm(&RTC_InitStructure,&gAlarm,RTC_ALARM_A,RTC_FORMAT_BIN);
    if ((flagButon == USER_RESET) && (HAL_GPIO_ReadPin(GPIO_BUTTON_PORT,GPIO_BUTTON_PIN) == USER_RESET))
    {
        MOD_LCD_SetCursor(&lcd_display,2U,1U);
        if ((__HAL_RTC_ALARM_GET_IT_SOURCE(&RTC_InitStructure,RTC_ALARM_A) == USER_SET) || (Alarm_TEMP == SET))
        {
            sprint_Alarm((char*)buffer,gAlarm);
            MOD_LCD_String(&lcd_display,(char*)buffer);
        }
        else
        {
            //The variable NAlarm is const but it value no is nedded tho modify 
            MOD_LCD_String(&lcd_display,(char*)nAlarm); /* cppcheck-suppress misra-c2012-11.8 */
        }
        flagButon = USER_SET;
    }
    else if (!HAL_GPIO_ReadPin(GPIO_BUTTON_PORT,GPIO_BUTTON_PIN))
    {
        clockState = CLOCK_SHOW_ALARM;
    }
    else
    {
        flagButon = USER_RESET;
        clockState = CLOCK_IDLE;
    }
}

void clockSetData(void)
{
    tickTime++;

    switch (SerialSet_Data.msg)
    {
    case TIME:
        setTime(SerialSet_Data.param1,SerialSet_Data.param2,SerialSet_Data.param3);
        break;
    
    case DATE:
        setDate(SerialSet_Data.param1,SerialSet_Data.param2,SerialSet_Data.param3);
        break;

    case ALARM:
        setAlarm(SerialSet_Data.param1,SerialSet_Data.param2);
        break;

    case TEMP:
        setTemp(SerialSet_Data.param1,SerialSet_Data.param2);
        break;

    case MEMORY_TIME_LOG:
        SetTimeLog(SerialSet_Data.param3);
        break;
    default:
        break;
    }
    clockState = CLOCK_IDLE;
}

void clock_Store_data(void)
{   
    static uint16_t memory_addr = 0;
    uint8_t memoryBufferLog[PAGE_SIZE_EEPROM] = {0};
    memory_crateLog(memoryBufferLog);
    uint8_t size_buffer = strlen((const char*)memoryBufferLog);
    static uint8_t eeprom_Write_InProgrees = 0U;

    if (size_buffer < CHECK_SIZE_DIR(memory_addr))
    {
        if (eeprom_Write_InProgrees == 0U)
        {
            eeprom_write_data(&eeprom_Handle,memory_addr,memoryBufferLog,size_buffer);
            clockState = CLOCK_IDLE;
        }
        else
        {
            size_buffer = size_buffer - eeprom_Write_InProgrees;
            eeprom_write_data(&eeprom_Handle,memory_addr,&memoryBufferLog[eeprom_Write_InProgrees],size_buffer);
            eeprom_Write_InProgrees = 0U;
            clockState = CLOCK_IDLE;
        }
        
    }
    else
    {
        if ((size_buffer + memory_addr) > (TOTAL_ADDR_EEPROM - 1UL))
        {
            memory_addr = 0UL;
        }
        size_buffer = CHECK_SIZE_DIR(memory_addr);
        eeprom_write_data(&eeprom_Handle,memory_addr,memoryBufferLog,size_buffer);
        eeprom_Write_InProgrees = size_buffer;
    }

    memory_addr += size_buffer;
    if (memory_addr >= TOTAL_ADDR_EEPROM)
    {
        memory_addr = 0UL;
    }
}

void spi_init(void)
{
    spi_Handle.Instance                  = SPI1;
    spi_Handle.Init.Mode                 = SPI_MODE_MASTER;
    spi_Handle.Init.BaudRatePrescaler    = SPI_BAUDRATEPRESCALER_256;   
    spi_Handle.Init.Direction            = SPI_DIRECTION_2LINES;
    spi_Handle.Init.CLKPhase             = SPI_PHASE_2EDGE;
    spi_Handle.Init.CLKPolarity          = SPI_POLARITY_HIGH;
    spi_Handle.Init.CRCCalculation       = SPI_CRCCALCULATION_DISABLE;
    spi_Handle.Init.CRCPolynomial        = 7U;
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

void i2c_init(void)
{
    i2c_Handle.Instance = I2C1;
    i2c_Handle.Init.Timing = I2C_TEMP_SENSOR_TIMMING;
    i2c_Handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    i2c_Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    i2c_Handle.Init.OwnAddress2 = 0U;
    i2c_Handle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    i2c_Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    i2c_Handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&i2c_Handle);

}

void memory_crateLog(uint8_t *bufferMemory)
{
    uint8_t memoryBufferLog[PAGE_SIZE_EEPROM] = {0};
    uint8_t memoryTempDecToStr[3] = {0};

    uint16_t temperature = 0;
    RTC_TimeTypeDef     gTime       = {0};
    RTC_DateTypeDef     gDate       = {0};
    HAL_RTC_GetTime(&RTC_InitStructure,&gTime,RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&RTC_InitStructure,&gDate,RTC_FORMAT_BIN);
    temperature = MOD_TEMP_Read(&temp_Handle);
    temperature = TEMP_CONVERTION_DEC(temperature);

    /*Date --------------------------------------------------------------------*/
    (void) strcat((char*)memoryBufferLog,DateRTC_memory);
    DecToStr(memoryTempDecToStr,gDate.Month);
    (void) strcat((char*)memoryBufferLog,(const char*)memoryTempDecToStr);
    (void) strcat((char*)memoryBufferLog,",");

    (void) CLEAR_BUFFER(memoryTempDecToStr);
    DecToStr(memoryTempDecToStr,gDate.Date);
    (void) strcat((char*)memoryBufferLog,(const char*)memoryTempDecToStr);
    (void) strcat((char*)memoryBufferLog,",");

    (void) CLEAR_BUFFER(memoryTempDecToStr);
    DecToStr(memoryTempDecToStr,gDate.Year);
    (void) strcat((char*)memoryBufferLog,(const char*)memoryTempDecToStr);
    (void) strcat((char*)memoryBufferLog,";");
    /*----------------------------------------------------------------------------*/

    /*Time ----------------------------------------------------------------------*/
    (void) CLEAR_BUFFER(memoryTempDecToStr);
    (void) strcat((char*)memoryBufferLog,TimeRTC_memory);
    DecToStr(memoryTempDecToStr,gTime.Hours);
    (void) strcat((char*)memoryBufferLog,(const char*)memoryTempDecToStr);
    (void) strcat((char*)memoryBufferLog,",");

    (void) CLEAR_BUFFER(memoryTempDecToStr);
    DecToStr(memoryTempDecToStr,gTime.Minutes);
    (void) strcat((char*)memoryBufferLog,(const char*)memoryTempDecToStr);
    (void) strcat((char*)memoryBufferLog,",");

    (void) CLEAR_BUFFER(memoryTempDecToStr);
    DecToStr(memoryTempDecToStr,gTime.Seconds);
    (void) strcat((char*)memoryBufferLog,(const char*)memoryTempDecToStr);
    (void) strcat((char*)memoryBufferLog,";");
    /*----------------------------------------------------------------------------*/

    /*Temperature ----------------------------------------------------------------*/
    (void) CLEAR_BUFFER(memoryTempDecToStr);
    (void) strcat((char*)memoryBufferLog,Temperature_memory);
    DecToStr(memoryTempDecToStr,temperature);
    (void) strcat((char*)memoryBufferLog,(const char*)memoryTempDecToStr);
    (void) strcat((char*)memoryBufferLog,";");
    /*----------------------------------------------------------------------------*/
    
    /*Transfer the frame ----------------------------------------------------------*/
    (void) strcat((char*)memoryBufferLog,"  ");
    (void) strcpy((char*)bufferMemory,(const char*)memoryBufferLog);
    /*-----------------------------------------------------------------------------*/
}

uint8_t dayOfWeek(uint8_t d, uint8_t m, uint16_t y)
{
    uint8_t a = ((14U - m)/12U);
    y = y - a; /* cppcheck-suppress misra-c2012-17.8 */
    m = m + (12U*a) -2U; /* cppcheck-suppress misra-c2012-17.8 */

    d = (d + y + (y/4U) - (y/100U) + (y/400U) + ((31U*m)/12U)) % 7U; /* cppcheck-suppress misra-c2012-17.8 */
    return d; /* cppcheck-suppress misra-c2012-17.8 */
}


void DecToStr(uint8_t *buffer, int32_t val)
{
    uint8_t nElements = number_digits(val)+1U;
    uint8_t bufferTemp[nElements]; /* cppcheck-suppress misra-c2012-18.8 */
    uint8_t i;
    int32_t valM = val;
    if (valM < (int32_t)0U)
    {   valM *= (-1L); 
        for (i = 1U; i <= nElements; i++)
        {
            bufferTemp[nElements - i] = ((valM % (int32_t)10UL) + '0');
            valM/=10L; 
        }
        bufferTemp[i - 1U] = '\0';
        bufferTemp[0U] = '-';
        (void) strcpy((char *)buffer,(const char*)bufferTemp);
    }
    else if(valM == (int32_t)0U)
    {
        (void) strcpy((char *)buffer,"0\0");
    }
    else
    {
        for (i = 1U; i <= nElements; i++)
        {
            bufferTemp[nElements - i] = ((valM % (int32_t)10UL) + '0');
            valM/=10L; 
        }
        bufferTemp[i - 1U] = '\0';
        (void) strcpy((char *)buffer,(const char*)&bufferTemp[1U]);
    }

    
}

uint8_t number_digits(int32_t num)
{
    uint8_t count = 0U;
    int numM = num;

    if (numM < (int32_t)0U)
    {
        numM *= (-1L); 
    }
    while(numM > (int32_t)0)
    {
        count++;
        numM /= 10L; 
    }
    return count;
}

void sprint_Date(char* buffer, RTC_DateTypeDef DateData)
{
    (void)buffer;
    char buffernum[5] = {0};
    uint8_t  dayweekSel  = 0;
    MOD_LCD_SetCursor(&lcd_display,1,1);
    MOD_LCD_Data(&lcd_display,' ');
    MOD_LCD_String(&lcd_display,(char*)months[DateData.Month]); /* cppcheck-suppress misra-c2012-11.8 */
    MOD_LCD_Data(&lcd_display,',');
    DecToStr((uint8_t*)buffernum,DateData.Date);
    MOD_LCD_Data(&lcd_display,'0');
    if (strlen(buffernum) == (size_t)2)
    {
        MOD_LCD_SetCursor(&lcd_display,1,6);
    }
    MOD_LCD_String(&lcd_display,buffernum);
    (void) CLEAR_BUFFER(buffernum);

    MOD_LCD_SetCursor(&lcd_display,1,9);
    DecToStr((uint8_t*)buffernum,DateData.Year + yearConversion);
    MOD_LCD_String(&lcd_display,buffernum);
    MOD_LCD_Data(&lcd_display,' ');
    dayweekSel = dayOfWeek(DateData.Date, DateData.Month,DateData.Year+yearConversion);
    MOD_LCD_String(&lcd_display,(char*)days[dayweekSel]); /* cppcheck-suppress misra-c2012-11.8 */
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

void sprint_TimeAlarm(char* buffer,RTC_TimeTypeDef TimeData, uint8_t stars)
{
    char bufferTemp[17] = {0};
    char buffernum[3] = {0};
    uint16_t temperature = 0;

    temperature = MOD_TEMP_Read(&temp_Handle);

    if (stars == 0U)
    {
        (void) strcat(bufferTemp,"    ");
    }
    else
    {
        (void) strcat(bufferTemp,"*** ");
    }

    (void) strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,TimeData.Hours);
    if (strlen(buffernum) == 1UL)
    {
        (void) strcpy(&bufferTemp[5],buffernum);
    }
    else
    {
        (void) strcpy(&bufferTemp[4],buffernum);
    }
    (void) CLEAR_BUFFER(buffernum);

    (void) strcat(bufferTemp,":");
    (void) strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,TimeData.Minutes);
    if (strlen(buffernum) == 1UL)
    {
        (void) strcpy(&bufferTemp[8],buffernum);
    }
    else
    {
        (void) strcpy(&bufferTemp[7],buffernum);
    }
    (void) CLEAR_BUFFER(buffernum);
    
    //Temperatura
    (void) strcat(bufferTemp," ");
    (void) strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,TEMP_CONVERTION_DEC(temperature));
    if (strlen(buffernum) == 1UL)
    {
        (void) strcpy(&bufferTemp[11],buffernum);
    }
    else
    {
        (void) strcpy(&bufferTemp[10],buffernum);
    }
    (void) strcat(bufferTemp,"C");

    if (stars == 0U)
    {
        (void) strcat(bufferTemp,"    ");
    }
    else
    {
        (void) strcat(bufferTemp," ***");
    }
    (void) strcpy(buffer,bufferTemp);
}

void sprint_Time(char* buffer,RTC_TimeTypeDef TimeData)
{
    char bufferTemp[17] = {0};
    char buffernum[3] = {0};
    uint16_t temperature = 0;
    temperature = MOD_TEMP_Read(&temp_Handle);

    (void) strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,TimeData.Hours);
    if (strlen(buffernum) == 1UL)
    {
        (void) strcpy(&bufferTemp[1],buffernum);
    }
    else
    {
        (void) strcpy(&bufferTemp[0],buffernum);
    }
    (void) CLEAR_BUFFER(buffernum);
    (void) strcat(bufferTemp,":");
    (void) strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,TimeData.Minutes);
    if (strlen(buffernum) == 1UL)
    {
        (void) strcpy(&bufferTemp[4],buffernum);
    }
    else
    {
        (void) strcpy(&bufferTemp[3],buffernum);
    }
    (void) CLEAR_BUFFER(buffernum);
    (void) strcat(bufferTemp,":");
    (void) strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,TimeData.Seconds);
    if (strlen(buffernum) == 1UL)
    {
        (void) strcpy(&bufferTemp[7],buffernum);
    }
    else
    {
        (void) strcpy(&bufferTemp[6],buffernum);
    }
    (void) strcat(bufferTemp," ");
    (void) CLEAR_BUFFER(buffernum);
    (void) strcat(bufferTemp, " 00");
    if (TEMP_GREATHER_OR_LESS_THAN_0(temperature) == TEMP_LESS_THAN_0) 
    {
        (void) strcpy(&bufferTemp[9],"-");
        DecToStr((uint8_t*)buffernum, (256U-(TEMP_CONVERTION_DEC(temperature))));
    }
    else
    {
        DecToStr((uint8_t*)buffernum,TEMP_CONVERTION_DEC(temperature));
    }
    
    
    if (strlen(buffernum) == 1UL)
    {
        (void) strcpy(&bufferTemp[11],buffernum);
    }
    else
    {
        (void) strcpy(&bufferTemp[10],buffernum);
    }
    (void) strcat(bufferTemp, "C");
    (void) strcat(bufferTemp, "   ");
    (void) strcpy(buffer,bufferTemp);
}

void sprint_Alarm(char* buffer, RTC_AlarmTypeDef AlarmData)
{
    char           bufferTemp[17]  = {0};
    char             buffernum[3]  = {0};
    uint8_t         tempBuffer[2]  = {0};
    uint8_t             lowerTemp  = 0;
    uint8_t             upperTemp  = 0;

    MOD_TEMP_ReadRegister(&temp_Handle,tempBuffer,ALERT_TEMP_LOWER_B_TRIP_REGISTER);
    lowerTemp = READ_REGISTERS_AND_CONVERTION_TEMP_LIMITS(tempBuffer[0],tempBuffer[1]);
    MOD_TEMP_ReadRegister(&temp_Handle,tempBuffer,ALERT_TEMP_UPPER_B_TRIP_REGISTER);
    upperTemp = READ_REGISTERS_AND_CONVERTION_TEMP_LIMITS(tempBuffer[0],tempBuffer[1]);

    (void) strcat(bufferTemp,"TA ");
    (void) strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,AlarmData.AlarmTime.Hours);
    if (strlen(buffernum) == 1UL)
    {
        (void) strcpy(&bufferTemp[4],buffernum);
    }
    else
    {
        (void) strcpy(&bufferTemp[3],buffernum);
    }
    (void) CLEAR_BUFFER(buffernum);
    (void) strcat(bufferTemp,":");
    (void) strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,AlarmData.AlarmTime.Minutes);
    if (strlen(buffernum) == 1UL)
    {
        (void) strcpy(&bufferTemp[7],buffernum);
    }
    else
    {
        (void) strcpy(&bufferTemp[6],buffernum);
    }
    (void) CLEAR_BUFFER(buffernum);
    (void) strcat(bufferTemp," ");

    //Temperatura:
    (void) strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,lowerTemp);
    if (strlen(buffernum) == 1UL)
    {
        (void) strcpy(&bufferTemp[10],buffernum);
    }
    else
    {
        (void) strcpy(&bufferTemp[9],buffernum);
    }
    (void) CLEAR_BUFFER(buffernum);
    (void) strcat(bufferTemp, "-");
    (void) strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,upperTemp);
    if (strlen(buffernum) == 1UL)
    {
        (void) strcpy(&bufferTemp[13],buffernum);
    }
    else
    {
        (void) strcpy(&bufferTemp[12],buffernum);
    }
    (void) strcat(bufferTemp, "C");
    (void) strcat(bufferTemp, " ");
    (void) strcpy(buffer,bufferTemp);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc) 
{
    (void) hrtc;
    AlarmRTC = SET; 
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) 
{
    (void) GPIO_Pin;
    Alarm_TEMP_Active = SET;
}