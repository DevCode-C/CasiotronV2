#include "temp.h"

void MOD_TEMP_Init( TEMP_HandleTypeDef *htemp )
{
    uint8_t config = 0;
    HAL_I2C_Mem_Write_IT(htemp->I2cHandler,SENSOR_ADDRES,CONFIGURATION_REGISTER,sizeof(uint8_t),&config,sizeof(uint8_t));
    MOD_TEMP_MspInit(htemp);
}

__weak void MOD_TEMP_MspInit( TEMP_HandleTypeDef *htemp )
{

}

uint16_t MOD_TEMP_Read( TEMP_HandleTypeDef *htemp );

void MOD_TEMP_SetAlarms( TEMP_HandleTypeDef *htemp, uint16_t lower, uint16_t upper  );

void MOD_TEMP_DisableAlarm ( LCD_HandleTypeDef *hlcd );