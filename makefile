CC = arm-none-eabi
CORE = -mcpu=cortex-m0 -mthumb -mfloat-abi=soft
TARGET = temp
SYMBOLS = -DSTM32F070xB -DUSE_HAL_DRIVER

VPATH = app/Source cmsisf0/startups half0/Src 
INCLUDES = -I half0/Inc -I cmsisf0/core -I cmsisf0/registers -I app/Header

F_SECTIONS = -ffunction-sections -fdata-sections
SPECS_V = -Wl,--gc-sections --specs=rdimon.specs --specs=nano.specs

CFLAGS = -g3 -c $(CORE) -std=gnu99 -Wall -O0 $(F_SECTIONS) $(INCLUDES) $(SYMBOLS)
LDFLAGS = $(CORE) $(SPECS_V) -T linker.ld -Wl,-Map=$(OUTPUT_F)/$(TARGET).map
OBJS_F = Build/Obj
OUTPUT_F = Build

SRCS  = main.c app_ints.c app_msps.c startup_stm32f070xb.c system_stm32f0xx.c 
SRCS += stm32f0xx_hal.c stm32f0xx_hal_cortex.c stm32f0xx_hal_rcc.c stm32f0xx_hal_flash.c
SRCS += stm32f0xx_hal_gpio.c stm32f0xx_hal_uart.c stm32f0xx_hal_dma.c stm32f0xx_hal_rtc.c
SRCS += stm32f0xx_hal_pwr.c app_serial.c app_clock.c stm32f0xx_hal_wwdg.c lcd.c stm32f0xx_hal_spi.c
SRCS += buffer.c queue.c temp.c stm32f0xx_hal_i2c.c stm32f0xx_hal_tim.c stm32f0xx_hal_tim_ex.c 
SRCS += eeprom.c

OBJS = $(SRCS:%.c=$(OBJS_F)/%.o)

all:$(TARGET)

$(TARGET) : $(addprefix $(OUTPUT_F)/,$(TARGET).elf)
	$(CC)-objcopy -Oihex $< Build/$(TARGET).hex
	$(CC)-objdump -S $< > Build/$(TARGET).lst
	$(CC)-size --format=berkeley $<

$(addprefix $(OUTPUT_F)/,$(TARGET).elf): $(OBJS)
	@$(CC)-gcc $(LDFLAGS) -o $@ $^	

$(addprefix $(OBJS_F)/,%.o) : %.c
	@mkdir -p $(OBJS_F)
	@$(CC)-gcc -MD $(CFLAGS) -o $@ $<

$(addprefix $(OBJS_F)/,%.o) : %.s
	@$(CC)-as -c $(CORE) -o $@ $<

-include $(OBJS_F)/*.d

.PHONY : misra clean flash open debug

misra:
	@cppcheck --addon=misra.json --inline-suppr --std=c99 --template=gcc --force app/Source -I app/Header

clean:
	@rm -rf $(OUTPUT_F)

flash : all
	@openocd -f interface/stlink.cfg -f target/stm32f0x.cfg -c "program $(OUTPUT_F)/$(TARGET).hex verify reset exit" 

open: 
	@openocd -f interface/stlink.cfg -f target/stm32f0x.cfg

debug: clean | all 
	@$(CC)-gdb $(OUTPUT_F)/$(TARGET).elf -q -iex "set auto-load safe-path /"