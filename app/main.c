#include "app_bsp.h"
#include "app_serial.h"
#include "app_clock.h"

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/


/**---------------------------------------------------------------
Brief.- Inicializacion del HEART_BEAT
Param.- NONE (VOID)
Return.- NONE (VOID)
----------------------------------------------------------------*/
static void heart_init(void);

/**---------------------------------------------------------------
Brief.- Task HEART_BEAT
Param.- NONE (VOID)
Return.- NONE (VOID)
----------------------------------------------------------------*/
static void heart_beat(void);

/**---------------------------------------------------------------
Brief.- Inicializacion del WWDG
Param.- NONE (VOID)
Return.- NONE (VOID)
----------------------------------------------------------------*/
static void dog_init(void);

/**---------------------------------------------------------------
Brief.- Actualizacion del contador del WWDG
Param.- NONE (VOID)
Return.- NONE (VOID)
----------------------------------------------------------------*/
static void peth_the_dog(void);

static uint32_t hearBeatTick    = 0; 
uint32_t WWDGTick               = 0; 
WWDG_HandleTypeDef WWDG_HandleInit  = {0};

int main( void )
{
    HAL_Init( );
    serial_init();
    heart_init();
    clock_init();
    dog_init();

    for (; ;)
    {
        serial_Task();
        clock_task();
        
        heart_beat();
        peth_the_dog();
    } 
    return 0u;
}

void heart_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStructure.Pin = GPIO_LED_PIN_BOARD;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIO_LED_PORT_BOARD,&GPIO_InitStructure);
    hearBeatTick = HAL_GetTick();
}
void heart_beat(void)
{
    if (HAL_GetTick() - hearBeatTick >= 300)
    {
        hearBeatTick = HAL_GetTick();
        HAL_GPIO_TogglePin(GPIO_LED_PORT_BOARD,GPIO_LED_PIN_BOARD);
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
    if ((HAL_GetTick() - WWDGTick) >= 40)
    {
        WWDGTick = HAL_GetTick();
        HAL_WWDG_Refresh(&WWDG_HandleInit);
    }
    
}