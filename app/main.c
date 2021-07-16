#include "app_bsp.h"
#include "app_serial.h"
#include "app_clock.h"

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/


/**
 * @brief HEART_BEAT Init 
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
static void heart_init(void);

/**
 * @brief HEART_BEAT Task (Led Toggle)
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
static void heart_beat(void);

/**
 * @brief WWDG Init
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
static void dog_init(void);

/**
 * @brief WWDG Refresh
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
static void peth_the_dog(void);

void timer_Init(void);

static uint32_t hearBeatTick    = 0; 
uint32_t WWDGTick               = 0; 
uint16_t hearBeatTickTime       = 300U;
WWDG_HandleTypeDef WWDG_HandleInit  = {0};
TIM_HandleTypeDef TimHandle;

int main( void )
{
    HAL_Init( );
    // serial_init();
    heart_init();
    // clock_init();
    // dog_init();
    timer_Init();
    for (; ;)
    {
        // serial_Task();
        // clock_task();
        
        // heart_beat();
        // peth_the_dog();
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
    if (HAL_GetTick() - hearBeatTick >= hearBeatTickTime)
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

void timer_Init(void)
{
    uint32_t prescalerValue = 0;
    prescalerValue = (uint32_t)(SystemCoreClock/10000) - 1;

    TimHandle.Instance = TIM3;
    TimHandle.Init.Period = 1000;
    TimHandle.Init.Prescaler = prescalerValue;
    TimHandle.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    TimHandle.Init.CounterMode = TIM_COUNTERMODE_UP;
    TimHandle.Init.RepetitionCounter = 0;
    TimHandle.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    HAL_TIM_Base_Init(&TimHandle);

    HAL_TIM_Base_Start_IT(&TimHandle);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    HAL_GPIO_TogglePin(GPIO_LED_PORT_BOARD,GPIO_LED_PIN_BOARD);
}