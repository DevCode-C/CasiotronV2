#ifndef EEPROM__H
#define EEPROM__H  
    // #include "app_bsp.h"
    #include "stm32f0xx.h"
    #include "stm32f0xx_hal_conf.h"

    #define PAGE_SIZE_EEPROM        32U
    #define TOTAL_PAGE_EEPROM       128U
    #define TOTAL_ADDR_EEPROM       4096UL

    /*Private Macros------------------------------------------------*/
    #define CHECK_SIZE_DIR(addr)  (32UL - ((addr) % 32UL))
    /*--------------------------------------------------------------*/

    typedef struct __memory_struct
    {
        SPI_HandleTypeDef       *SpiHandler;
        GPIO_TypeDef            *Cs_MemoryPort;
        uint32_t                Cs_MemoryPin;
    }EEPROM_HandleTypeDef;

    /**
     * @brief 
     * 
     * @param
     * 
     * @return
    */
    void eeprom_Init(EEPROM_HandleTypeDef * heeprom);
    
    /**
     * @brief 
     * 
     * @param
     * 
     * @return
    */
    void eeprom_MspInit(EEPROM_HandleTypeDef * heeprom);
    
    /**
     * @brief 
     * 
     * @param
     * 
     * @return
    */
    uint8_t eeprom_check(EEPROM_HandleTypeDef * heeprom);
    
    /**
     * @brief 
     * 
     * @param
     * 
     * @return
    */
    void eeprom_write_byte(EEPROM_HandleTypeDef *heeprom ,uint16_t addr, uint8_t data);
    
    /**
     * @brief 
     * 
     * @param
     * 
     * @return
    */
    uint8_t eeprom_read_byte(EEPROM_HandleTypeDef *heeprom ,uint16_t addr);
    
    /**
     * @brief 
     * 
     * @param
     * 
     * @return
    */
    void eeprom_write_data(EEPROM_HandleTypeDef * heeprom, uint16_t addr, uint8_t *data, uint8_t size);
    
    /**
     * @brief 
     * 
     * @param
     * 
     * @return
    */
    void eeprom_read_data(EEPROM_HandleTypeDef *heeprom ,uint16_t addr, uint8_t *data, uint8_t size);

#endif