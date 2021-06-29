#include "app_serial.h"

const char* msgOK           = {"OK\r\n"};
const char* msgError        = {"ERROR\r\n"};
const char *comando_AT[]    = {"AT+TIME" , "AT+DATE" , "AT+ALARM"};

UART_HandleTypeDef UartHandle           = {0};
Serial_MsgTypeDef SerialTranferData     = {0};

static uint8_t RxByte;
static uint8_t RxBuffer[30];
static uint8_t BufferTemp[30];

uint8_t bufferCircular[200];
BUFFER_HandleTypeDef CircBuffer;

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

    SerialTranferData.msg = NONE;
    uartState = SET;

    CircBuffer.Buffer = bufferCircular;
    CircBuffer.Elements = 200U;
    HIL_BUFFER_Init(&CircBuffer);
}

void serial_Task(void)
{
    SerialStateFun[serialState]();
}

void serialdle(void)
{
    if (statusRx)
    {
        statusRx = RESET;

        while (HIL_BUFFER_IsEmpty(&CircBuffer) == 0)
        {
            HAL_NVIC_DisableIRQ(USART2_IRQn);
            printf("%c\n",HIL_BUFFER_Read(&CircBuffer));
            HAL_NVIC_EnableIRQ(USART2_IRQn);
        }
        
        
        // memcpy((char*)BufferTemp,(const char*)RxBuffer,strlen((const char*)RxBuffer));
        // serialState = SERIAL_AT;
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
    uint8_t hour_day        = 0;
    uint8_t min_month       = 0;
    uint16_t sec_year       = 0;
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
        serialState = SERIAL_OK;
    }
}

void serialDate(void)
{
    uint8_t hour_day        = 0;
    uint8_t min_month       = 0;
    uint16_t sec_year       = 0;
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
        serialState = SERIAL_OK;
    }
}
void serialAlarm(void)
{
    uint8_t hour_day        = 0;
    uint8_t min_month       = 0;
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
    memset(RxBuffer,0,sizeof(RxBuffer));
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    // static uint32_t i = 0;
    // RxBuffer[i] = RxByte;
    // i++;
    // if(RxBuffer[i-1] == '\r')
    // {
    //     RxBuffer[i-1] = '\0';
    //     i=0;
    // }
    HIL_BUFFER_Write(&CircBuffer,RxByte);
    statusRx = SET;
    HAL_UART_Receive_IT(&UartHandle,&RxByte,1);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    memset(RxBuffer,0,sizeof(RxBuffer));
    uartError = SET;
    statusRx = RESET;
}