#include "app_serial.h"

const char* msgOK           = {"OK\r\n"};
const char* msgError        = {"ERROR\r\n"};
const char *comando_AT[]    = {"AT+TIME" , "AT+DATE" , "AT+ALARM"};

UART_HandleTypeDef UartHandle           = {0};
Serial_MsgTypeDef SerialTranferData     = {0};

uint8_t RxByte;
uint8_t RxBuffer[30];

__IO ITStatus uartState             = SET;
__IO ITStatus uartError             = RESET;
__IO ITStatus statusRx              = RESET;
__IO static uint8_t serialState     = SERIAL_IDLE; 
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
}

void serial_Task(void)
{
    uint8_t BufferTemp[30]  = {0};
    uint8_t hour_day        = 0;
    uint8_t min_month       = 0;
    uint16_t sec_year       = 0;
    char *InpuyComand, *parametro;

    switch (serialState)
    {
        case SERIAL_IDLE:
            if (statusRx)
            {
                statusRx = RESET;
                memcpy((char*)BufferTemp,(const char*)RxBuffer,strlen((const char*)RxBuffer));
                serialState = SERIAL_AT;
            }
            if(uartError)
            {
                uartError = RESET;   
                serialState = SERIAL_ERROR;
            }
            break;
        case SERIAL_AT:
            InpuyComand = strtok((char*)RxBuffer, "=" );
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
            break;
        
        case SERIAL_TIME:
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
            break;
        
        case SERIAL_DATE:
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
            break;

        case SERIAL_ALARM:
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
            
            break;
        
        case SERIAL_ERROR:
            if (uartState == SET)
            {
                HAL_UART_Transmit_IT(&UartHandle,(uint8_t*)msgError,strlen(msgError));
            }
            serialState = SERIAL_IDLE;
            break;

        case SERIAL_OK:
            if (uartState == SET)
            {
                HAL_UART_Transmit_IT(&UartHandle,(uint8_t*)msgOK,strlen(msgOK));   
            }
            serialState = SERIAL_IDLE;
            break;

        default:
            break;
    }
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
    if ((day <= 30) && (month <= 12) && (year <= 9999))
    {
        flag = HAL_OK;
    }
    return flag;
}

HAL_StatusTypeDef checkDataAlarm(uint8_t hour, uint8_t minutes)
{
    HAL_StatusTypeDef flag = HAL_ERROR;
    if (hour < 24 && minutes < 59)
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
    static uint32_t i = 0;
    RxBuffer[i] = RxByte;
    i++;
    if(RxBuffer[i-1] == '\r')
    {
        RxBuffer[i-1] = '\0';
        statusRx = SET;
        i=0;
    }
    HAL_UART_Receive_IT(&UartHandle,&RxByte,1);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    memset(RxBuffer,0,sizeof(RxBuffer));
    uartError = SET;
    statusRx = RESET;
}