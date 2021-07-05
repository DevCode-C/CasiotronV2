#include "temp.h"



void MOD_TEMP_Init( TEMP_HandleTypeDef *htemp )
{
    uint8_t buffer[3] = {0};
    buffer[0] = RESOLUTION_REGISTER;
    buffer[1] = _0_5C;
    HAL_I2C_Master_Transmit(htemp->I2cHandler,MCP9808_ADDRES_W,buffer,2U,HAL_MAX_DELAY);



    MOD_TEMP_MspInit(htemp);    
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

    // test[0] = TEMPERATURE_REGISTER;
    // HAL_I2C_Master_Transmit(&i2c_Handle,MCP9808_ADDRES_W,test,0x01U,HAL_MAX_DELAY);
    // HAL_I2C_Master_Receive(&i2c_Handle,MCP9808_ADDRES_R,&test[1],2,HAL_MAX_DELAY);
    // uint32_t temperature =  0;
    // temperature = (((0x0F&test[1])*100) *(2<<3)) + ((test[2]*100)/(2<<3));
    // // temperature = temperature * 100;
    // printf("Temperature R: %d, %d, %ld.%ld\n", 0x0F& test[1],0xF0&test[2],temperature/100,temperature%100);
}

void MOD_TEMP_SetAlarms( TEMP_HandleTypeDef *htemp, uint16_t lower, uint16_t upper  );

void MOD_TEMP_DisableAlarm ( LCD_HandleTypeDef *hlcd );