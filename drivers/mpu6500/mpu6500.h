#ifndef MPU6500_H
#define MPU6500_H
#include <stdint.h>


// address byte
#define MPU_READ 0x80 // or this into the address, top bit set means read

// who am i
#define MPU_REG_WHO_AM_I 0x75 // 0x70 = 6500, 0x71 = 9250, 0x73 = 9255

// power
#define MPU_REG_PWR_MGMT_1 0x6B 
#define MPU_REG_PWR_MGMT_2 0x6C
#define MPU_REG_USER_CTRL  0x6A

#define MPU_PWR1_SLEEP  (1U << 6)
#define MPU_PWR1_WAKEUP 0x00 

// config, how fast and how sensitive
#define MPU_REG_SMPLRT_DIV    0x19
#define MPU_REG_CONFIG        0x1A
#define MPU_REG_GYRO_CONFIG   0x1B
#define MPU_REG_ACCEL_CONFIG  0x1C
#define MPU_REG_ACCEL_CONFIG2 0x1D

// data registers the sensor fills these in on its own 
// 14 registers back to back, accel then temp then gyro.
// two registers per axis because one register only holds 0 to 255
#define MPU_REG_ACCEL_XOUT_H 0x3B // 0x3B..0x40 accel x y z
#define MPU_REG_TEMP_OUT_H   0x41 // 0x41..0x42
#define MPU_REG_GYRO_XOUT_H  0x43 // 0x43..0x48 gyro x y z

typedef enum {
    MPU_GYRO_250DPS  = 0, 
    MPU_GYRO_500DPS  = 1,
    MPU_GYRO_1000DPS = 2,
    MPU_GYRO_2000DPS = 3,
} mpu_gyro_range_t;

typedef enum {
    MPU_ACCEL_2G  = 0, 
    MPU_ACCEL_4G  = 1,
    MPU_ACCEL_8G  = 2,
    MPU_ACCEL_16G = 3,
} mpu_accel_range_t;

// one reading for all three directions combined into one struct
typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} mpu_axis3_t;

uint8_t mpu_read(uint8_t reg);
void    mpu_write(uint8_t reg, uint8_t value);
void mpu_init(void);
void mpu_read_accel(mpu_axis3_t *a);

#endif // MPU6500_H