#include "lcd.h"

#define CURSOR  0x80U

void MOD_LCD_Init( LCD_HandleTypeDef *hlcd )
{
    MOD_LCD_MspInit(hlcd);
    
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

}

__weak void MOD_LCD_MspInit( LCD_HandleTypeDef *hlcd ) 
{
    (void) hlcd;
}

void MOD_LCD_Command( LCD_HandleTypeDef *hlcd, uint8_t cmd )
{
    HAL_GPIO_WritePin(hlcd->CsPort,hlcd->CsPin,RESET);
    HAL_GPIO_WritePin(hlcd->RsPort,hlcd->RsPin,RESET);
    HAL_SPI_Transmit(hlcd->SpiHandler,&cmd,1,100);
    HAL_GPIO_WritePin(hlcd->CsPort,hlcd->CsPin,SET);
    
}

void MOD_LCD_Data( LCD_HandleTypeDef *hlcd, uint8_t data )
{
    HAL_GPIO_WritePin(hlcd->CsPort,hlcd->CsPin,RESET);
    HAL_GPIO_WritePin(hlcd->RsPort,hlcd->RsPin,SET);
    HAL_SPI_Transmit(hlcd->SpiHandler,&data,1,100);
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
    uint8_t row_temp = row;
    uint8_t col_temp = col;

    row_temp -= 1U;
    col_temp -= 1U;
    if ((row_temp < 2U) && (col_temp < 16U))
    {
        if (row_temp == 0U)
        {
            MOD_LCD_Command(hlcd,CURSOR | col_temp);
        }
        else if (row_temp == 1U)
        {
            MOD_LCD_Command(hlcd,CURSOR |(0x40U|col_temp));
        }
        else
        {
            
        }
        
    }
    else
    {
        row_temp = 0U;
        col_temp = 0U;
        MOD_LCD_Command(hlcd,CURSOR | col_temp);
    }
}