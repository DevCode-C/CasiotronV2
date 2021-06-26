#include "lcd.h"

void MOD_LCD_Init( LCD_HandleTypeDef *hlcd )
{
    HAL_GPIO_WritePin(hlcd->CsPort,hlcd->CsPin,SET);
    HAL_GPIO_WritePin(hlcd->RstPort,hlcd->RstPin,RESET);
    HAL_Delay(2);
    HAL_GPIO_WritePin(hlcd->RstPort,hlcd->RstPin,SET);
    HAL_Delay(20);
    MOD_LCD_Command(hlcd,WAKE_UP);
    HAL_Delay(2);
    MOD_LCD_Command(hlcd,WAKE_UP);
    MOD_LCD_Command(hlcd,WAKE_UP);
    MOD_LCD_Command(hlcd,FUNCTION_SET);
    MOD_LCD_Command(hlcd,INT_OSC_FREC);
    MOD_LCD_Command(hlcd,POWER_CONTROL);
    MOD_LCD_Command(hlcd,FOLLOWER_CONTROL);
    MOD_LCD_Command(hlcd,CONTRAST);
    MOD_LCD_Command(hlcd,DISPLAY_ON);
    MOD_LCD_Command(hlcd,ENTRY_MODE);
    MOD_LCD_Command(hlcd,CLEAR_DISPLAY);
    HAL_Delay(10);

    MOD_LCD_MspInit(hlcd);
}

__weak void MOD_LCD_MspInit( LCD_HandleTypeDef *hlcd )
{
    
}

void MOD_LCD_Command( LCD_HandleTypeDef *hlcd, uint8_t cmd )
{
    HAL_GPIO_WritePin(hlcd->CsPort,hlcd->CsPin,RESET);
    HAL_GPIO_WritePin(hlcd->RsPort,hlcd->RsPin,RESET);
    // HAL_SPI_Transmit(hlcd->SpiHandler,&cmd,1,100);
    HAL_SPI_Transmit_IT(hlcd->SpiHandler,&cmd,1);
    HAL_GPIO_WritePin(hlcd->CsPort,hlcd->CsPin,SET);
}

void MOD_LCD_Data( LCD_HandleTypeDef *hlcd, uint8_t data )
{
    HAL_GPIO_WritePin(hlcd->CsPort,hlcd->CsPin,RESET);
    HAL_GPIO_WritePin(hlcd->RsPort,hlcd->RsPin,SET);
    // HAL_SPI_Transmit(hlcd->SpiHandler,&data,1,100);
    HAL_SPI_Transmit_IT(hlcd->SpiHandler,&data,1);
    HAL_GPIO_WritePin(hlcd->CsPort,hlcd->CsPin,SET);
    
}

void MOD_LCD_String( LCD_HandleTypeDef *hlcd, char *str )
{
    uint8_t charNumber = strlen((const char*)str);
    for (uint8_t i = 0; i < charNumber; i++)
    {
        MOD_LCD_Data(hlcd,str[i]);
    }
    
}

void MOD_LCD_SetCursor( LCD_HandleTypeDef *hlcd, uint8_t row, uint8_t col )
{
    row -= 1;
    col -= 1;
    if (row < 2U && col < 16U)
    {
        if (row == 0)
        {
            MOD_LCD_Command(hlcd,0x80 | col);
        }
        else if (row == 1)
        {
            MOD_LCD_Command(hlcd,0x80 |(0x40|col));
        }
    }
    else
    {
        row = 0;
        col = 0;
        MOD_LCD_Command(hlcd,0x80 | col);
    }
}