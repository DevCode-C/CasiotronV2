#ifndef __APP_CLOCK__H
#define __APP_CLOCK__H
#include "app_bsp.h"
#include "lcd.h"
#include "queue.h"


/**
 * @brief Clock Init and configurations of related parameters
 * 
 * @param NONE (void)
 * 
 * @return NONE (void)
*/
void clock_init(void);

/**
 * @brief Task to see and show data of clock (Time, date and alarm)
 * 
 * @param NONE (void)
 * 
 * @return NONE (void)
 *
*/
void clock_task(void);

#endif