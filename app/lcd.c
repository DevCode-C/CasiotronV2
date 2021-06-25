#include "lcd.h"

void MOD_LCD_Init( LCD_HandleTypeDef *hlcd );

__weak void MOD_LCD_MspInit( LCD_HandleTypeDef *hlcd );

void MOD_LCD_Command( LCD_HandleTypeDef *hlcd, uint8_t cmd );

void MOD_LCD_Data( LCD_HandleTypeDef *hlcd, uint8_t data );

void MOD_LCD_String( LCD_HandleTypeDef *hlcd, char *str );

void MOD_LCD_SetCursor( LCD_HandleTypeDef *hlcd, uint8_t row, uint8_t col );