#ifndef TEMP__H
#define TEMP__H
#include "app_bsp.h"

#define DEFAULT_ADDRES                      0x07U   //Default addres without left shitf
#define ADDRES_STATIC                       0x30U   //Addres static with left shift

#define MCP9808_ADDRES                      0x3EU
#define MCP9808_ADDRES_W                    MCP9808_ADDRES & 0xFEU
#define MCP9808_ADDRES_R                    MCP9808_ADDRES | 0x01U

//REGISTER POINTER  (WRITE-ONLY)

#define CONFIGURATION_REGISTER              0x01U   //16 bits R-W REGISTER
#define ALERT_TEMP_UPPER_B_TRIP_REGISTER    0x02U   //16 bits R-W REGISTER
#define ALERT_TEMP_LOWER_B_TRIP_REGISTER    0x03U   //16 bits R-W REGISTER
#define CRITICAL_TEMP_TRIP_REGISTER         0x04U   //16 bits R-W REGISTER
#define TEMPERATURE_REGISTER                0x05U   //16 bits R   REGISTER
#define MANUFACTURE_REGISTER                0x06U   //16 bits R   REGISTER, DEFAULT VALUE -> 0x0054U
#define DEVICE_ID_REV_REGISTER              0x07U   //16 bits R   REGISTER, DEFAULT VALUE (DEVICE-ID = 15-8 bits)-> 0x04
#define RESOLUTION_REGISTER                 0x08U   // 8 bits R-W REGISTER, (b[1:0])

//RESOLUTION BITS

#define  RESOLUTION_0_5C          0x00U   //Time conversion = 30 ms typical
#define  RESOLUTION_0_25C         0x01U   //Time conversion = 65 ms typical
#define  RESOLUTION_0_125C        0x02U   //Time conversion = 130 ms typical
#define  RESOLUTION_0_0625C       0x03U   //Time conversion = 250 ms typical (Power-up default)

#define TEMP_GREATHER_THAN_0      0U
#define TEMP_LESS_THAN_0          1U

#define BIT_SIGN_LESS            0X01000U
#define BIT_SIGN_GREATHER        0X00000U

/**
 * @brief Performs the fit of two TEMP_LIMITS registers
 * 
 * @param REGISTER1, Upper register, (Bits 8 - 15)
 * 
 * @param REGISTER2, Lower register (Bits 0 - 7)
 * 
 * @return uint8_t, Value of Registers in just one variable
*/
#define READ_REGISTERS_AND_CONVERTION_TEMP_LIMITS(REGISTER1,REGISTER2)  (((REGISTER1) << 4U)|((REGISTER2) >> 4U))

/**
 * @brief Performs the conditional selection of limits od temperature, if less o greather than zero
 * 
 * @param uint16_t Temp_value_limit, 
 * 
 * @return The value of Temperature limit value with BIT_SIGN "LESS or GREATHER"
*/
#define CONVERTION_TEMP_REG(Temp_value_limit)   (( (Temp_value_limit) > 200U ) ? (BIT_SIGN_LESS|(Temp_value_limit)) :  (BIT_SIGN_GREATHER|(Temp_value_limit)) )

/**
 * @brief Performs the conversion of the register value to decimal temperature
 * 
 * @param uint16_t temperature, Temperature register
 * 
 * @return Temperature in degrees centigrade (decimal)
*/
#define TEMP_CONVERTION_DEC(temperature)    (((0x0FU &((temperature)>>8U))*16U)+((0x00FFU & (temperature))/16U))

/**
 * @brief Check the flag temperature, this flag say if the temperature is " >= 0" or " 0 <="  
 * 
 * @param uint16_t temperature_R , Temperature register
 * 
 * @return '1' if less than or equal to zero, '0' if greater than or equal to zero
*/
#define TEMP_GREATHER_OR_LESS_THAN_0(temperature_R)         (((0x01U&((temperature)>>12U)) == 0U) ? 0U: 1U)

/**
 * @defgroup   POWER-ON RESET DEFAULTS
 * @note    The MCP9808 has an internal Power-on Reset (POR)
 *          circuit. If the power supply voltage, VDD, glitches below
 *          the VPOR threshold, the device resets the registers to
 *          the power-on default settings.
 * 
 * @ref     CONFIGURATION_REGISTER definition
 *          
 *          Default Register Data -> 0x0000
 *          {
 *              Comparator Mode
 *              Active-Low Output
 *              Alert and Critical Output
 *              Output Disabled
 *              Alert Not Asserted
 *              Interrupt Cleared
 *              Alert Limits Unlocked
 *              Critical Limit Unlocked
 *              Continuous Conversion
 *              0°C Hysteresis
 *          }
 * 
*/

typedef struct
{
    I2C_HandleTypeDef  	*I2cHandler;
    GPIO_TypeDef		*AlertPort;
    uint32_t			 AlertPin;
}TEMP_HandleTypeDef;

/**
 * @brief Inicializa el sensor para que este esté listo para recibir datos a desplegar en su pantalla,
 *      mediante la estructura TEMP_HandleTypeDef se debe seleccionar el handler del I2C
 * 
 * @param TEMP_HandleTypeDef, *htemp Pointer struct 
 * 
 * @return None (void)
*/
void MOD_TEMP_Init( TEMP_HandleTypeDef *htemp );

/**
 * @brief Extra function for add extra code only related with the application
 * 
 * @param TEMP_HandleTypeDef *htemp, *htemp Pointer struct 
 * 
 * @return NONE (void)
*/
void MOD_TEMP_MspInit( TEMP_HandleTypeDef *htemp );

/** 
 *@brief Pregunta al sensor de temperatura la ultima lectura que este ya tiene disponible.
 * 
 * @param TEMP_HandleTypeDef *htemp
 * 
 * @retval uint16_t Temperature value
*/
uint16_t MOD_TEMP_Read( TEMP_HandleTypeDef *htemp );

/**
 * @brief Establece el valor de temperatura para la alarma inferior y la alarma superior, cuando el 
 *        sensor supere o baje de esos valores el pin de alarma deberá activarse 
 * 
 * @param TEMP_HandleTypeDef, *htemp Pointer struct 
 * @param uint16_t lower,    Temperature limit in degrees centigrade (decimal)
 * @param uint16_t upper,    Temperature limit in degrees centigrade (decimal)
 * 
 * @return None (void)
*/
void MOD_TEMP_SetAlarms( TEMP_HandleTypeDef *htemp, uint16_t lower, uint16_t upper  );

/**
 * @brief Disable the window alarm of temperature
 * 
 * @param TEMP_HandleTypeDef, *htemp Pointer struct 
 * 
 * @return None (void)
*/
void MOD_TEMP_DisableAlarm ( TEMP_HandleTypeDef *htemp );

/**
 * @brief Read the registers
 * 
 * @param TEMP_HandleTypeDef, *htemp Pointer struct 
 * @param uint8_t, *buffer Array de minimo dos elementos (array[2])  
 * @param uint8_t, RegisterToRead  Registro a leer
 * 
 * @return None (void)
*/
void MOD_TEMP_ReadRegister(TEMP_HandleTypeDef *htemp, uint8_t * buffer, uint8_t RegisterToRead);

#endif