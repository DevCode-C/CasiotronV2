#ifndef __APP_SERIAL__H
#define __APP_SERIAL__H
#include "app_bsp.h"
#include "buffer.h"
#include "queue.h"

/**
 * @brief Funcion que inicializa los parametros relacionados al "APP_SERIAL"
 * 
 * @param NONE (void)
 * 
 * @return NONE (void)
*/
void serial_init(void);

/**
 * @brief Funcion encargada de recibir, procesar y transmitir la informacion
 * 
 * @param NONE (void)
 * 
 * @return  NONE (void)
*/
void serial_Task(void);

#endif