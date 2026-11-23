#include "mpu6500.h"
#include "spi.h"
#include "gpio.h"
#include "delay.h"

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

void mpu_init(void) {
    mpu_write(MPU_REG_PWR_MGMT_1, 0x00);          // clear SLEEP, chip boots asleep
    for (volatile uint32_t i = 0; i < 100000; i++) { } // crude settle, ~50ms at 16MHz
}

void mpu_read_accel(mpu_axis3_t *a) {
    uint8_t buf[6];

    cs_low();
    spi_transfer(SPI1, MPU_REG_ACCEL_XOUT_H | MPU_READ);
    for (int i = 0; i < 6; i++) {
        buf[i] = spi_transfer(SPI1, 0x00); // dummy bytes to clock data out
    }
    spi_wait_idle(SPI1);
    cs_high();

    a->x = (int16_t)((buf[0] << 8) | buf[1]); // high byte first, big endian
    a->y = (int16_t)((buf[2] << 8) | buf[3]);
    a->z = (int16_t)((buf[4] << 8) | buf[5]);
}