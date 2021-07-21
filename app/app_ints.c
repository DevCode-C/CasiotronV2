#include "stm32f0xx.h"
#include <stdint.h>
#include "app_bsp.h"
#include "stm32f0xx_hal_conf.h"


/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
void NMI_Handler( void )
{

}

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
void HardFault_Handler( void )
{
    assert_param( 0u );
}

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
void SVC_Handler( void )
{

}

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
void PendSV_Handler( void )
{

}

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
void SysTick_Handler( void )
{
    HAL_IncTick( );
}

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/
extern UART_HandleTypeDef UartHandle;
void USART2_IRQHandler(void)
{
    HAL_UART_IRQHandler(&UartHandle);
}

extern RTC_HandleTypeDef   RTC_InitStructure;

void RTC_IRQHandler(void)
{
    HAL_RTC_AlarmIRQHandler(&RTC_InitStructure);
}

void EXTI2_3_IRQHandler(void)
{
    HAL_GPIO_EXTI_IRQHandler(GPIO_PIN_ALERT);
}

extern TIM_HandleTypeDef TimHandle;
void TIM3_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&TimHandle);
}