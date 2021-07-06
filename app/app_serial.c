#include "app_serial.h"

#define SERIAL_IDLE     0U
#define SERIAL_AT       1U
#define SERIAL_TIME     2U
#define SERIAL_DATE     3U
#define SERIAL_ALARM    4U
#define SERIAL_ERROR    5U
#define SERIAL_OK       6U

void serialdle(void);
void serialAT_Sel(void);
void serialTime(void);
void serialDate(void);
void serialAlarm(void);
void serialOK(void);
void serialERROR(void);

int32_t validate_StrToInt(char * buffer);
HAL_StatusTypeDef checkDataTime(uint8_t hour, uint8_t minutes, uint16_t seconds);
HAL_StatusTypeDef checkDataDate(uint8_t day, uint8_t month, uint16_t year);
HAL_StatusTypeDef checkDataAlarm(uint8_t hour, uint8_t minutes);

typedef void (*serialSelection)(void);

const char* msgOK           = {"OK\r\n"};
const char* msgError        = {"ERROR\r\n"};
const char *comando_AT[]    = {"AT+TIME" , "AT+DATE" , "AT+ALARM"};

UART_HandleTypeDef UartHandle           = {0};

static uint8_t RxByte;
static uint8_t BufferTemp[30];

static uint8_t SerialRx_BufferQ[100];
QUEUE_HandleTypeDef QueueSerialRx;

uint8_t Serial_MSG_BufferQ[100];
QUEUE_HandleTypeDef QueueSerialTx;

static uint32_t serialTimeTick;

static serialSelection SerialStateFun[] = {serialdle,serialAT_Sel,serialTime,serialDate,serialAlarm,serialERROR,serialOK};

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
    QueueSerialRx.Elements = 100U;
    QueueSerialRx.Size = sizeof(uint8_t);
    HIL_QUEUE_Init(&QueueSerialRx);

    QueueSerialTx.Buffer = (void*) Serial_MSG_BufferQ;
    QueueSerialTx.Elements = 100U;
    QueueSerialTx.Size = sizeof(Serial_MsgTypeDef);
    HIL_QUEUE_Init(&QueueSerialTx);

    serialTimeTick = HAL_GetTick();
}

void serial_Task(void)
{
    SerialStateFun[serialState]();
}

void serialdle(void)
{
    uint8_t data = 0;
    uint8_t index = 0;
    if ((HAL_GetTick() - serialTimeTick) >= 5000)
    {
        serialTimeTick = HAL_GetTick();
        while (HIL_QUEUE_IsEmpty(&QueueSerialRx) == 0)
        {
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
        HIL_QUEUE_Write(&QueueSerialTx,&SerialTranferData);
        serialState = SERIAL_OK;
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
        HIL_QUEUE_Write(&QueueSerialTx,&SerialTranferData);
        serialState = SERIAL_OK;
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
        HIL_QUEUE_Write(&QueueSerialTx,&SerialTranferData);
        serialState = SERIAL_OK;
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
    if ((day <= 30) && (month <= 12) && (year <= 9999) && (month >= 1) && (day >= 1))
    {
        flag = HAL_OK;
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

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    uartState = SET;
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // static uint32_t i = 0;
    // RxBuffer[i] = RxByte;
    // i++;
    // if(RxBuffer[i-1] == '\r')
    // {
    //     RxBuffer[i-1] = '\0';
    //     statusRx = SET;
    //     i=0;
    // }
    // statusRx = SET;
    // HIL_BUFFER_Write(&CircBuffer,RxByte);
    HIL_QUEUE_Write(&QueueSerialRx,(void*)&RxByte);
    HAL_UART_Receive_IT(&UartHandle,&RxByte,1);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    uartError = SET;
    statusRx = RESET;
}