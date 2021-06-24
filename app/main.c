#include "app_bsp.h"
#include "app_serial.h"
#include "app_clock.h"

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/

extern void initialise_monitor_handles(void);

int main( void )
{
    initialise_monitor_handles();
    printf("\n");

    HAL_Init( );
    serial_init();
    clock_init();
    
    for (; ;)
    {
        serial_Task();
        clock_task();
    } 
    return 0u;
}