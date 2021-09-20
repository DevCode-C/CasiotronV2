#include "eeprom.h"
#include "app_bsp.h"

/*Private defines ----------------------------------------------*/
#define READ    3U
#define WRITE   2U
#define WRDI    4U
#define WREN    6U
#define RDSR    5U
#define WRSR    1U

/*--------------------------------------------------------------*/

void eeprom_Init(EEPROM_HandleTypeDef * heeprom)
{
    eeprom_MspInit(heeprom);
    HAL_GPIO_WritePin(heeprom->Cs_MemoryPort,heeprom->Cs_MemoryPin,GPIO_PIN_SET);
}

uint8_t eeprom_check(EEPROM_HandleTypeDef * heeprom)
{
    uint8_t Tx_SPI_M = 0U;
    uint8_t Rx_SPI_M = 0U;
    HAL_GPIO_WritePin(heeprom->Cs_MemoryPort,heeprom->Cs_MemoryPin,RESET);
    Tx_SPI_M = RDSR;
    HAL_SPI_Transmit(heeprom->SpiHandler,&Tx_SPI_M,1UL,100UL);
    HAL_SPI_Receive(heeprom->SpiHandler,&Rx_SPI_M,1UL,100UL);
    HAL_GPIO_WritePin(heeprom->Cs_MemoryPort,heeprom->Cs_MemoryPin,SET);
    return Rx_SPI_M;
}

void eeprom_write_byte(EEPROM_HandleTypeDef *heeprom ,uint16_t addr, uint8_t data)
{
    uint8_t Tx_b_SPI_Memory[4] = {0};

    HAL_GPIO_WritePin(heeprom->Cs_MemoryPort,heeprom->Cs_MemoryPin,RESET);
    Tx_b_SPI_Memory[0] = WREN;
    HAL_SPI_Transmit(heeprom->SpiHandler,Tx_b_SPI_Memory,1UL,100UL);
    HAL_GPIO_WritePin(heeprom->Cs_MemoryPort,heeprom->Cs_MemoryPin,SET);
    

    HAL_GPIO_WritePin(heeprom->Cs_MemoryPort,heeprom->Cs_MemoryPin,RESET);
    Tx_b_SPI_Memory[0] = WRITE;
    Tx_b_SPI_Memory[1] = (uint8_t)(addr>>8U);
    Tx_b_SPI_Memory[2] = (uint8_t)(addr);
    Tx_b_SPI_Memory[3] = data;
    HAL_SPI_Transmit(heeprom->SpiHandler,Tx_b_SPI_Memory,4UL,100UL);
    HAL_GPIO_WritePin(heeprom->Cs_MemoryPort,heeprom->Cs_MemoryPin,SET);
}

uint8_t eeprom_read_byte(EEPROM_HandleTypeDef *heeprom ,uint16_t addr)
{
    uint8_t Rx_b_SPI[4] = {0};
    uint8_t Rx__SPI = 0;
    HAL_GPIO_WritePin(heeprom->Cs_MemoryPort,heeprom->Cs_MemoryPin,RESET);
    Rx_b_SPI[0] = READ; 
    Rx_b_SPI[1] = (uint8_t)(addr>>8);
    Rx_b_SPI[2] = (uint8_t)(addr);
    HAL_SPI_Transmit(heeprom->SpiHandler,Rx_b_SPI,3U,100UL);
    HAL_SPI_Receive(heeprom->SpiHandler,&Rx__SPI,1U,100UL);
    HAL_GPIO_WritePin(heeprom->Cs_MemoryPort,heeprom->Cs_MemoryPin,SET);

    return Rx__SPI;
}

void eeprom_write_data(EEPROM_HandleTypeDef * heeprom, uint16_t addr, uint8_t *data, uint8_t size)
{
    uint8_t Tx_b_SPI_Memory[4] = {0};

    HAL_GPIO_WritePin(heeprom->Cs_MemoryPort,heeprom->Cs_MemoryPin,RESET);
    Tx_b_SPI_Memory[0] = WREN;
    HAL_SPI_Transmit(heeprom->SpiHandler,Tx_b_SPI_Memory,1UL,100UL);
    HAL_GPIO_WritePin(heeprom->Cs_MemoryPort,heeprom->Cs_MemoryPin,SET);
    

    HAL_GPIO_WritePin(heeprom->Cs_MemoryPort,heeprom->Cs_MemoryPin,RESET);
    Tx_b_SPI_Memory[0] = WRITE;
    Tx_b_SPI_Memory[1] = (uint8_t)(addr>>8U);
    Tx_b_SPI_Memory[2] = (uint8_t)(addr);
    HAL_SPI_Transmit(heeprom->SpiHandler,Tx_b_SPI_Memory,3UL,100UL);
    for (uint8_t i = 0; i < size; i++)
    {
        Tx_b_SPI_Memory[3] = data[i];
        HAL_SPI_Transmit(heeprom->SpiHandler,&Tx_b_SPI_Memory[3],1UL,100UL);
    }
    HAL_GPIO_WritePin(heeprom->Cs_MemoryPort,heeprom->Cs_MemoryPin,SET);
}

void eeprom_read_data(EEPROM_HandleTypeDef *heeprom ,uint16_t addr, uint8_t *data, uint8_t size)
{
    uint8_t dataIterations = 0U;
    for (uint16_t i = addr; i < (addr + size); i++)
    {
        data[dataIterations] = eeprom_read_byte(heeprom,i);
        dataIterations++;
    }
    // (void) printf("%d: %s\n",pageNumber,buffer);
}

__weak void eeprom_MspInit(EEPROM_HandleTypeDef * heeprom)
{
    (void) heeprom;
}