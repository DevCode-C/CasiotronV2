#include "app_clock.h"
#include "lcd.h"
#include "queue.h"
#include "temp.h"

#define CLOCK_IDLE          0U
#define CLOCK_SHOW          1U
#define CLOCK_SHOW_ALARM    2U
#define CLOCK_SET_DATA      3U 
#define CLOCK_ALARM_UP      4U

#define CLOCK_ALARM_TEMP_UP 5U

#define TIME_TRANSITION     1000U

/**
 * @brief Funcion que verifica el estado de la banderas y selecciona el estado correspondiente
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void clockIdle(void);

/**
 * @brief Muestra por la lcd el tiempo actual del RTC
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void showClock(void);

/**
 * @brief Muestra por la lcd la alarma cuando se activo la bandera "ALARMRTC"
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void showAlarmUp(void);

/**
 * @brief Verifica el tipo de de dato y manda al estado correpondiente para asignar la informacion
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void clockSetData(void);

/**
 * @brief Muestra por la lcd la hora de activacion de alarma o si no esta configurada
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void clockShowAlarm(void);

/**
 * @brief  Actualiza la informacion del RTC Time
 * 
 * @param uint8_t hour, Valor decimal de la nueva hora
 * 
 * @param uint8_t minutes, Valor decima de los nuevos minutos
 * 
 * @param uint16_t seconds, Valor decimal de los segundos
 * 
 * @return NONE (VOID)
*/
void setTime(uint8_t hour, uint8_t minutes, uint16_t seconds);

/**
 * @brief  Actualiza la informacion del RTC Date
 * 
 * @param uint8_t dia, Valor decimal del dia
 * 
 * @param uint8_t mes, Valor decima del mes
 * 
 * @param uint16_t año, Valor decimal del año
 * 
 * @return NONE (VOID)
*/
void setDate(uint8_t day, uint8_t month, uint16_t year);

/**
 * @brief  Actualiza la informacion del RTC Alarm
 * 
 * @param uint8_t hour, Valor decimal de la nueva hora
 * 
 * @param uint8_t minutes, Valor decima de los nuevos minutos
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
void setTemp(int8_t lower, uint8_t upper);

/**
 * @brief  Convierte valores deciamles a caracteres para alacenarlos en el buffer
 * 
 * @param uint8_t *buffer, buffer donde se almacena la informacion 
 * 
 * @param int32_t val, valor decimal para convertir 
 * 
 * @return NONE (VOID)
*/
void DecToStr(uint8_t *buffer, int32_t val);


/**
 * @brief  Verifica la cantida de digitos en un valor decimal 
 * 
 * @param int32_t num, Valor decimal a contar
 * 
 * @return uint8_t, el numero de digitos contados
*/
uint8_t number_digits(int32_t num);

/**
 * @brief  Realiza la configuracion de la informacion de DATE para mostrar por LCD
 * 
 * @param char* buffer, Buffer donde se gurada la informacion
 * 
 * @param RTC_DateTypeDef DateData, Estrucutra que contiene los datos que se mostraran 
 * 
 * @return NONE (VOID)
*/
void sprint_Date(char* buffer, RTC_DateTypeDef DateData);

/**
 * @brief  Realiza la configuracion de la informacion de TIME para mostrar por LCD
 * 
 * @param char* buffer, Buffer donde se gurada la informacion
 * 
 * @param RTC_TimeTypeDef TimeData, Estrucutra que contiene los datos que se mostraran 
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
 * @param uint8_t stars, Bandera que selecciona el mostrar "*" o " "
 * 
 * @return NONE (VOID)
*/
void sprint_TimeAlarm(char* buffer,RTC_TimeTypeDef TimeData, uint8_t stars);

/**
 * @brief  Realiza la configuracion de la informacion de ALARM para mostrar por LCD
 * 
 * @param char* buffer, Buffer donde se gurada la informacion
 * 
 * @param RTC_AlarmTypeDef AlarmData, Estrucutra que contiene los datos que se mostraran 
 * 
 * @return NONE (VOID)
*/
void sprint_Alarm(char* buffer, RTC_AlarmTypeDef AlarmData);

/**
 * @brief  Inicializacion del modulo LCD
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void lcd_init(void);

/**
 * @brief  Inicializacion del modulo SPI
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
 * @brief  Algoritmo de congruencia de Zeller,
 * 
 * @param uint8_t d, numero del dia de la semana
 * 
 * @param uint8_t m, numero del mes 
 * 
 * @param uint16_t y, año
 * 
 * @return uint8_t El numero del dia de la semana acorde al d,m,y
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

RTC_HandleTypeDef              RTC_InitStructure       = {0};
static RTC_TimeTypeDef         RTC_TImeConfig          = {0};
static RTC_DateTypeDef         RTC_DateConfig          = {0};
static RTC_AlarmTypeDef        RTC_AlarmConfig         = {0};
LCD_HandleTypeDef              lcd_display             = {0};
SPI_HandleTypeDef              spi_Handle              = {0};
I2C_HandleTypeDef              i2c_Handle              = {0};
TEMP_HandleTypeDef             temp_Handle             = {0};

static clockSelection clockSelectionFun[] = {clockIdle,showClock,clockShowAlarm,clockSetData,showAlarmUp};

static uint16_t yearConversion  = 2000;
static uint32_t tick            = 0;
static Serial_MsgTypeDef    SerialSet_Data;

__IO ITStatus AlarmRTC               = RESET;
__IO ITStatus Alarm_Active           = RESET;
__IO ITStatus Alarm_TEMP             = RESET;
__IO ITStatus Alarm_TEMP_Active      = RESET;
__IO static uint8_t clockState       = CLOCK_IDLE;

/*
    extern variables
*/
extern QUEUE_HandleTypeDef  QueueSerialTx;
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

void setTemp(int8_t lower, uint8_t upper)
{
    MOD_TEMP_SetAlarms(&temp_Handle,lower,upper);
    
    HAL_NVIC_SetPriority(EXTI2_3_IRQn,1,0);
    HAL_NVIC_EnableIRQ(EXTI2_3_IRQn);
    Alarm_TEMP = SET;
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
    if(AlarmRTC == SET || Alarm_TEMP_Active == SET)
    {
        clockState = CLOCK_ALARM_UP;
    }
    if(HIL_QUEUE_Read(&QueueSerialTx,&SerialSet_Data) == 1)
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

    // sprintf((char*)buffer," %s,%02d %04d %s",months[gDate.Month],gDate.Date,gDate.Year+yearConversion,days[dayweekSel]);
    // MOD_LCD_SetCursor(&lcd_display,1,1);
    // MOD_LCD_String(&lcd_display,(char*)buffer);

    // sprintf((char*)buffer,"%02d:%02d:%02d %02dC    ",gTime.Hours, gTime.Minutes, gTime.Seconds,temperature);
    // MOD_LCD_SetCursor(&lcd_display,1,1); 
    sprint_Date((char*)buffer,gDate);
    MOD_LCD_String(&lcd_display,(char*)buffer);

    MOD_LCD_SetCursor(&lcd_display,2,1);
    sprint_Time((char*)buffer,gTime);
    MOD_LCD_String(&lcd_display,(char*)buffer);
    if (__HAL_RTC_ALARM_GET_IT_SOURCE(&RTC_InitStructure,RTC_ALARM_A))
    {
        MOD_LCD_SetCursor(&lcd_display,2,15);
        MOD_LCD_Data(&lcd_display,'A');
    }
    if (Alarm_TEMP == SET)
    {
        MOD_LCD_SetCursor(&lcd_display,2,16);
        MOD_LCD_Data(&lcd_display,'T');
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
    if (Alarm_TEMP_Active == SET)
    {
        Alarm_TEMP_Active = RESET;
        Alarm_TEMP = RESET;
        MOD_TEMP_DisableAlarm(&temp_Handle);
        HAL_NVIC_DisableIRQ(EXTI2_3_IRQn);
    }
    


    MOD_LCD_SetCursor(&lcd_display,2,1);
    if (HAL_GetTick() - tick >= TIME_TRANSITION)
    {
        tick = HAL_GetTick();
        HAL_RTC_GetTime(&RTC_InitStructure,&gTime,RTC_FORMAT_BIN);
        HAL_RTC_GetDate(&RTC_InitStructure,&gDate,RTC_FORMAT_BIN);
        if (time%2)
        {
            sprint_TimeAlarm((char*)buffer,gTime,1);
        }
        else
        {
            sprint_TimeAlarm((char*)buffer,gTime,0);
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
        if (__HAL_RTC_ALARM_GET_IT_SOURCE(&RTC_InitStructure,RTC_ALARM_A) || Alarm_TEMP == SET)
        {
            sprint_Alarm((char*)buffer,gAlarm);
            MOD_LCD_String(&lcd_display,(char*)buffer);
        }
        else
        {
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
    else if (SerialSet_Data.msg == TEMP)
    {
        setTemp(SerialSet_Data.param1,SerialSet_Data.param2);
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

void i2c_init(void)
{
    i2c_Handle.Instance = I2C1;
    i2c_Handle.Init.Timing = 0x10815E89;
    i2c_Handle.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    i2c_Handle.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    i2c_Handle.Init.OwnAddress2 = 0;
    i2c_Handle.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    i2c_Handle.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    i2c_Handle.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&i2c_Handle);

}


void MOD_TEMP_MspInit( TEMP_HandleTypeDef *htemp )
{
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStructure.Pin          = GPIO_PIN_ALERT;
    GPIO_InitStructure.Mode         = GPIO_MODE_IT_FALLING;
    GPIO_InitStructure.Pull         = GPIO_NOPULL;
    GPIO_InitStructure.Speed        = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIO_PORT_ALERT,&GPIO_InitStructure);

    htemp->AlertPort = GPIO_PORT_ALERT;
    htemp->AlertPin = GPIO_PIN_ALERT;
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

void sprint_TimeAlarm(char* buffer,RTC_TimeTypeDef TimeData, uint8_t stars)
{
    char bufferTemp[17] = {0};
    char buffernum[3] = {0};
    uint16_t            temperature = 0;

    temperature = MOD_TEMP_Read(&temp_Handle);

    if (stars == 0)
    {
        strcat(bufferTemp,"    ");
    }
    else
    {
        strcat(bufferTemp,"*** ");
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
    
    //Temperatura
    strcat(bufferTemp," ");
    strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,temperature);
    if (strlen(buffernum) == 1)
    {
        strcpy(&bufferTemp[11],buffernum);
    }
    else
    {
        strcpy(&bufferTemp[10],buffernum);
    }
    strcat(bufferTemp,"C");

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

void sprint_Time(char* buffer,RTC_TimeTypeDef TimeData)
{
    char bufferTemp[17] = {0};
    char buffernum[3] = {0};
    uint16_t temperature = 0;
    temperature = MOD_TEMP_Read(&temp_Handle);

    strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,TimeData.Hours);
    if (strlen(buffernum) == 1)
    {
        strcpy(&bufferTemp[1],buffernum);
    }
    else
    {
        strcpy(&bufferTemp[0],buffernum);
    }
    CLEAR_BUFFER(buffernum);
    strcat(bufferTemp,":");
    strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,TimeData.Minutes);
    if (strlen(buffernum) == 1)
    {
        strcpy(&bufferTemp[4],buffernum);
    }
    else
    {
        strcpy(&bufferTemp[3],buffernum);
    }
    CLEAR_BUFFER(buffernum);
    strcat(bufferTemp,":");
    strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,TimeData.Seconds);
    if (strlen(buffernum) == 1)
    {
        strcpy(&bufferTemp[7],buffernum);
    }
    else
    {
        strcpy(&bufferTemp[6],buffernum);
    }
    strcat(bufferTemp," ");
    CLEAR_BUFFER(buffernum);
    DecToStr((uint8_t*)buffernum,temperature);
    strcat(bufferTemp, "00");
    if (strlen(buffernum) == 1)
    {
        strcpy(&bufferTemp[10],buffernum);
    }
    else
    {
        strcpy(&bufferTemp[9],buffernum);
    }
    strcat(bufferTemp, "C");
    strcat(bufferTemp, "    ");
    strcpy(buffer,bufferTemp);
}

void sprint_Alarm(char* buffer, RTC_AlarmTypeDef AlarmData)
{
    char           bufferTemp[17]  = {0};
    char             buffernum[3]  = {0};
    uint8_t         tempBuffer[2]  = {0};
    uint8_t             lowerTemp  = 0;
    uint8_t             upperTemp  = 0;

    MOD_TEMP_ReadRegister(&temp_Handle,tempBuffer,ALERT_TEMP_LOWER_B_TRIP_REGISTER);
    lowerTemp = (tempBuffer[0] << 4) | (tempBuffer[1]>>4);
    MOD_TEMP_ReadRegister(&temp_Handle,tempBuffer,ALERT_TEMP_UPPER_B_TRIP_REGISTER);
    upperTemp = (tempBuffer[0] << 4) | (tempBuffer[1]>>4);

    strcat(bufferTemp," A ");
    strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,AlarmData.AlarmTime.Hours);
    if (strlen(buffernum) == 1)
    {
        strcpy(&bufferTemp[4],buffernum);
    }
    else
    {
        strcpy(&bufferTemp[3],buffernum);
    }
    CLEAR_BUFFER(buffernum);
    strcat(bufferTemp,":");
    strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,AlarmData.AlarmTime.Minutes);
    if (strlen(buffernum) == 1)
    {
        strcpy(&bufferTemp[7],buffernum);
    }
    else
    {
        strcpy(&bufferTemp[6],buffernum);
    }
    CLEAR_BUFFER(buffernum);
    strcat(bufferTemp," ");

    //Temperatura:
    strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,lowerTemp);
    if (strlen(buffernum) == 1)
    {
        strcpy(&bufferTemp[10],buffernum);
    }
    else
    {
        strcpy(&bufferTemp[9],buffernum);
    }
    CLEAR_BUFFER(buffernum);
    strcat(bufferTemp, "-");
    strcat(bufferTemp, "00");
    DecToStr((uint8_t*)buffernum,upperTemp);
    if (strlen(buffernum) == 1)
    {
        strcpy(&bufferTemp[13],buffernum);
    }
    else
    {
        strcpy(&bufferTemp[12],buffernum);
    }
    strcat(bufferTemp, "C");
    strcat(bufferTemp, " ");
    strcpy(buffer,bufferTemp);
}

void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *hrtc)
{
    AlarmRTC = SET;
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    Alarm_TEMP_Active = SET;
}