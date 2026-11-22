include lib/stm32f446-bare-metal/stm32f446.mk

CFLAGS = -g -O0 -mcpu=cortex-m4 -mthumb -nostdlib -Wall -Wextra \
         $(F446_INC) -Ifirmware/stm32/include

SRC = $(wildcard firmware/stm32/src/*.c)

all: firmware.elf

firmware.elf: $(F446_ASM) $(SRC) $(F446_SRCS)
	arm-none-eabi-gcc $(CFLAGS) -T $(F446_LD) $(F446_ASM) $(F446_SRCS) $(SRC) -o firmware.elf

clean:
	rm -f firmware.elf

flash:
	openocd -f interface/stlink.cfg -f target/stm32f4x.cfg -c "program firmware.elf verify reset exit"

FMT_FILES := $(shell find firmware \( -name '*.c' -o -name '*.h' \) 2>/dev/null)

format:
	clang-format -i $(FMT_FILES)

format-check:
	clang-format --dry-run --Werror $(FMT_FILES)

tidy:
	clang-tidy $(FMT_FILES) -- $(F446_INC) -Ifirmware/stm32/include

.PHONY: all clean flash format format-check tidy