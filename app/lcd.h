#ifndef __LCD__H
#define __LCD__H
#include "app_bsp.h"

#define CLEAR_DISPLAY       0x01U    //Limpia la pantalla
#define WAKE_UP             0x30U    //
#define RETURN_HOME         0x02U
#define ENTRY_MODE          0x06U
#define CONTRAST            0x70U
#define FOLLOWER_CONTROL    0x6DU
#define POWER_CONTROL       0x56U
#define FUNCTION_SET        0x39U
#define INT_OSC_FREC        0x14U

//Display Options
#define DISPLAY_ON      0x0CU    //Enciende la pantalla  
#define DISPLAY_OFF     0x08U    //Apaga la pantalla
#define CURSOR_ON       0x02U    //Cursor on
#define CURSOR_OFF      0x00U    //Cursor off
#define BLINK_ON        0x01U    //Blink on
#define BLINK_OFF       0x00U    //Blink off
 
#define LCD_CLEAR(x)     MOD_LCD_Command(x,CLEAR_DISPLAY) // x -> pointer to struct LCD_HandleTypeDef

typedef struct
{
    SPI_HandleTypeDef       *SpiHandler;
    GPIO_TypeDef            *RstPort;
    uint32_t                RstPin;
    GPIO_TypeDef            *RsPort;
    uint32_t                RsPin;
    GPIO_TypeDef            *CsPort;
    uint32_t                CsPin;

}LCD_HandleTypeDef;

/*
Inicializa el LCD para que este esté listo para recibir datos a desplegar en su pantalla, 
mediante la estructura LCD_HandleTypeDef se debe seleccionar el handler del SPI (ya configurado) 
que se va a usar así com los pines que se usarán como RST, CS y RS . 
NOTA: esta función no debe inicializar el SPI ni los
 puertos GPIO que usará el driver.
*/
void MOD_LCD_Init( LCD_HandleTypeDef *hlcd );

/*
Función de extra inicialización para agregar código exclusivo dentro de la aplicación.
*/
void MOD_LCD_MspInit( LCD_HandleTypeDef *hlcd );

/*
Manda un comando al LCD usando el SPI, la función deberá esperar lo necesario para que el comando sea 
aceptado por el LCD.
*/
void MOD_LCD_Command( LCD_HandleTypeDef *hlcd, uint8_t cmd );

/*
Manda un caracter a desplegar al LCD usando el SPI, la función deberá esperar lo necesario para que el comando 
sea aceptado por el LCD.
*/
void MOD_LCD_Data( LCD_HandleTypeDef *hlcd, uint8_t data );

/*
Manda una cadena de caracteres referenciada por str y de tamaño size al LCD a través del SPI.
*/
void MOD_LCD_String( LCD_HandleTypeDef *hlcd, char *str );

/*
Especifica las coordenadas del LCD en el que se empezara a escribir donde los valores de row: 0-1 y col: 0-15. 
*/
void MOD_LCD_SetCursor( LCD_HandleTypeDef *hlcd, uint8_t row, uint8_t col );

#endif

//luis.alberto.hernandez@capgemini.com
//carolina.rincon@capgemini.com