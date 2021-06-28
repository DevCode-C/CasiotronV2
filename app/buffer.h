#ifndef __BUFFER__H
#define __BUFFER__H
#include "app_bsp.h"

#define INVALID_CHARACTER '~'

typedef struct
{
    uint8_t  	*Buffer;
    uint32_t	Elements;
    uint32_t	Head;
    uint32_t	Tail;
    uint8_t	    Empty;
    uint8_t	    Full;
}BUFFER_HandleTypeDef;

/*
Inicializa el buffer circular colando los elementos head y tail a cero, 
y los valores de empty a uno y full a cero.
*/
void HIL_BUFFER_Init( BUFFER_HandleTypeDef *hbuffer );

/*
Escribe un nuevo dato de 8 bits en el buffer si hay espacio disponible, 
de no haberlo no se escribirá dato alguno
*/
void HIL_BUFFER_Write( BUFFER_HandleTypeDef *hbuffer, uint8_t data );

/*
Lee un dato del buffer, el dato que es leído ya no existirá dentro del buffer. 
Si el buffer está vacío no se leerá ningún dato, y el valor regresado por la función no será válido
*/
uint8_t HIL_BUFFER_Read( BUFFER_HandleTypeDef *hbuffer );

/*
La función regresa un uno si no hay más elementos que se puedan leer del buffer circular y 
un cero si al menos existe un elemento que se pueda leer. Es necesario usar esta función
antes de usar la función de lectura.
*/
uint8_t HIL_BUFFER_IsEmpty( BUFFER_HandleTypeDef *hbuffer );

#endif