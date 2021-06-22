#include "stm32f0xx.h"
#include "stm32f0xx_hal.h"
#include <stdint.h>
#include "app_bsp.h"
#include "stm32f0xx_hal_conf.h"
#include "stm32f070xb.h"
#include "stm32f0xx_hal_rcc.h"

void HAL_MspInit( void )
{
    __HAL_RCC_PWR_CLK_ENABLE();
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState =  RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
    RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;

    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK || RCC_CLOCKTYPE_PCLK1 || RCC_CLOCKTYPE_SYSCLK;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    HAL_RCC_ClockConfig(&RCC_ClkInitStruct,FLASH_LATENCY_1);

    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_SYSCFG_CLK_ENABLE();

    GPIO_InitStructure.Pin = GPIO_PIN_5;
    GPIO_InitStructure.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOA,&GPIO_InitStructure);

    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStructure.Pin = GPIO_PIN_13;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC,&GPIO_InitStructure); 
}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStructure.Pin  = GPIO_PIN_2 | GPIO_PIN_3;
    GPIO_InitStructure.Mode     = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull     = GPIO_NOPULL;
    GPIO_InitStructure.Speed    = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Alternate = GPIO_AF1_USART2;

    HAL_GPIO_Init(GPIOA,&GPIO_InitStructure);
    HAL_NVIC_SetPriority(USART2_IRQn,1,0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
}

void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
    HAL_PWR_EnableBkUpAccess();
    __HAL_RCC_LSE_CONFIG(RCC_LSE_ON);
    __HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSE);

    __HAL_RTC_ALARMA_ENABLE(hrtc);
    HAL_NVIC_SetPriority(RTC_IRQn,2,0);
    HAL_NVIC_EnableIRQ(RTC_IRQn);
}
