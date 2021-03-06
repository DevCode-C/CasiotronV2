#include "app_bsp.h"

void HAL_MspInit( void )
{
    __HAL_RCC_PWR_CLK_ENABLE();
    __HAL_RCC_SYSCFG_CLK_ENABLE();
    HAL_PWR_EnableBkUpAccess();
    RCC_ClkInitTypeDef  RCC_ClkInitStruct = {0};
    RCC_OscInitTypeDef  RCC_OscInitStruct = {0};
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_ON;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState =  RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
    RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL6;
    RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;

    HAL_RCC_OscConfig(&RCC_OscInitStruct);

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_SYSCLK;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

    HAL_RCC_ClockConfig(&RCC_ClkInitStruct,FLASH_LATENCY_1);

}

void HAL_UART_MspInit(UART_HandleTypeDef *huart)
{
    (void) huart;
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_USART2_CLK_ENABLE();
    __HAL_RCC_GPIOA_CLK_ENABLE();

    GPIO_InitStructure.Pin      = UART_PINES;
    GPIO_InitStructure.Mode     = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull     = GPIO_NOPULL;
    GPIO_InitStructure.Speed    = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Alternate = GPIO_AF1_USART2;

    HAL_GPIO_Init(UART_PORT,&GPIO_InitStructure);
    HAL_NVIC_SetPriority(USART2_IRQn,NVIC_PRIORITY_HIGHEST,0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
}

void HAL_RTC_MspInit(RTC_HandleTypeDef *hrtc)
{
    RCC_OscInitTypeDef RCC_LSEConfig;
    RCC_LSEConfig.OscillatorType = RCC_OSCILLATORTYPE_LSE;
    RCC_LSEConfig.LSEState = RCC_LSE_ON;
    HAL_RCC_OscConfig(&RCC_LSEConfig);

    __HAL_RCC_RTC_CONFIG(RCC_RTCCLKSOURCE_LSE);
    __HAL_RCC_RTC_ENABLE();
    
    __HAL_RTC_ALARMA_ENABLE(hrtc);
    HAL_NVIC_SetPriority(RTC_IRQn,NVIC_PRIORITY_LOW,0);
    HAL_NVIC_EnableIRQ(RTC_IRQn);
}

void HAL_WWDG_MspInit(WWDG_HandleTypeDef *hwwdg) 
{
    (void) hwwdg;
    __HAL_RCC_CLEAR_RESET_FLAGS();
    __HAL_RCC_WWDG_CLK_ENABLE();
}

void HAL_SPI_MspInit(SPI_HandleTypeDef *hspi) 
{
    (void) hspi;
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_SPI1_CLK_ENABLE();

    GPIO_InitStructure.Pin          = SPI_PINES;
    GPIO_InitStructure.Mode         = GPIO_MODE_AF_PP;
    GPIO_InitStructure.Pull         = GPIO_PULLUP;
    GPIO_InitStructure.Speed        = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStructure.Alternate    = GPIO_AF0_SPI1;
    HAL_GPIO_Init(SPI_PORT, &GPIO_InitStructure);
}

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c) 
{
    (void) hi2c;
    __HAL_RCC_I2C1_CONFIG(RCC_I2C1CLKSOURCE_SYSCLK);
    __HAL_RCC_I2C1_CLK_ENABLE();
    
    GPIO_InitTypeDef GPIO_InitStruct = {0};
    __HAL_RCC_GPIOB_CLK_ENABLE();

    //PB6 -> I2C1_SCL
    //PB7 -> I2C1_SDA
    GPIO_InitStruct.Pin = I2C_PINES;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
    GPIO_InitStruct.Pull = GPIO_PULLUP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF1_I2C1;
    HAL_GPIO_Init(I2C_PORT, &GPIO_InitStruct);
}

void MOD_TEMP_MspInit( TEMP_HandleTypeDef *htemp ) 
{
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStructure.Pin          = GPIO_PIN_ALERT;
    GPIO_InitStructure.Mode         = GPIO_MODE_IT_FALLING;
    GPIO_InitStructure.Pull         = GPIO_NOPULL;
    GPIO_InitStructure.Speed        = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIO_PORT_ALERT,&GPIO_InitStructure);

    htemp->AlertPort = GPIO_PORT_ALERT;
    htemp->AlertPin = GPIO_PIN_ALERT;
    HAL_NVIC_SetPriority(EXTI2_3_IRQn,NVIC_PRIORITY_HIGH,0);
}

void MOD_LCD_MspInit( LCD_HandleTypeDef *hlcd ) 
{
    (void) hlcd;
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_GPIOC_CLK_ENABLE();
    GPIO_InitStructure.Pin          = LCD_PINES;
    GPIO_InitStructure.Mode         = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull         = GPIO_NOPULL;
    GPIO_InitStructure.Speed        = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(LCD_PORT,&GPIO_InitStructure);
    HAL_GPIO_WritePin(LCD_PORT,LCD_CS,SET);
}

void HAL_TIM_Base_MspInit(TIM_HandleTypeDef *htim) 
{
    (void) htim;
    __HAL_RCC_TIM3_CLK_ENABLE();

    HAL_NVIC_SetPriority(TIM3_IRQn,NVIC_PRIORITY_HIGH,0);

    HAL_NVIC_EnableIRQ(TIM3_IRQn);
}

void eeprom_MspInit(EEPROM_HandleTypeDef * heeprom)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    __HAL_RCC_GPIOB_CLK_ENABLE();
    GPIO_InitStructure.Pin          = EEPROM_CS;
    GPIO_InitStructure.Mode         = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStructure.Pull         = GPIO_NOPULL;
    GPIO_InitStructure.Speed        = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(EEPROM_PORT,&GPIO_InitStructure);
    HAL_GPIO_WritePin(EEPROM_PORT,EEPROM_CS,SET);

    heeprom->Cs_MemoryPort = EEPROM_PORT;
    heeprom->Cs_MemoryPin = EEPROM_CS;
}