#ifndef __APP_SERIAL__H
#define __APP_SERIAL__H
#include "app_bsp.h"
#include "buffer.h"
#include "queue.h"

/**
 * @brief Serial Init and configurations of related parameters
 * 
 * @param NONE (void)
 * 
 * @return NONE (void)
*/
void serial_init(void);

/**
 * @brief Task to receive, process and transmmit data
 * 
 * @param NONE (void)
 * 
 * @return  NONE (void)
*/
void serial_Task(void);

#endif