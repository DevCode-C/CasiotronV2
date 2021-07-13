#include "app_serial.h"

#define SERIAL_IDLE     0U
#define SERIAL_AT       1U
#define SERIAL_TIME     2U
#define SERIAL_DATE     3U
#define SERIAL_ALARM    4U
#define SERIAL_ERROR    5U
#define SERIAL_OK       6U

/**
 * @brief Verifica el estado de la banderas y selecciona el estado correspondiente
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void serialdle(void);

/**
 * @brief Verifica el tipo de mensaje encontrado
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void serialAT_Sel(void);

/**
 * @brief Segmenta los parametros necesario para el tipo TIME y los verifica
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void serialTime(void);

/**
 * @brief Segmenta los parametros necesario para el tipo DATE y los verifica
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void serialDate(void);

/**
 * @brief Segmenta los parametros necesario para el tipo ALARM y los verifica
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void serialAlarm(void);

/**
 * @brief Manda por UART el mensaje "OK"
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void serialOK(void);

/**
 * @brief Manda por UART el mensaje "ERROR"
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void serialERROR(void);

/**
 * @brief Valida, convierte y retorna el string a int32
 * 
 * @param char * buffer
 * 
 * @return int32_t, -1 si el string es invalido
*/
int32_t validate_StrToInt(char * buffer);

/**
 * @brief Verifica los parametros relacionado al TIME
 * 
 * @param uint8_t hour, Valor decimal de la hora
 * 
 * @param uint8_t minutes, Valor decima de los minutos
 * 
 * @param uint16_t seconds, Valor decimal de los segundos
 * 
 * @return HAL_StatusTypeDef, HAL_OK si los parametros son correctos
*/
HAL_StatusTypeDef checkDataTime(uint8_t hour, uint8_t minutes, uint16_t seconds);

/**
 * @brief Verifica los parametros relacionado al DATE
 * 
 * @param uint8_t dia, Valor decimal del dia
 * 
 * @param uint8_t mes, Valor decima del mes
 * 
 * @param uint16_t año, Valor decimal del año
 * 
 * @return HAL_StatusTypeDef, HAL_OK si los parametros son correctos
*/
HAL_StatusTypeDef checkDataDate(uint8_t day, uint8_t month, uint16_t year);

/**
 * @brief Verifica los parametros relacionado al ALARM
 * 
 * @param uint8_t hour, Valor decimal de la hora
 * 
 * @param uint8_t minutes, Valor decima de los minutos
 * 
 * @return HAL_StatusTypeDef, HAL_OK si los parametros son correctos
*/
HAL_StatusTypeDef checkDataAlarm(uint8_t hour, uint8_t minutes);

typedef void (*serialSelection)(void);

const char* msgOK           = {"OK\r\n"};
const char* msgError        = {"ERROR\r\n"};
const char *comando_AT[]    = {"AT+TIME" , "AT+DATE" , "AT+ALARM"};

UART_HandleTypeDef UartHandle           = {0};
Serial_MsgTypeDef SerialTranferData     = {0};

static uint8_t RxByte;
static uint8_t RxBuffer[30];
static uint8_t BufferTemp[30];

uint8_t bufferCircular[116];
BUFFER_HandleTypeDef CircBuffer;
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

    SerialTranferData.msg = NONE;
    uartState = SET;

    CircBuffer.Buffer = bufferCircular;
    CircBuffer.Elements = 116U;
    HIL_BUFFER_Init(&CircBuffer);
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
    if ((HAL_GetTick() - serialTimeTick) > 10)
    {
        serialTimeTick = HAL_GetTick();
        while (HIL_BUFFER_IsEmpty(&CircBuffer) == 0)
        {
            HAL_NVIC_DisableIRQ(USART2_IRQn);
            data = HIL_BUFFER_Read(&CircBuffer);
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

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
{
    uartState = SET;
    memset(RxBuffer,0,sizeof(RxBuffer));
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    HIL_BUFFER_Write(&CircBuffer,RxByte);
    HAL_UART_Receive_IT(&UartHandle,&RxByte,1);
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    memset(RxBuffer,0,sizeof(RxBuffer));
    uartError = SET;
    statusRx = RESET;
}