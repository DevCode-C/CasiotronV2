#include "temp.h"

void MOD_TEMP_Init( TEMP_HandleTypeDef *htemp )
{
    
}

__weak void MOD_TEMP_MspInit( TEMP_HandleTypeDef *htemp );

uint16_t MOD_TEMP_Read( TEMP_HandleTypeDef *htemp );

void MOD_TEMP_SetAlarms( TEMP_HandleTypeDef *htemp, uint16_t lower, uint16_t upper  );

void MOD_TEMP_DisableAlarm ( LCD_HandleTypeDef *hlcd );