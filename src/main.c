#include "rcc.h"
#include "gpio.h"
#include "uart.h"
#include "spi.h"
#include "mpu6500.h"

static void print_hex(uint8_t b) {
    const char *digits = "0123456789ABCDEF";
    usart_write_char(USART2, digits[b >> 4]);
    usart_write_char(USART2, digits[b & 0x0F]);
    usart_write_char(USART2, '\r');
    usart_write_char(USART2, '\n');
}

int main(void) {
    rcc_enable(RCC_GPIOA);
    rcc_enable(RCC_USART2);
    rcc_enable(RCC_SPI1);

    // PA2 TX, PA3 RX on AF7
    gpio_init(GPIOA, 2, GPIO_MODE_AF);
    gpio_set_af(GPIOA, 2, 7);
    gpio_init(GPIOA, 3, GPIO_MODE_AF);
    gpio_set_af(GPIOA, 3, 7);

    // PA5 SCK, PA6 MISO, PA7 MOSI on AF5
    gpio_init(GPIOA, 5, GPIO_MODE_AF);
    gpio_set_af(GPIOA, 5, 5);
    gpio_init(GPIOA, 6, GPIO_MODE_AF);
    gpio_set_af(GPIOA, 6, 5);
    gpio_init(GPIOA, 7, GPIO_MODE_AF);
    gpio_set_af(GPIOA, 7, 5);

    // PA4 chip select, idle high
    gpio_init(GPIOA, 4, GPIO_MODE_OUTPUT);
    gpio_set(GPIOA, 4);

    usart_init(USART2, BAUD_115200);
    spi_init(SPI1, SPI_MODE_3, SPI_BAUD_DIV64, SPI_MASTER);

    print_hex(mpu_read(MPU_REG_WHO_AM_I));

    while (1) { }
}