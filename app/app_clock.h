#ifndef __APP_CLOCK__H
#define __APP_CLOCK__H
#include "app_bsp.h"
#include "lcd.h"

#define CLOCK_IDLE          0U
#define CLOCK_SHOW          1U
#define CLOCK_SHOW_ALARM    2U
#define CLOCK_SET_DATA      3U 
#define CLOCK_ALARM_UP      4U 
// #define CLOCK_SET_TIME      3U
// #define CLOCK_SET_DATE      4U
// #define CLOCK_SET_ALARM     5U

#define TIME_TRANSITION     1000U

void clock_init(void);
void showClock(void);
void showAlarm(void);
void clockShowAlarm(void);
void clock_task(void);

HAL_StatusTypeDef setTime(uint8_t hour, uint8_t minutes, uint16_t seconds);
HAL_StatusTypeDef setDate(uint8_t day, uint8_t month, uint16_t year);
HAL_StatusTypeDef setAlarm(uint8_t hour, uint8_t minutes);

void lcd_init(void);
uint8_t dayOfWeek(uint8_t d, uint8_t m, uint16_t y);

#endif