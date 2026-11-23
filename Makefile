include lib/stm32f446-bare-metal/stm32f446.mk

DRV_DIRS := $(wildcard drivers/*)
DRV_SRCS := $(wildcard drivers/*/*.c)
DRV_INC  := $(addprefix -I,$(DRV_DIRS))

CFLAGS = -g -O0 -mcpu=cortex-m4 -mthumb -nostdlib -Wall -Wextra \
         $(F446_INC) $(DRV_INC)

SRC = $(wildcard src/*.c) $(DRV_SRCS)

all: firmware.elf

firmware.elf: $(F446_ASM) $(SRC) $(F446_SRCS)
	arm-none-eabi-gcc $(CFLAGS) -T $(F446_LD) $(F446_ASM) $(F446_SRCS) $(SRC) -o firmware.elf

clean:
	rm -f firmware.elf

flash:
	openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program firmware.elf verify reset exit"

.PHONY: all clean flash