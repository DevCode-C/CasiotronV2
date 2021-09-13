#ifndef EEPROM__H
#define EEPROM__H
    #include "temp.h"

    typedef struct __memory_struct
    {
        SPI_HandleTypeDef       *SpiHandler;
        GPIO_TypeDef            *Cs_MemoryPort;
        uint32_t                Cs_MemoryPin;
        UART_HandleTypeDef      *UartHandleM;
        RTC_HandleTypeDef       *Rtc_handleM;
        // TEMP_HandleTypeDef      *Temp_HandleM;
    }MEMORY_HandleTypeDef;
    

    void memory_Init(void);
    void memory_Task(void);

    void memory_MspInit(void);
#endif