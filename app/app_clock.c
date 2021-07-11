#include "app_clock.h"
#include "lcd.h"

#define CLOCK_IDLE          0U
#define CLOCK_SHOW          1U
#define CLOCK_SHOW_ALARM    2U
#define CLOCK_SET_DATA      3U 
#define CLOCK_ALARM_UP      4U 

#define TIME_TRANSITION     1000U

/**---------------------------------------------------------------
Brief.- Funcion que verifica el estado de la banderas y selecciona el estado correspondiente
Param.- NONE (VOID)
Return.- NONE (VOID)
----------------------------------------------------------------*/
void clockIdle(void);

/**---------------------------------------------------------------
Brief.- Muestra por la lcd el tiempo actual del RTC
Param.- NONE (VOID)
Return.- NONE (VOID)
----------------------------------------------------------------*/
void showClock(void);

/**---------------------------------------------------------------
Brief.- Muestra por la lcd la alarma cuando se activo la bandera "ALARMRTC"
Param.- NONE (VOID)
Return.- NONE (VOID)
----------------------------------------------------------------*/
void showAlarmUp(void);

/**---------------------------------------------------------------
Brief.- Verifica el tipo de de dato y manda al estado correpondiente para asignar la informacion
Param.- NONE (VOID)
Return.- NONE (VOID)
----------------------------------------------------------------*/
void clockSetData(void);

/**---------------------------------------------------------------
Brief.- Muestra por la lcd la hora de activacion de alarma o si no esta configurada
Param.- NONE (VOID)
Return.- NONE (VOID)
----------------------------------------------------------------*/
void clockShowAlarm(void);

/**---------------------------------------------------------------
Brief.- Actualiza la informacion del RTC Time
Param.- uint8_t hour, Valor decimal de la nueva hora
Param.- uint8_t minutes, Valor decima de los nuevos minutos
Param.- uint16_t seconds, Valor decimal de los segundos
Return.- NONE (VOID)
----------------------------------------------------------------*/
void setTime(uint8_t hour, uint8_t minutes, uint16_t seconds);

/**---------------------------------------------------------------
Brief.- Actualiza la informacion del RTC Date
Param.- uint8_t dia, Valor decimal del dia
Param.- uint8_t mes, Valor decima del mes
Param.- uint16_t año, Valor decimal del año
Return.- NONE (VOID)
----------------------------------------------------------------*/
void setDate(uint8_t day, uint8_t month, uint16_t year);

/**---------------------------------------------------------------
Brief.- Actualiza la informacion del RTC Alarm
Param.- uint8_t hour, Valor decimal de la nueva hora
Param.- uint8_t minutes, Valor decima de los nuevos minutos
Return.- NONE (VOID)
----------------------------------------------------------------*/
void setAlarm(uint8_t hour, uint8_t minutes);

/**---------------------------------------------------------------
Brief.- Convierte valores deciamles a caracteres para alacenarlos en el buffer
Param.- uint8_t *buffer, buffer donde se almacena la informacion 
Param.- int32_t val, valor decimal para convertir 
Return.- NONE (VOID)
----------------------------------------------------------------*/
void DecToStr(uint8_t *buffer, int32_t val);

/**---------------------------------------------------------------
Brief.- Verifica la cantida de digitos en un valor decimal 
Param.- int32_t num, Valor decimal a contar
Return.- uint8_t, el numero de digitos contados
----------------------------------------------------------------*/
uint8_t number_digits(int32_t num);

/**---------------------------------------------------------------
Brief.- Realiza la configuracion de la informacion de DATE para mostrar por LCD
Param.- char* buffer, Buffer donde se gurada la informacion
Param.- RTC_DateTypeDef DateData, Estrucutra que contiene los datos que se mostraran 
Return.- NONE (VOID)
----------------------------------------------------------------*/
void sprint_Date(char* buffer, RTC_DateTypeDef DateData);

/**---------------------------------------------------------------
Brief.- Realiza la configuracion de la informacion de TIME para mostrar por LCD
Param.- char* buffer, Buffer donde se gurada la informacion
Param.- RTC_TimeTypeDef TimeData, Estrucutra que contiene los datos que se mostraran 
Param.- uint8_t stars, Bandera que selecciona el mostrar "*" o " "
Return.- NONE (VOID)
----------------------------------------------------------------*/
void sprint_Time(char* buffer,RTC_TimeTypeDef TimeData, uint8_t stars);

/**---------------------------------------------------------------
Brief.- Realiza la configuracion de la informacion de ALARM para mostrar por LCD
Param.- char* buffer, Buffer donde se gurada la informacion
Param.- RTC_AlarmTypeDef AlarmData, Estrucutra que contiene los datos que se mostraran 
Return.- NONE (VOID)
----------------------------------------------------------------*/
void sprint_Alarm(char* buffer, RTC_AlarmTypeDef AlarmData);

/**---------------------------------------------------------------
Brief.- Inicializacion del modulo LCD
Param.- NONE (VOID)
Return.- NONE (VOID)
----------------------------------------------------------------*/
void lcd_init(void);

/**---------------------------------------------------------------
Brief.- Inicializacion del modulo SPI
Param.- NONE (VOID)
Return.- NONE (VOID)
----------------------------------------------------------------*/
void spi_init(void);

/**---------------------------------------------------------------
Brief.- Algoritmo de congruencia de Zeller,
Param.- uint8_t d, numero del dia de la semana
Param.- uint8_t m, numero del mes 
Param.- uint16_t y, año
Return.- uint8_t El numero del dia de la semana acorde al d,m,y
----------------------------------------------------------------*/
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

extern Serial_MsgTypeDef    SerialTranferData;
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
    if(SerialTranferData.msg != NONE)
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

    sprint_Date((char*)buffer,gDate);
    // MOD_LCD_SetCursor(&lcd_display,1,1);
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