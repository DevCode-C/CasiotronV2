#include "app_bsp.h"

/*Private defines ----------------------------------------------*/
//States 
#define MEMORY_IDLE         0U
#define MEMORY_CHECK_MSG    1U
#define MEMORY_CHECK        2U
#define MEMORY_WRITE        3U
#define MEMORY_READ         4U
#define MEMOMRY_TIME_LOG_M  5U 
#define MEMORY_WRITE_OK     6U
#define MEMORY_WIRTE_ERROR  7U

#define MEMORY_ALARM_UP     

#define READ    3U
#define WRITE   2U
#define WRDI    4U
#define WREN    6U
#define RDSR    5U
#define WRSR    1U

#define MEMORY_LOG_DATA_SIZE    10U
#define PAGE_SIZE_EEPROM        32U
#define TOTAL_PAGE_EEPROM       128U
#define TOTAL_ADDR_EEPROM       4095UL

#define TIM16_COUNTER_CLK        10000UL
#define TIM16_PERIOD             9999UL
/*--------------------------------------------------------------*/

/*Private Macros------------------------------------------------*/
#define CHECK_SIZE_DIR(addr)  (32UL - ((addr) % 32UL))
/*--------------------------------------------------------------*/

/*Private prototype --------------------------------------------*/
void memory_Idle(void);
void memory_check_Msg(void);
void memory_check(void);
void memory_write(void);
void memory_read(void);
void memory_time_log_modify(void);

void memory_writeOK(void);
void memory_writeERROR(void);

uint8_t read_byte(uint16_t addr);
void write_data(uint16_t addr, uint8_t *data, uint8_t size);
uint8_t read_Status_eeprom(void);
void memory_crateLog(uint8_t *bufferMemory);
void memory_TimerInit(void);
/*--------------------------------------------------------------*/

/*Private Typedef ----------------------------------------------*/
typedef void (*memorySeletionState)(void);
/*--------------------------------------------------------------*/

/*Private variables --------------------------------------------*/
static memorySeletionState memoryState[] = {memory_Idle, memory_check_Msg, memory_check, memory_write,\
memory_read, memory_time_log_modify, memory_writeOK, memory_writeERROR};
__IO static uint8_t memoryState_Sel = MEMORY_IDLE; 

static Memory_MsgTypeDef memory_transferDataQ[MEMORY_LOG_DATA_SIZE];
QUEUE_HandleTypeDef QueueMemoryData;

uint8_t memory_dump_Flag = 0;
uint8_t memory_write_Flag = 0;

uint8_t waiting_Data = USER_RESET;
uint8_t memory_alarm_FLag = USER_RESET;
uint32_t memory_Counter = 0UL;
TIM_HandleTypeDef TimHandle_Memory;
static Serial_MsgTypeDef memoryRx_command;
static uint16_t Timing_Log = 10UL;
/*--------------------------------------------------------------*/

/*Private extern variables--------------------------------------*/
extern __IO ITStatus uartState;
/*--------------------------------------------------------------*/

/*Private const data----------------------------------------------------*/
const char* WriteOKData     = {"WRITE OK\r\n"};
const char* WriteERRORData  = {"WRITE ERROR\r\n"};

const char* DateRTC_memory      = {"D:"};
const char* TimeRTC_memory      = {"T:"};
const char* Temperature_memory  = {"TP:"};
/*--------------------------------------------------------------*/
void memory_Init(void)
{
    memory_MspInit();
    QueueMemoryData.Buffer = (void*)memory_transferDataQ;
    QueueMemoryData.Elements = MEMORY_LOG_DATA_SIZE;
    QueueMemoryData.Size = sizeof(Memory_MsgTypeDef);
    HIL_QUEUE_Init(&QueueMemoryData);

    HAL_GPIO_WritePin(EEPROM_PORT,CS_EEPROM,SET);
    memory_TimerInit();
}

void memory_Task(void)
{
    memoryState[memoryState_Sel]();
}

void memory_Idle(void)
{
    if (waiting_Data == USER_SET)
    {
        memoryState_Sel = MEMORY_CHECK;
    }
    

    if (HIL_QUEUE_Read(&QueueSerialMemoryRx,&memoryRx_command) == READ_OK)
    {
        memoryState_Sel = MEMORY_CHECK_MSG;
    }
}

void memory_check_Msg(void)
{
    if (memoryRx_command.msg == MEMORY_DUMP)
    {
        memoryState_Sel = MEMORY_READ;
    }
    else if (memoryRx_command.msg == MEMORY_TIME_LOG)
    {
        memoryState_Sel = MEMOMRY_TIME_LOG_M;
    }
    else
    {
        /* code */
    }
}

void memory_check(void)
{
    uint8_t status = 0;
    static uint8_t TryNumber = 0;

    status = read_Status_eeprom();

    if ((status & 1U) == 0U)
    {
        memoryState_Sel = MEMORY_WRITE;
    }
    else
    {
        TryNumber++;
        if (TryNumber == 2U)
        {
            TryNumber = 0U ;
            memoryState_Sel = MEMORY_WIRTE_ERROR;
        }
        
    }
}

void memory_write(void)
{
    waiting_Data = USER_RESET;
    
    static uint16_t memory_addr = 0;
    memory_alarm_FLag = USER_RESET;
    uint8_t memoryBufferLog[PAGE_SIZE_EEPROM] = {0};
    memory_crateLog(memoryBufferLog);
    uint8_t size_buffer = strlen((const char*)memoryBufferLog);
    static uint8_t eeprom_Write_InProgrees = 0U;
    // printf("%s\t %d\n",memoryBufferLog,strlen((const char*)memoryBufferLog));

    if (size_buffer < CHECK_SIZE_DIR(memory_addr))
    {
        if (eeprom_Write_InProgrees == 0U)
        {
            write_data(memory_addr,memoryBufferLog,size_buffer);
            memoryState_Sel = MEMORY_IDLE;
        }
        else
        {
            size_buffer = size_buffer - eeprom_Write_InProgrees;
            write_data(memory_addr,&memoryBufferLog[eeprom_Write_InProgrees],size_buffer);
            eeprom_Write_InProgrees = 0U;
            memoryState_Sel = MEMORY_IDLE;
        }
        
    }
    else
    {
        if ((size_buffer + memory_addr) > TOTAL_ADDR_EEPROM)
        {
            memory_addr = 0UL;
        }
        size_buffer = CHECK_SIZE_DIR(memory_addr);
        write_data(memory_addr,memoryBufferLog,size_buffer);
        memoryState_Sel = MEMORY_CHECK;
        eeprom_Write_InProgrees = size_buffer;
    }

    memory_addr += size_buffer;
    if (memory_addr >= TOTAL_ADDR_EEPROM)
    {
        memory_addr = 0UL;
    }
    memoryState_Sel = MEMORY_IDLE;
    
}

void memory_read(void)
{
    uint8_t buffer[PAGE_SIZE_EEPROM] = {0};
    static uint8_t pageNumber = 0U;
    static uint16_t dirMemory = 0UL;

    for (uint16_t i = dirMemory; i < (dirMemory + PAGE_SIZE_EEPROM); i++)
    {
        // printf("%s ",read_byte(i));
        buffer[(i % 32U)] = read_byte(i);
    }
    (void) printf("%d: %s\n",pageNumber,buffer);
    
    if (uartState == SET)
    {
        uartState = RESET;
        HAL_UART_Transmit_IT(memoryTaskHandle.UartHandleM,buffer,sizeof(buffer));
    }
    else
    {
        /* code */
    }
    
    dirMemory += PAGE_SIZE_EEPROM;
    pageNumber++;

    if (TOTAL_PAGE_EEPROM == pageNumber)
    {
        pageNumber = 0;
        dirMemory = 0;
        memoryState_Sel = MEMORY_IDLE;
    }
    else
    {
        /* code */
    }
    
}

void memory_time_log_modify(void)
{
    Timing_Log = memoryRx_command.param3;
    memoryState_Sel = MEMORY_IDLE;
}

void memory_writeOK(void)
{
    if (uartState == SET)
    {
        uartState = RESET;
        //The variable WriteOKData is const but it value no is nedded tho modify 
        /* cppcheck-suppress misra-c2012-11.8 */
        HAL_UART_Transmit_IT(memoryTaskHandle.UartHandleM,(uint8_t*)WriteOKData,strlen(WriteOKData));
    }
    else
    {
        /* code */
    }
    memoryState_Sel = MEMORY_IDLE;
}

void memory_writeERROR(void)
{
    if (uartState == SET)
    {
        uartState = RESET;
    //The variable WriteERRORData is const but it value no is nedded tho modify
        /* cppcheck-suppress misra-c2012-11.8 */ 
        HAL_UART_Transmit_IT(memoryTaskHandle.UartHandleM,(uint8_t*)WriteERRORData,strlen(WriteERRORData));
    }
    else
    {
        /* code */
    }
    memoryState_Sel = MEMORY_IDLE;
}

uint8_t read_byte(uint16_t addr)
{
    uint8_t Rx_b_SPI[4] = {0};
    uint8_t Rx__SPI = 0;
    HAL_GPIO_WritePin(memoryTaskHandle.Cs_MemoryPort,memoryTaskHandle.Cs_MemoryPin,RESET);
    Rx_b_SPI[0] = READ; 
    Rx_b_SPI[1] = (uint8_t)(addr>>8);
    Rx_b_SPI[2] = (uint8_t)(addr);
    HAL_SPI_Transmit(memoryTaskHandle.SpiHandler,Rx_b_SPI,3U,100UL);
    HAL_SPI_Receive(memoryTaskHandle.SpiHandler,&Rx__SPI,1U,100UL);
    HAL_GPIO_WritePin(memoryTaskHandle.Cs_MemoryPort,memoryTaskHandle.Cs_MemoryPin,SET);

    return Rx__SPI;
}

void write_data(uint16_t addr, uint8_t *data, uint8_t size)
{
    uint8_t Tx_b_SPI_Memory[4] = {0};

    HAL_GPIO_WritePin(EEPROM_PORT,CS_EEPROM,RESET);
    Tx_b_SPI_Memory[0] = WREN;
    HAL_SPI_Transmit(memoryTaskHandle.SpiHandler,Tx_b_SPI_Memory,1UL,100UL);
    HAL_GPIO_WritePin(EEPROM_PORT,CS_EEPROM,SET); 

    HAL_GPIO_WritePin(memoryTaskHandle.Cs_MemoryPort,memoryTaskHandle.Cs_MemoryPin,RESET);
    Tx_b_SPI_Memory[0] = WRITE;
    Tx_b_SPI_Memory[1] = (uint8_t)(addr>>8U);
    Tx_b_SPI_Memory[2] = (uint8_t)(addr);
    HAL_SPI_Transmit(memoryTaskHandle.SpiHandler,Tx_b_SPI_Memory,3UL,100UL);
    for (uint8_t i = 0; i < size; i++)
    {
        Tx_b_SPI_Memory[3] = data[i];
        HAL_SPI_Transmit(memoryTaskHandle.SpiHandler,&Tx_b_SPI_Memory[3],1UL,100UL);
    }
    HAL_GPIO_WritePin(EEPROM_PORT,CS_EEPROM,SET);
    
}

void memory_crateLog(uint8_t *bufferMemory)
{
    uint8_t memoryBufferLog[PAGE_SIZE_EEPROM] = {0};
    uint8_t memoryTempDecToStr[3] = {0};

    Memory_MsgTypeDef getData = {0};
    
    if (HIL_QUEUE_Read(&QueueMemoryData,(void*)&getData) == READ_OK)
    {
        /*Date --------------------------------------------------------------------*/
        (void) strcat((char*)memoryBufferLog,DateRTC_memory);
        DecToStr(memoryTempDecToStr,getData.data[1].param1);
        (void) strcat((char*)memoryBufferLog,(const char*)memoryTempDecToStr);
        (void) strcat((char*)memoryBufferLog,",");

        (void) CLEAR_BUFFER(memoryTempDecToStr);
        DecToStr(memoryTempDecToStr,getData.data[1].param2);
        (void) strcat((char*)memoryBufferLog,(const char*)memoryTempDecToStr);
        (void) strcat((char*)memoryBufferLog,",");

        (void) CLEAR_BUFFER(memoryTempDecToStr);
        DecToStr(memoryTempDecToStr,getData.data[1].param3);
        (void) strcat((char*)memoryBufferLog,(const char*)memoryTempDecToStr);
        (void) strcat((char*)memoryBufferLog,";");
        /*----------------------------------------------------------------------------*/

        /*Time ----------------------------------------------------------------------*/
        (void) CLEAR_BUFFER(memoryTempDecToStr);
        (void) strcat((char*)memoryBufferLog,TimeRTC_memory);
        DecToStr(memoryTempDecToStr,getData.data[0].param1);
        (void) strcat((char*)memoryBufferLog,(const char*)memoryTempDecToStr);
        (void) strcat((char*)memoryBufferLog,",");

        (void) CLEAR_BUFFER(memoryTempDecToStr);
        DecToStr(memoryTempDecToStr,getData.data[0].param2);
        (void) strcat((char*)memoryBufferLog,(const char*)memoryTempDecToStr);
        (void) strcat((char*)memoryBufferLog,",");

        (void) CLEAR_BUFFER(memoryTempDecToStr);
        DecToStr(memoryTempDecToStr,getData.data[0].param3);
        (void) strcat((char*)memoryBufferLog,(const char*)memoryTempDecToStr);
        (void) strcat((char*)memoryBufferLog,";");
        /*----------------------------------------------------------------------------*/

        /*Temperature ----------------------------------------------------------------*/
        (void) CLEAR_BUFFER(memoryTempDecToStr);
        (void) strcat((char*)memoryBufferLog,Temperature_memory);
        DecToStr(memoryTempDecToStr,getData.temperature_data);
        (void) strcat((char*)memoryBufferLog,(const char*)memoryTempDecToStr);
        (void) strcat((char*)memoryBufferLog,";");
        /*----------------------------------------------------------------------------*/
        
        /*Transfer the frame ----------------------------------------------------------*/
        (void) strcat((char*)memoryBufferLog,"  ");
        (void) strcpy((char*)bufferMemory,(const char*)memoryBufferLog);
        /*-----------------------------------------------------------------------------*/
    }
    else
    {
        /*Transfer the frame -----------------------------------------------------------*/
        (void) strcat((char*)memoryBufferLog,"Error Writing Data");
        (void) strcpy((char*)bufferMemory,(const char*)memoryBufferLog);
        /*------------------------------------------------------------------------------*/
    }
}

uint8_t read_Status_eeprom(void)
{
    uint8_t Tx_SPI_M = 0U;
    uint8_t Rx_SPI_M = 0U;
    HAL_GPIO_WritePin(memoryTaskHandle.Cs_MemoryPort,memoryTaskHandle.Cs_MemoryPin,RESET);
    Tx_SPI_M = RDSR;
    HAL_SPI_Transmit(memoryTaskHandle.SpiHandler,&Tx_SPI_M,1UL,100UL);
    HAL_SPI_Receive(memoryTaskHandle.SpiHandler,&Rx_SPI_M,1UL,100UL);
    HAL_GPIO_WritePin(memoryTaskHandle.Cs_MemoryPort,memoryTaskHandle.Cs_MemoryPin,SET);
    return Rx_SPI_M;
}

void memory_TimerInit(void)
{
    uint32_t prescalerValue = 0;
    prescalerValue = (uint32_t)(SystemCoreClock/TIM16_COUNTER_CLK) - 1UL; //Prescaler = (TIMx_ClOCK/CNT_CLK) - 1;

    TimHandle_Memory.Instance               = TIM16;
    TimHandle_Memory.Init.Period            = TIM16_PERIOD;
    TimHandle_Memory.Init.Prescaler         = prescalerValue;
    TimHandle_Memory.Init.ClockDivision     = 0;
    TimHandle_Memory.Init.CounterMode       = TIM_COUNTERMODE_UP;
    TimHandle_Memory.Init.RepetitionCounter = 0;
    TimHandle_Memory.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&TimHandle_Memory);

    HAL_TIM_Base_Start_IT(&TimHandle_Memory);
}

__weak void memory_MspInit(void)
{
    
}