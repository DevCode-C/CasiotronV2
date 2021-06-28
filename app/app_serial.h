#ifndef __APP_SERIAL__H
#define __APP_SERIAL__H
#include "app_bsp.h"

#define SERIAL_IDLE     0U
#define SERIAL_AT       1U
#define SERIAL_TIME     2U
#define SERIAL_DATE     3U
#define SERIAL_ALARM    4U
#define SERIAL_ERROR    5U
#define SERIAL_OK       6U



void serial_init(void);
void serial_Task(void);

void serialdle(void);
void serialAT_Sel(void);
void serialTime(void);
void serialDate(void);
void serialAlarm(void);
void serialOK(void);
void serialERROR(void);

int32_t validate_StrToInt(char * buffer);
HAL_StatusTypeDef checkDataTime(uint8_t hour, uint8_t minutes, uint16_t seconds);
HAL_StatusTypeDef checkDataDate(uint8_t day, uint8_t month, uint16_t year);
HAL_StatusTypeDef checkDataAlarm(uint8_t hour, uint8_t minutes);

typedef void (*serialSelection)(void);

#endif