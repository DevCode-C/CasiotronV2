#include "temp.h"



void MOD_TEMP_Init( TEMP_HandleTypeDef *htemp )
{
    MOD_TEMP_MspInit(htemp);

    uint8_t buffer[3] = {0};
    buffer[0] = RESOLUTION_REGISTER;
    buffer[1] = _0_5C;
    HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,buffer,2U,HAL_MAX_DELAY);

    buffer[0] = CONFIGURATION_REGISTER;
    buffer[1] = 0x00U;
    buffer[2] = 0x00U;
    HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,buffer,3U,HAL_MAX_DELAY);
}

__weak void MOD_TEMP_MspInit( TEMP_HandleTypeDef *htemp )
{

}

uint16_t MOD_TEMP_Read( TEMP_HandleTypeDef *htemp )
{
    uint8_t buffer[2] = {0};
    uint8_t registerPointer = 0;
    uint16_t temperature = 0;

    registerPointer = TEMPERATURE_REGISTER;
    HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,&registerPointer,1U,HAL_MAX_DELAY);

    HAL_I2C_Master_Receive(htemp->I2cHandler,MCP9808_ADDRES_R,buffer,2,HAL_MAX_DELAY);

    if (buffer[0] != 0 && buffer[1] != 0)
    {
        temperature = (( 0x0F & buffer[0])* (2<<3)) + (buffer[1]/(2<<3));
    }

    return temperature;
}

void MOD_TEMP_SetAlarms( TEMP_HandleTypeDef *htemp, uint16_t lower, uint16_t upper  )
{
    uint8_t buffer[3] = {0};
    

    buffer[0] = ALERT_TEMP_UPPER_B_TRIP_REGISTER;
    buffer[1] = (0x0F & (upper>>4));
    buffer[2] = (0xF0 & (upper<<4));
    HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,buffer,3U,HAL_MAX_DELAY);

    buffer[0] = ALERT_TEMP_LOWER_B_TRIP_REGISTER;
    buffer[1] = (0x0F & (lower>>4));
    buffer[2] = (0xF0 & (lower<<4));
    HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,buffer,3U,HAL_MAX_DELAY);

    buffer[0] = CRITICAL_TEMP_TRIP_REGISTER;
    upper += 5;
    buffer[1] = (0x0F & (upper>>4));
    buffer[2] = (0xF0 & (upper<<4));
    HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,buffer,3U,HAL_MAX_DELAY);

    buffer[0] = CONFIGURATION_REGISTER;
    buffer[1] = 0x00U;
    buffer[2] = 0x08U;
    HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,buffer,3U,HAL_MAX_DELAY);

}

void MOD_TEMP_DisableAlarm ( TEMP_HandleTypeDef *htemp )
{
    uint8_t buffer[3] = {0};
    buffer[0] = CONFIGURATION_REGISTER;
    buffer[1] = 0x00U;
    buffer[2] = 0x00U;
    HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,buffer,3U,HAL_MAX_DELAY);

    buffer[0] = ALERT_TEMP_UPPER_B_TRIP_REGISTER;
    buffer[1] = (0x0F & (0>>4));
    buffer[2] = (0xF0 & (0<<4));
    HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,buffer,3U,HAL_MAX_DELAY);

    buffer[0] = ALERT_TEMP_LOWER_B_TRIP_REGISTER;
    buffer[1] = (0x0F & (0>>4));
    buffer[2] = (0xF0 & (0<<4));
    HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,buffer,3U,HAL_MAX_DELAY);

}

void MOD_TEMP_ReadRegister( TEMP_HandleTypeDef *htemp, uint8_t * buffer, uint8_t RegisterToRead)
{
    if (RegisterToRead == RESOLUTION_REGISTER)
    {
        HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,&RegisterToRead,1U,HAL_MAX_DELAY);
        HAL_I2C_Master_Receive(htemp->I2cHandler,MCP9808_ADDRES_R,buffer,1,HAL_MAX_DELAY);
    }
    else
    {
        HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,&RegisterToRead,1U,HAL_MAX_DELAY);
        HAL_I2C_Master_Receive(htemp->I2cHandler,MCP9808_ADDRES_R,buffer,2,HAL_MAX_DELAY);
    }
}