#include "app_serial.h"
#include "buffer.h"
#include "queue.h"

#define SERIAL_IDLE     0U
#define SERIAL_AT       1U
#define SERIAL_TIME     2U
#define SERIAL_DATE     3U
#define SERIAL_ALARM    4U
#define SERIAL_ERROR    5U
#define SERIAL_OK       6U
#define SERIAL_HEART    7U
#define SERIAL_TEMP     8U

#define BUFFER_COMMAD_SIZE          30U
#define BUFFER_SERIAL_INPUT_SIZE    116U
#define BUFFER_TRANSFERT_SIZE       160U 
#define BUFFER_BLINK_SIZE           40U

/**
 * @brief Verify the flags state and select the corresponding state 
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void serialdle(void);

/**
 * @brief Verify the found msg type
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void serialAT_Sel(void);

/**
 * @brief Segmenting and check all the TIME related parameters
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void serialTime(void);

/**
 * @brief Segmenting and check all the DATE related parameters
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void serialDate(void);

/**
 * @brief Segmenting and check all the ALARM related parameters
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void serialAlarm(void);

void serialTemp(void);

/**
 * @brief Segmenting and check all the HEARTBEAT related parameters
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void serialHeart(void);

/**
 * @brief Send by UART the msg "OK"
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void serialOK(void);

/**
 * @brief Send by UART the msg "ERROR"
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void serialERROR(void);

/**
 * @brief Enter a string for validate, convert and return a int32
 * 
 * @param char * buffer
 * 
 * @return int32_t, -1 if char* buffer is invalidate
*/
int32_t validate_StrToInt(char * buffer);

/**
 * @brief Verifying related parameters of TIME
 * 
 * @param uint8_t hour, Decimal value of hour (i.e 0U - 23U)
 * 
 * @param uint8_t minutes, Decimal value of minutes (i.e 0U - 59U)
 * 
 * @param uint16_t seconds, Decimal value of seconds (i.e 0U - 59U)
 * 
 * @return HAL_StatusTypeDef, HAL_OK If all parameters are correct
*/
HAL_StatusTypeDef checkDataTime(uint8_t hour, uint8_t minutes, uint16_t seconds);

/**
 * @brief Verifying related parameters of DATE
 * 
 * @param uint8_t day, Decimal value of day (i.e 1U - 31U)
 * 
 * @param uint8_t month, Decimal value of month (i.e 1U - 12U)
 * 
 * @param uint16_t year, Decimal value of year (i.e 0U - 9999U)
 * 
 * @return HAL_StatusTypeDef, HAL_OK If all parameters are correct
*/
HAL_StatusTypeDef checkDataDate(uint8_t day, uint8_t month, uint16_t year);

/**
 * @brief Verifying related parameters of ALARM
 * 
 * @param uint8_t hour, Decimal value of hour (i.e 0U - 23U)
 * 
 * @param uint8_t minutes, Decimal value of minutes (i.e 0U - 59U)
 * 
 * @return HAL_StatusTypeDef, HAL_OK If all parameters are correct
*/
HAL_StatusTypeDef checkDataAlarm(uint8_t hour, uint8_t minutes);

/**
 * @brief
 * 
 * @param
 * 
 * @param
 * 
 * @return HAL_StatusTypeDef, HAL_OK If all parameters are correct
*/
HAL_StatusTypeDef checkDataTemp(int8_t lower, uint8_t uper);

/**
 * @brief Verifying related parameters of BLINK TIME
 * 
 * @param uint16_t time, Decimal value of time (i.e 50U - 1000U)
 * 
 * @return HAL_StatusTypeDef, HAL_OK If all parameters are correct
*/
HAL_StatusTypeDef checkDataBlinkTime(uint16_t time);

typedef void (*serialSelection)(void);

const char* msgOK           = {"OK\r\n"};
const char* msgError        = {"ERROR\r\n"};
const char *comando_AT[]    = {"AT+TIME" , "AT+DATE" , "AT+ALARM" , "AT+HEARTBEAT", "AT+TEMP"};

UART_HandleTypeDef UartHandle           = {0};

static uint8_t RxByte;
static uint8_t BufferTemp[BUFFER_COMMAD_SIZE];

static uint8_t SerialRx_BufferQ[BUFFER_SERIAL_INPUT_SIZE];
QUEUE_HandleTypeDef QueueSerialRx;

Serial_MsgTypeDef Serial_MSG_BufferQ[BUFFER_TRANSFERT_SIZE];
QUEUE_HandleTypeDef QueueSerialTx;

uint16_t blinkTime[BUFFER_BLINK_SIZE];
QUEUE_HandleTypeDef QueueSerialBlink;

static serialSelection SerialStateFun[] = {serialdle,serialAT_Sel,serialTime,serialDate,serialAlarm,serialERROR,serialOK,serialHeart,serialTemp};

__IO static ITStatus uartState             = SET;
__IO static ITStatus uartError             = RESET;
__IO static ITStatus statusRx              = RESET;
__IO static uint8_t serialState            = SERIAL_IDLE; 
void serial_init()
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

    uartState = SET;

    QueueSerialRx.Buffer = (void*) SerialRx_BufferQ;
    QueueSerialRx.Elements = BUFFER_SERIAL_INPUT_SIZE;
    QueueSerialRx.Size = sizeof(uint8_t);
    HIL_QUEUE_Init(&QueueSerialRx);

    QueueSerialTx.Buffer = (void*) Serial_MSG_BufferQ;
    QueueSerialTx.Elements = BUFFER_TRANSFERT_SIZE;
    QueueSerialTx.Size = sizeof(Serial_MsgTypeDef);
    HIL_QUEUE_Init(&QueueSerialTx);

    QueueSerialBlink.Buffer = (void*) blinkTime;
    QueueSerialBlink.Elements = BUFFER_BLINK_SIZE;    
    QueueSerialBlink.Size = sizeof(uint16_t);
    HIL_QUEUE_Init(&QueueSerialBlink);

}

void serial_Task(void)
{
    SerialStateFun[serialState]();
}

void serialdle(void)
{
    uint8_t data = 0;
    uint8_t index = 0;
    
    while (HIL_QUEUE_IsEmpty(&QueueSerialRx) == ELEMENTS_IN_BUFFER)
    {
        //Agregar modificacion para desahabilitar todas las interrupciones
        HAL_NVIC_DisableIRQ(USART2_IRQn);
        HIL_QUEUE_Read(&QueueSerialRx,&data);
        HAL_NVIC_EnableIRQ(USART2_IRQn);

        if (data == '\r')
        {
            serialState = SERIAL_AT;
            break;
        }
        else
        {
            BufferTemp[index] = data;
            index++;
        }
    }    
    if(uartError)
    {
        uartError = RESET;   
        serialState = SERIAL_ERROR;
    }
}

void serialAT_Sel(void)
{
    char* InpuyComand = strtok((char*)BufferTemp, "=" );
    if (strcmp(InpuyComand,comando_AT[0]) == 0)
    {
        serialState = SERIAL_TIME;
    }
    else if (strcmp(InpuyComand,comando_AT[1]) == 0)
    {
        serialState = SERIAL_DATE;
    }
    else if (strcmp(InpuyComand,comando_AT[2]) == 0)
    {
        serialState = SERIAL_ALARM;
    }
    else if (strcmp(InpuyComand,comando_AT[3]) == 0)
    {
        serialState = SERIAL_HEART;
    }
    else if (strcmp(InpuyComand,comando_AT[4]) == 0)
    {
        serialState = SERIAL_TEMP;
    }
    else
    {
        serialState = SERIAL_ERROR;
    }
}

void serialTime(void)
{
    uint8_t             hour_day              = 0;
    uint8_t             min_month             = 0;
    uint16_t            sec_year              = 0;
    Serial_MsgTypeDef   SerialTranferData     = {NONE,0,0,0};
    char *parametro         = NULL;

    serialState = SERIAL_ERROR;
    parametro = strtok(NULL, "," );
    hour_day = validate_StrToInt(parametro);

    parametro = strtok(NULL, "," );
    min_month = validate_StrToInt(parametro);

    parametro = strtok(NULL, "," );
    sec_year  = validate_StrToInt(parametro);
    if (checkDataTime(hour_day,min_month,sec_year) == HAL_OK)
    {
        SerialTranferData.msg       = TIME;
        SerialTranferData.param1    = hour_day;
        SerialTranferData.param2    = min_month;
        SerialTranferData.param3    = sec_year;
        if (HIL_QUEUE_Write(&QueueSerialTx,&SerialTranferData) == WRITE_OK)
        {
            serialState = SERIAL_OK;    
        }
    }
}

void serialDate(void)
{
    uint8_t             hour_day              = 0;
    uint8_t             min_month             = 0;
    uint16_t            sec_year              = 0;
    Serial_MsgTypeDef   SerialTranferData     = {NONE,0,0,0};
    char *parametro         = NULL;

    serialState = SERIAL_ERROR;
    parametro = strtok(NULL, "," );
    hour_day = validate_StrToInt(parametro);

    parametro = strtok(NULL, "," );
    min_month = validate_StrToInt(parametro);

    parametro = strtok(NULL, "," );
    sec_year  = validate_StrToInt(parametro);
    if (checkDataDate(hour_day,min_month,sec_year) == HAL_OK)
    {
        SerialTranferData.msg       = DATE;
        SerialTranferData.param1    = hour_day;
        SerialTranferData.param2    = min_month;
        SerialTranferData.param3    = sec_year;
        if (HIL_QUEUE_Write(&QueueSerialTx,&SerialTranferData) == WRITE_OK)
        {
            serialState = SERIAL_OK;    
        }
    }
}
void serialAlarm(void)
{
    uint8_t             hour_day              = 0;
    uint8_t             min_month             = 0;
    Serial_MsgTypeDef   SerialTranferData     = {NONE,0,0,0};
    char *parametro         = NULL;

    serialState = SERIAL_ERROR;
    parametro = strtok(NULL, "," );
    hour_day = validate_StrToInt(parametro);

    parametro = strtok(NULL, "," );
    min_month = validate_StrToInt(parametro);
    if (checkDataAlarm(hour_day,min_month) == HAL_OK)
    {
        SerialTranferData.msg       = ALARM;
        SerialTranferData.param1    = hour_day;
        SerialTranferData.param2    = min_month;
        SerialTranferData.param3    = 0;
        if (HIL_QUEUE_Write(&QueueSerialTx,&SerialTranferData) == WRITE_OK)
        {
            serialState = SERIAL_OK;    
        }
    }
}

void serialTemp(void)
{
    int8_t              lowerTemp               = 0;
    uint8_t             uperTemp                = 0;
    Serial_MsgTypeDef   SerialTranferData       = {NONE,0,0,0};
    char *parametro         = NULL;
    serialState = SERIAL_ERROR;

    parametro = strtok(NULL, "," );
    lowerTemp = validate_StrToInt(parametro);
    
    parametro = strtok(NULL, "," );
    uperTemp = validate_StrToInt(parametro);

    if (checkDataTemp(lowerTemp,uperTemp) == HAL_OK)
    {
        SerialTranferData.msg = TEMP;
        SerialTranferData.param1 = lowerTemp;
        SerialTranferData.param2 = uperTemp;
        SerialTranferData.param3 = 0;
        if (HIL_QUEUE_Write(&QueueSerialTx,&SerialTranferData) == WRITE_OK)
        {
            serialState = SERIAL_OK;    
        }
    }
}

void serialHeart(void)
{
    uint16_t             blinkTime            = 0;
    char *parametro                           = NULL;
    serialState = SERIAL_ERROR;

    parametro = strtok(NULL, "\0" );
    blinkTime = validate_StrToInt(parametro);
    if (checkDataBlinkTime(blinkTime) == HAL_OK)
    {
        if (HIL_QUEUE_Write(&QueueSerialBlink,&blinkTime) == WRITE_OK)
        {
            serialState = SERIAL_OK;    
        }
    }
    
}

void serialOK(void)
{
    memset(BufferTemp,0,sizeof(BufferTemp));
    if (uartState == SET)
    {
        HAL_UART_Transmit_IT(&UartHandle,(uint8_t*)msgOK,strlen(msgOK));   
    }
    serialState = SERIAL_IDLE;
}

void serialERROR(void)
{
    memset(BufferTemp,0,sizeof(BufferTemp));
    if (uartState == SET)
    {
        HAL_UART_Transmit_IT(&UartHandle,(uint8_t*)msgError,strlen(msgError));
    }
    serialState = SERIAL_IDLE;
}

int32_t validate_StrToInt(char * buffer)
{
    int32_t temp = -1;
    uint8_t sizeStr = 0;

    if (buffer != NULL)
    {
        if (buffer[sizeStr] == '-')
        {
            sizeStr++;
        }
        
        while (isdigit((int8_t)buffer[sizeStr]))
        {
            sizeStr++;
        }

        if (sizeStr == strlen(buffer))
        {
            temp = atoi(buffer);
        }
    }
    return temp;
}

HAL_StatusTypeDef checkDataTime(uint8_t hour, uint8_t minutes, uint16_t seconds)
{
    HAL_StatusTypeDef flag = HAL_ERROR;
    if ((hour < 24) && (minutes < 60) && (seconds < 60))
    {
        flag = HAL_OK;    
    }
    return flag;    
}

HAL_StatusTypeDef checkDataDate(uint8_t day, uint8_t month, uint16_t year)
{
    HAL_StatusTypeDef flag = HAL_ERROR;
    if ((day <= 31) && (month <= 12) && (year <= 9999) && (month >= 1) && (day >= 1))
    {
        switch (month)
        {
            case 1:
            case 3:
            case 5:
            case 7:
            case 8:
            case 10:
            case 12:
                flag = HAL_ERROR;
                if (day <= 31)
                {
                    flag = HAL_OK;
                }
                break;
            case 4:
            case 6:
            case 9:
            case 11:
                flag = HAL_ERROR;
                if (day <= 30)
                {
                    flag = HAL_OK;
                }
                break;
            case 2:
                flag = HAL_ERROR;
                if ((year % 4 == 0) && (year % 100 != 0))
                {
                    if (day <= 29)
                    {
                        flag = HAL_OK;
                    }
                }
                if (day<= 28)
                {
                    flag = HAL_OK;
                }                
        }
        if (flag == HAL_OK)
        {
            flag = HAL_OK;
        }
    }
    return flag;
}

HAL_StatusTypeDef checkDataAlarm(uint8_t hour, uint8_t minutes)
{
    HAL_StatusTypeDef flag = HAL_ERROR;
    if (hour < 24 && minutes < 60)
    {
        flag = HAL_OK;
    }
    return flag;
}
HAL_StatusTypeDef checkDataTemp(int8_t lower, uint8_t uper)
{
    HAL_StatusTypeDef flag = HAL_ERROR;
    if (lower < uper)
    {
        flag = HAL_OK;
    }
    return flag;
}

HAL_StatusTypeDef checkDataBlinkTime(uint16_t time)
{
    HAL_StatusTypeDef flag = HAL_ERROR;
    if ((time >= 50U) && ( time % 50 == 0) && (time <= 1000U) )
    {
        flag = HAL_OK;
    }
    
    return flag;
}


void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    uartState = SET;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    HIL_QUEUE_Write(&QueueSerialRx,(void*)&RxByte);
    HAL_UART_Receive_IT(&UartHandle,&RxByte,1);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    uartError = SET;
    statusRx = RESET;
}