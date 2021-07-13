#ifndef __QUEUE__H
#define __QUEUE__H
#include "app_bsp.h"

typedef struct
{
    void*       Buffer;
    uint32_t	Elements;
    uint8_t     Size;     //tamaño del tipo de elementos
    uint32_t	Head;
    uint32_t	Tail;
    uint8_t	    Empty;
    uint8_t	    Full;

}QUEUE_HandleTypeDef;


/**
 * @brief   Inicializa la cola de espera colocando los elementos head y tail a cero,
 *           y los valores de empty a uno y full a cero.
 * 
 * @param QUEUE_HandleTypeDef *hqueue
 * 
 * @return NONE (void)
*/
void HIL_QUEUE_Init( QUEUE_HandleTypeDef *hqueue );


/**
 * @brief   Copia la información referenciada por el puntero vacío data al buffer controlado por hqueue, 
 *          la cantidad de bytes a copiar está indicada por el elemento Size de la estructura tipo QUEUE_HandleTypeDef
 * 
 * @param QUEUE_HandleTypeDef *hqueue
 * 
 * @param void *data 
 * 
 * @return uint8_t si la escritura es exitosa la función regresa un uno y si no un cero
*/
uint8_t HIL_QUEUE_Write( QUEUE_HandleTypeDef *hqueue, void *data );

/**
 * @brief   Lee un dato del buffer controlado por hqueue, la información es copiada en el tipo de datos 
 *          referenciado por el puntero vacío data, la cantidad de bytes a copiar está indicada por el 
 *          elemento Size de la estructura tipo QUEUE_HandleTypeDef
 * 
 * @param QUEUE_HandleTypeDef *hqueue
 * 
 * @param void *data, Direccion de los elemtos a copiar
 * 
 * @return uint8_t, si la lectura es exitosa la 
 *         función regresa un uno y si no un cero (no hay información que leer y la cola está vacía) 
*/
uint8_t HIL_QUEUE_Read( QUEUE_HandleTypeDef *hqueue, void *data );

/**
 * @brief   Verifica que la cola de espera contenga por lo menos algun elemento
 * 
 * @param QUEUE_HandleTypeDef *hqueue
 * 
 * @return uint8_t, La función regresa un uno si no hay más elementos que se puedan leer del buffer circular y 
 *          un cero si al menos existe un elemento que se pueda leer.
*/
uint8_t HIL_QUEUE_IsEmpty( QUEUE_HandleTypeDef *hqueue );

#endif