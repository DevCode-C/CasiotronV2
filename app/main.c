#include "app_bsp.h"
#include "app_serial.h"
#include "app_clock.h"

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/

static uint32_t hearBeatTick    = 0; 
static uint32_t WWDGTick        = 0; 
static WWDG_HandleTypeDef WWDG_HandleInit = {0};

extern void initialise_monitor_handles(void);

static void heart_init(void);
static void heart_beat(void);

static void dog_init(void);
static void peth_the_dog(void);

int main( void )
{
    initialise_monitor_handles();
    printf("\n");

    HAL_Init( );
    serial_init();
    heart_init();
    clock_init();
    // dog_init();
    
    for (; ;)
    {
        serial_Task();
        clock_task();
        heart_beat();
        // peth_the_dog();
    } 
    return 0u;
}

void heart_init(void)
{
    hearBeatTick = HAL_GetTick();
}

void heart_beat(void)
{
    if (HAL_GetTick() - hearBeatTick >= 300)
    {
        hearBeatTick = HAL_GetTick();
        HAL_GPIO_TogglePin(GPIOA,GPIO_PIN_5);
    }
    
}

void dog_init(void)
{
    WWDG_HandleInit.Instance = WWDG;
    WWDG_HandleInit.Init.Prescaler = WWDG_PRESCALER_8;
    WWDG_HandleInit.Init.Window = 83;
    WWDG_HandleInit.Init.Counter = 127; 
    WWDG_HandleInit.Init.EWIMode = WWDG_EWI_DISABLE;
    HAL_WWDG_Init(&WWDG_HandleInit);
    
    WWDGTick = HAL_GetTick();
}

void peth_the_dog(void)
{
    if ((HAL_GetTick() - WWDGTick) >= 35)
    {
        WWDGTick = HAL_GetTick();
        HAL_WWDG_Refresh(&WWDG_HandleInit);
    }
    
}