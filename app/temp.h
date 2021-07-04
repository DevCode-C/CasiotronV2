#ifndef __TEMP__H
#define __TEMP__H
#include "app_bsp.h"
#include "lcd.h"

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

/*
Pregunta al sensor de temperatura la ultima lectura que este ya tiene disponible.
*/
uint16_t MOD_TEMP_Read( TEMP_HandleTypeDef *htemp );

/*
Establece el valor de temperatura para la alarma inferior y la alarma superior, cuando el 
sensor supere o baje de esos valores el pin de alarma deberá activarse 
*/
void MOD_TEMP_SetAlarms( TEMP_HandleTypeDef *htemp, uint16_t lower, uint16_t upper  );

/*
Deshabilita la ventana de alarma establecida.
*/
void MOD_TEMP_DisableAlarm ( LCD_HandleTypeDef *hlcd );

#endif