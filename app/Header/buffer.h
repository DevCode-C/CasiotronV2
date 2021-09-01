#ifndef BUFFER__H
#define BUFFER__H
#include "app_bsp.h"

#define INVALID_CHARACTER '~'

typedef struct
{
    uint8_t  	*Buffer;
    uint32_t	Elements;
    uint32_t	Head;
    uint32_t	Tail;
    uint8_t	Empty;
    uint8_t	Full;
}BUFFER_HandleTypeDef;


/**
 * @brief  Initialize the circular buffer by setting values in head, tail, empty and full
 * 
 * @param BUFFER_HandleTypeDef *hbuffer, Control Struct 
 * 
 * @return NONE (void)
*/
void HIL_BUFFER_Init( BUFFER_HandleTypeDef *hbuffer );

/**
 * @brief  Write a uint8_t value to a circular buffer if there is at least one space available
 * 
 * @param BUFFER_HandleTypeDef *hbuffer, Control Struct 
 * 
 * @return NONE (void)
*/
void HIL_BUFFER_Write( BUFFER_HandleTypeDef *hbuffer, uint8_t data );

/**
 * @brief  Read a uint8_t value from circular buffer and that element will removed in circular buffer
 * 
 * @param BUFFER_HandleTypeDef *hbuffer, Control Struct 
 * 
 * @return uint8_t, If circular buffer contain at least one element return a valid value
*/
uint8_t HIL_BUFFER_Read( BUFFER_HandleTypeDef *hbuffer );

/**
 * @brief  Verify if the circular buffer is empty or not
 *          
 * @param BUFFER_HandleTypeDef *hbuffer, Control Struct 
 * 
 * @return uint8_t, Zero if in circular buffer contain at least one element
*/
uint8_t HIL_BUFFER_IsEmpty( BUFFER_HandleTypeDef *hbuffer );

#endif