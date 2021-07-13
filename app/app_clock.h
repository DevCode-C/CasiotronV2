#ifndef __APP_CLOCK__H
#define __APP_CLOCK__H
#include "app_bsp.h"


/**
 * @brief Inicializacion de todos los parametros relacionados al "APP_CLOCK"
 * 
 * @param NONE (void)
 * 
 * @return NONE (void)
*/
void clock_init(void);

/**
 * @brief Funcion principal del reloj
 * 
 * @param NONE (void)
 * 
 * @return NONE (void)
 *
*/
void clock_task(void);

#endif