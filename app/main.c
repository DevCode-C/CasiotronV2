#include "app_bsp.h"
#include "app_serial.h"
#include "app_clock.h"

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/

#define HEARTBEAT_TIMEBASE  50U
#define WWDG_COUNTER        127U
#define WWDG_WINDOW         100U
#define TIM3_PERIOD         1000U
#define TIM3_COUNTER_CLK    100000U

#define TASK_10MS           1U
#define TASK_30MS           3U
#define TASK_50MS           5U
#define TASK_200MS          20U

#define TASK_TIME_COMP(counter,baseTime)       (counter % baseTime) 
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

/**
 * @brief Timer 3 Init
 * 
 * @param NONE (VOID)
 * 
 * @return NONE (VOID)
*/
void timer_Init(void);

uint16_t hearBeatTickTime       = 100U;
WWDG_HandleTypeDef WWDG_HandleInit  = {0};
TIM_HandleTypeDef TimHandle;
static uint32_t counter = 0;
static uint8_t timerCount = 0;      //Variable to count up the number of iterations of timer and enter in the sequens when timerCount equal to "blinkTimeSet/50"
__IO ITStatus TimerFlag = RESET;

int main( void )
{
    HAL_Init( );
    serial_init();
    heart_init();
    clock_init();
    timer_Init();
    dog_init();
    for (; ;)
    {
        if (TimerFlag == SET)
        {
            TimerFlag = RESET;
            if ((counter % TASK_10MS) == USER_RESET)
            {
                serial_Task();
            }
            if ((counter % TASK_30MS) == USER_RESET)
            {
                peth_the_dog();
            }
            if ((counter % TASK_50MS) == USER_RESET)
            {
                heart_beat();
            }
            // if ((counter % TASK_200MS) == USER_RESET)
            if (TASK_TIME_COMP(counter,TASK_200MS) == USER_RESET)
            {
                clock_task();
            }
            
            // switch (counter)
            // {
            // case 0:
            // case 1:
            // case 2:
            // case 4:
            //     serial_Task();
            //     clock_task();
            //     heart_beat();
            //     break;
            // case 3:
            // case 6:
            //     serial_Task();
            //     clock_task();
            //     peth_the_dog();
            //     break;
            // case 5: 
            //     serial_Task();
            //     clock_task();
            //     // heart_beat();
            //     break;
            // default:
            //     break;
            // }
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

}
void heart_beat(void)
{
    uint16_t blinktimeSet = 0;  //Variable use to get the value of queue buffer
    timerCount++;               //Increment of timer
    
    while (HIL_QUEUE_IsEmpty(&QueueSerialBlink) == ELEMENTS_IN_BUFFER)   //If the buffer in not empty, read all the data, and pass tha last element in queue buffer
    {
        if (HIL_QUEUE_Read(&QueueSerialBlink,&blinktimeSet) == READ_OK)
        {
            hearBeatTickTime = blinktimeSet; 
            timerCount = 0;  
        }
    }
    if (timerCount == (hearBeatTickTime / HEARTBEAT_TIMEBASE)) // Comparetion of timerCount and  "(hearBeatTickTime / HEARTBEAT_TIMEBASE)" 
    {
        timerCount= 0;
        HAL_GPIO_TogglePin(GPIO_LED_PORT_BOARD,GPIO_LED_PIN_BOARD);
    }
}

void dog_init(void)
{
    WWDG_HandleInit.Instance = WWDG;
    WWDG_HandleInit.Init.Prescaler = WWDG_PRESCALER_8;
    WWDG_HandleInit.Init.Window = WWDG_WINDOW;
    WWDG_HandleInit.Init.Counter = WWDG_COUNTER; 
    WWDG_HandleInit.Init.EWIMode = WWDG_EWI_DISABLE;
    HAL_WWDG_Init(&WWDG_HandleInit);
}

void peth_the_dog(void)
{   
    HAL_WWDG_Refresh(&WWDG_HandleInit);
}

void timer_Init(void)
{
    uint32_t prescalerValue = 0;
    prescalerValue = (uint32_t)(SystemCoreClock/TIM3_COUNTER_CLK) - 1; //Prescaler = (TIMx_ClOCK/CNT_CLK) - 1;

    TimHandle.Instance = TIM3;
    TimHandle.Init.Period = TIM3_PERIOD; 
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
    TimerFlag = SET;
    // counter = (counter + 1) % 15;
    counter += 1;

   
}