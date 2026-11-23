#include <stdint.h>
#include "rcc.h"
#include "gpio.h"
#include "uart.h"
#include "spi.h"
#include "dwt.h"
#include "mpu6500.h"

static void put_str(const char *s) {
    while (*s) usart_write_char(USART2, *s++);
}

static void put_u32(uint32_t v) {
    char buf[11];
    int i = 0;
    if (v == 0) { usart_write_char(USART2, '0'); return; }
    while (v) { buf[i++] = (char)('0' + (v % 10)); v /= 10; }
    while (i--) usart_write_char(USART2, buf[i]);
}

static void put_i32(int32_t v) {
    if (v < 0) { usart_write_char(USART2, '-'); v = -v; }
    put_u32((uint32_t)v);
}

int main(void) {
    rcc_enable(RCC_GPIOA);
    rcc_enable(RCC_USART2);
    rcc_enable(RCC_SPI1);

    gpio_init(GPIOA, 2, GPIO_MODE_AF); gpio_set_af(GPIOA, 2, 7);  // USART2 TX
    gpio_init(GPIOA, 3, GPIO_MODE_AF); gpio_set_af(GPIOA, 3, 7);  // USART2 RX
    gpio_init(GPIOA, 5, GPIO_MODE_AF); gpio_set_af(GPIOA, 5, 5);  // SPI1 SCK
    gpio_init(GPIOA, 6, GPIO_MODE_AF); gpio_set_af(GPIOA, 6, 5);  // SPI1 MISO
    gpio_init(GPIOA, 7, GPIO_MODE_AF); gpio_set_af(GPIOA, 7, 5);  // SPI1 MOSI

    gpio_init(GPIOA, 4, GPIO_MODE_OUTPUT);
    gpio_set(GPIOA, 4);                                           // CS idle high

    usart_init(USART2, BAUD_115200);
    spi_init(SPI1, SPI_MODE_3, SPI_BAUD_DIV8, SPI_MASTER);

    dwt_init();

    put_str("who am i: "); put_u32(mpu_read(MPU_REG_WHO_AM_I)); put_str("\r\n");

    mpu_init();

    mpu_axis3_t a;

    uint32_t t0 = dwt_cycles();
    mpu_read_accel(&a);
    uint32_t t1 = dwt_cycles();

    put_str("burst cycles: "); put_u32(t1 - t0); put_str("\r\n");

    put_str("x "); put_i32(a.x);
    put_str(" y "); put_i32(a.y);
    put_str(" z "); put_i32(a.z);
    put_str("\r\n");

    while (1) { }
}