#ifndef __APP_CLOCK__H
#define __APP_CLOCK__H
#include "app_bsp.h"
#include "lcd.h"
#include "queue.h"


/*
Inicializacion del Clock
*/
void clock_init(void);

/*
Funcion principal del reloj
*/
void clock_task(void);

#endif