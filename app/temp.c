#include "temp.h"

#define LOW_PART_SELECT     0x1FU
#define HIGH_PART_SELECT    0xF0U

#define SET_CLEAN           0x00U
#define SHIFT_NUMBER        0x04U
#define TIMEOUT_I2C_USER    0x64U
#define TEMP_CONVERTION_DIV 0x10U
#define ENABLE_ALARM_TEMP   0x08U
#define DISABLE_ALARM_TEMP  0x00U 

#define SET_TEMPERATURE_H(Temperature, Shitf)       (LOW_PART_SELECT & (Temperature >> Shitf))
#define SET_TEMPERATURE_L(Temperature, Shitf)       (HIGH_PART_SELECT & (Temperature << Shitf))

void MOD_TEMP_Init( TEMP_HandleTypeDef *htemp )
{
    MOD_TEMP_MspInit(htemp);

    uint8_t buffer[3] = {0};
    buffer[0] = RESOLUTION_REGISTER;
    buffer[1] = RESOLUTION_0_5C;
    HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,buffer,2U,TIMEOUT_I2C_USER);

    buffer[0] = CONFIGURATION_REGISTER;
    buffer[1] = SET_CLEAN;
    buffer[2] = SET_CLEAN;
    HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,buffer,3U,TIMEOUT_I2C_USER);
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
    HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,&registerPointer,1U,TIMEOUT_I2C_USER);

    HAL_I2C_Master_Receive(htemp->I2cHandler,MCP9808_ADDRES_R,buffer,2,TIMEOUT_I2C_USER);

    if (buffer[0] != 0 || buffer[1] != 0)
    {
        temperature = (buffer[0]<<NEXT_BYTE) | buffer[1];
    }

    return temperature;
}

void MOD_TEMP_SetAlarms( TEMP_HandleTypeDef *htemp, uint16_t lower, uint16_t upper  )
{
    uint8_t buffer[3] = {0};
    
    buffer[0] = ALERT_TEMP_UPPER_B_TRIP_REGISTER;
    buffer[1] = SET_TEMPERATURE_H(upper,SHIFT_NUMBER);
    buffer[2] = SET_TEMPERATURE_L(upper,SHIFT_NUMBER);
    HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,buffer,3U,TIMEOUT_I2C_USER);

    buffer[0] = ALERT_TEMP_LOWER_B_TRIP_REGISTER;
    buffer[1] = SET_TEMPERATURE_H(lower,SHIFT_NUMBER);
    buffer[2] = SET_TEMPERATURE_L(lower,SHIFT_NUMBER);
    HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,buffer,3U,TIMEOUT_I2C_USER);

    buffer[0] = CRITICAL_TEMP_TRIP_REGISTER;
    upper += 5;
    buffer[1] = SET_TEMPERATURE_H(upper,SHIFT_NUMBER);
    buffer[2] = SET_TEMPERATURE_L(upper,SHIFT_NUMBER);
    HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,buffer,3U,TIMEOUT_I2C_USER);

    buffer[0] = CONFIGURATION_REGISTER;
    buffer[1] = SET_CLEAN;
    buffer[2] = ENABLE_ALARM_TEMP;
    HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,buffer,3U,TIMEOUT_I2C_USER);

}

void MOD_TEMP_DisableAlarm ( TEMP_HandleTypeDef *htemp )
{
    uint8_t buffer[3] = {0};
    buffer[0] = CONFIGURATION_REGISTER;
    buffer[1] = SET_CLEAN;
    buffer[2] = DISABLE_ALARM_TEMP;
    HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,buffer,3U,TIMEOUT_I2C_USER);

    buffer[0] = ALERT_TEMP_UPPER_B_TRIP_REGISTER;
    buffer[1] = SET_TEMPERATURE_H(SET_CLEAN,SHIFT_NUMBER);
    buffer[2] = SET_TEMPERATURE_L(SET_CLEAN,SHIFT_NUMBER);
    HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,buffer,3U,TIMEOUT_I2C_USER);

    buffer[0] = ALERT_TEMP_LOWER_B_TRIP_REGISTER;
    buffer[1] = SET_TEMPERATURE_H(SET_CLEAN,SHIFT_NUMBER);
    buffer[2] = SET_TEMPERATURE_L(SET_CLEAN,SHIFT_NUMBER);
    HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,buffer,3U,TIMEOUT_I2C_USER);

}

void MOD_TEMP_ReadRegister( TEMP_HandleTypeDef *htemp, uint8_t * buffer, uint8_t RegisterToRead)
{
    if (RegisterToRead == RESOLUTION_REGISTER)
    {
        HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,&RegisterToRead,1U,TIMEOUT_I2C_USER);
        HAL_I2C_Master_Receive(htemp->I2cHandler,MCP9808_ADDRES_R,buffer,1,TIMEOUT_I2C_USER);
    }
    else
    {
        HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,&RegisterToRead,1U,TIMEOUT_I2C_USER);
        HAL_I2C_Master_Receive(htemp->I2cHandler,MCP9808_ADDRES_R,buffer,2,TIMEOUT_I2C_USER);
    }
}