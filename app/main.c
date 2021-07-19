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
static uint32_t counter = 0;

uint32_t task1 = 0;
uint32_t task2 = 0;
uint32_t task3 = 0;
uint32_t task4 = 0;
int main( void )
{
    HAL_Init( );
    task1 = counter;
    serial_init();
    heart_init();
    task2 = counter;
    clock_init();
    timer_Init();
    dog_init();
    for (; ;)
    {

        if (counter - task1 >= 10)
        {
            task1 = counter;
            serial_Task();
        }

        if (counter - task2 >= 50)
        {
            task2 = counter;
            clock_task();
        }
        
        
        if (counter - task3 >= 5)
        {
            task3 = counter;
            heart_beat();
        }
        
        if (counter - task4 >= 4)
        {
            task4 = counter;
            peth_the_dog();
        }
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
    task3 = counter;

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
    task4 = counter;
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

void timer_Init(void)
{
    uint32_t prescalerValue = 0;
    prescalerValue = (uint32_t)(SystemCoreClock/100000) - 1;

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
    // if (counter == 100)
    // {
    //     counter=0;
    //     HAL_GPIO_TogglePin(GPIO_LED_PORT_BOARD,GPIO_LED_PIN_BOARD);    
    // }
    counter += 1;
    
}