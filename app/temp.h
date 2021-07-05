#ifndef __TEMP__H
#define __TEMP__H
#include "app_bsp.h"
#include "lcd.h"

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

#define  _0_5C          0x00U   //Time conversion = 30 ms typical
#define  _0_25C         0x01U   //Time conversion = 65 ms typical
#define  _0_125C        0x02U   //Time conversion = 130 ms typical
#define  _0_0625C       0x03U   //Time conversion = 250 ms typical (Power-up default)


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
 * @ref     RESOLUTION REGISTER
 *          Default Register Data -> 0x03;
 *          {
 *              Resolution bits 
 *          }
*/

typedef struct
{
    I2C_HandleTypeDef  	*I2cHandler;
    GPIO_TypeDef		*AlertPort;
    uint32_t			 AlertPin;
    //agregar más elementos si se requieren
}TEMP_HandleTypeDef;

/*
Inicializa el sensor para que este esté listo para recibir datos a desplegar en su pantalla,
 mediante la estructura TEMP_HandleTypeDef se debe seleccionar el handler del I2C
*/
void MOD_TEMP_Init( TEMP_HandleTypeDef *htemp );

/*
Función de extra inicialización para agregar código exclusivo dentro de la aplicación. Esta
función se debe mandar llamar dentro de la función MOD_TEMP_Init y deberá definirse 
como weak para que se pueda redefinir dentro de la aplicación, esta función es un buen 
lugar para inicializar el pin que leerá la señal de alarma.
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
 * @param uint16_t lower,    Temperature limit
 * @param uint16_t upper,    Temperature limit 
 * 
 * @return None (void)
*/
void MOD_TEMP_SetAlarms( TEMP_HandleTypeDef *htemp, uint16_t lower, uint16_t upper  );

/**
 * @brief Deshabilita la ventana de alarma establecida.
 * 
 * @param TEMP_HandleTypeDef, *htemp Pointer struct 
 * 
 * @return None (void)
*/
void MOD_TEMP_DisableAlarm ( LCD_HandleTypeDef *hlcd );

#endif