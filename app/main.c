#include "app_bsp.h"
#include "app_serial.h"
#include "app_clock.h"

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/

static uint32_t hearBeatTick    = 0; 
static uint32_t WWDGTick        = 0; 
static WWDG_HandleTypeDef WWDG_HandleInit = {0};

static void heart_init(void);
static void heart_beat(void);

static void dog_init(void);
static void peth_the_dog(void);

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

    GPIO_InitStructure.Pin = GPIO_PIN_5;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA,&GPIO_InitStructure);
    
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
    /*
    PCLK = 8 Mhz

    TimeOut  = (1000*(63+1))/(PCLK/(4096*Prescaler)) -> 262 mS

    Refresh  = (1000*( Counter - Window ))/(PCLK/(4096*Prescaler))

    127 - 110 = 17 -> (1000*(17))/(PCLK/(4096*Prescaler)) = 69 mS
    
    174mS

    */
    WWDG_HandleInit.Instance = WWDG;
    WWDG_HandleInit.Init.Prescaler = WWDG_PRESCALER_8;
    WWDG_HandleInit.Init.Window = 125; //110
    WWDG_HandleInit.Init.Counter = 127;
    WWDG_HandleInit.Init.EWIMode = WWDG_EWI_DISABLE;
    HAL_WWDG_Init(&WWDG_HandleInit);
    
    WWDGTick = HAL_GetTick();
}

void peth_the_dog(void)
{
    if ((HAL_GetTick() - WWDGTick) >= 10) //70
    {
        WWDGTick = HAL_GetTick();
        HAL_WWDG_Refresh(&WWDG_HandleInit);
    }
    
}