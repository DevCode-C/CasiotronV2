#ifndef APP_SERIAL__H
#define APP_SERIAL__H
#include "app_bsp.h"

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