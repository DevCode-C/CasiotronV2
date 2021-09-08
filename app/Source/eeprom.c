#include "eeprom.h"

/*Private defines ----------------------------------------------*/
//States 
#define MEMORY_IDLE         0U
#define MEMORY_CHECK        1U
#define MEMORY_WRITE        2U
#define MEMORY_READ         3U 
#define MEMORY_ALARM_UP     4U

#define CS      GPIO_PIN_10
#define READ    3U
#define WRITE   2U
#define WRDI    4U
#define WREN    6U
#define RDSR    5U
#define WRSR    1U
/*--------------------------------------------------------------*/

/*Private prototype --------------------------------------------*/
void memory_Idle(void);
void memory_check(void);
void memory_write(void);
void memory_read(void);

void write_byte(uint16_t addr, uint8_t data);
uint8_t read_byte(uint16_t addr);
void write_data(uint16_t addr, uint8_t *data, uint8_t size);
void read_data(uint16_t addr, uint8_t *data, uint8_t size);
/*--------------------------------------------------------------*/

typedef void (*memorySeletionState)(void);

static memorySeletionState memoryState[] = {memory_Idle, memory_check, memory_write, memory_read};

__IO static uint8_t memoryState_Sel = MEMORY_IDLE; 

void eeprom_Init(void)
{
    //Init
}

void memory_Task(void)
{
    memoryState[memoryState_Sel];
}

void memory_Idle(void)
{
    
}