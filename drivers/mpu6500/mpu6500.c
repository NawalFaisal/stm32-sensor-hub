#include "mpu6500.h"
#include "spi.h"
#include "gpio.h"

#define CS_PORT GPIOA
#define CS_PIN  4

static void cs_low(void)  { gpio_reset(CS_PORT, CS_PIN); }
static void cs_high(void) { gpio_set(CS_PORT, CS_PIN); }

uint8_t mpu_read(uint8_t reg) {
    cs_low();
    spi_transfer(SPI1, reg | MPU_READ);       
    uint8_t value = spi_transfer(SPI1, 0x00);  //dummy
    spi_wait_idle(SPI1);
    cs_high();
    return value;
}

void mpu_write(uint8_t reg, uint8_t value) {
    cs_low();
    spi_transfer(SPI1, reg);
    spi_transfer(SPI1, value);
    spi_wait_idle(SPI1);
    cs_high();
}