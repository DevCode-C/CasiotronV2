#ifndef __APP_SERIAL__H
#define __APP_SERIAL__H
#include "app_bsp.h"
#include "buffer.h"
#include "queue.h"

/**---------------------------------------------------------------
Brief.- Funcion que inicializa los parametros relacionados al "APP_SERIAL"
Param.- NONE (VOID)
Return.- NONE (VOID)
----------------------------------------------------------------*/
void serial_init(void);

/**---------------------------------------------------------------
Brief.- Funcion encargada de recibir, procesar y transmitir la informacion
Param.- NONE (VOID)
Return.- NONE (VOID)
----------------------------------------------------------------*/
void serial_Task(void);

#endif