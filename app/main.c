#include "app_bsp.h"
#include "app_serial.h"
#include "app_clock.h"
#include "lcd.h"

/**------------------------------------------------------------------------------------------------
Brief.- Punto de entrada del programa
-------------------------------------------------------------------------------------------------*/

static uint32_t hearBeatTick    = 0; 
static uint32_t WWDGTick        = 0; 

static WWDG_HandleTypeDef WWDG_HandleInit = {0};

SPI_HandleTypeDef spi_Handle = {0};
LCD_HandleTypeDef lcd_initI = {0};
extern void initialise_monitor_handles(void);

static void heart_init(void);
static void heart_beat(void);

static void dog_init(void);
static void peth_the_dog(void);

static void spi_init(void);
void lcd_init(void);

int main( void )
{
    initialise_monitor_handles();
    printf("\n");

    HAL_Init( );
    spi_init();
    serial_init();
    heart_init();
    // dog_init();
    clock_init();
    lcd_init();

    for (; ;)
    {
        // serial_Task();
        // clock_task();
        
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

void spi_init(void)
{
    spi_Handle.Instance                  = SPI1;
    spi_Handle.Init.Mode                 = SPI_MODE_MASTER;
    spi_Handle.Init.BaudRatePrescaler    = SPI_BAUDRATEPRESCALER_16;
    spi_Handle.Init.Direction            = SPI_DIRECTION_2LINES;
    spi_Handle.Init.CLKPhase             = SPI_PHASE_2EDGE;
    spi_Handle.Init.CLKPolarity          = SPI_POLARITY_LOW;
    spi_Handle.Init.CRCCalculation       = SPI_CRCCALCULATION_DISABLE;
    spi_Handle.Init.CRCPolynomial        = 7;
    spi_Handle.Init.DataSize             = SPI_DATASIZE_8BIT;
    spi_Handle.Init.FirstBit             = SPI_FIRSTBIT_MSB;
    spi_Handle.Init.NSS                  = SPI_NSS_SOFT;
    spi_Handle.Init.TIMode               = SPI_TIMODE_DISABLE;
    HAL_SPI_Init(&spi_Handle);
    HAL_GPIO_WritePin(EEPROM_PORT,CS_EEPROM,SET);
    HAL_GPIO_WritePin(LCD_PORT,LCD_CS,SET);
}

void lcd_init(void)
{
    lcd_initI.SpiHandler = &spi_Handle;
    lcd_initI.CsPort     = LCD_PORT;
    lcd_initI.RsPort     = LCD_PORT;
    lcd_initI.RstPort    = LCD_PORT;
    lcd_initI.CsPin      = LCD_CS;
    lcd_initI.RsPin      = LCD_RS;
    lcd_initI.RstPin     = LCD_RST;

    MOD_LCD_Init(&lcd_initI);
}