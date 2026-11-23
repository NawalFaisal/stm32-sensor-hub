# stm32-sensor-hub

An MPU-6500 IMU driven over SPI from an STM32F446RE, on hand-written bare-metal drivers.

## Overview

No vendor HAL and no sensor library. Peripheral drivers come from [stm32f446-bare-metal](https://github.com/NawalFaisal/stm32f446-bare-metal) as a git submodule; the sensor driver is written against the MPU's register map.

## Target Hardware

| Feature | Detail |
|---|---|
| MCU | STM32F446RE on a Nucleo-F446RE |
| Sensor | InvenSense MPU-6500, 6-axis IMU |
| Bus | SPI1, master, mode 3, 250 kHz |
| Console | USART2 at 115200 over ST-Link VCP |
| Toolchain | `arm-none-eabi-gcc`, OpenOCD |

## Project Structure

```
stm32-sensor-hub/
├── src/                        # main.c, where the MCU and the sensor meet
├── drivers/
│   └── mpu6500/                # one folder per sensor, header and source together
├── lib/
│   └── stm32f446-bare-metal/   # peripheral library, submodule
└── Makefile                    # globs drivers/*/ into the include path
```

Adding a sensor means adding a folder. No Makefile change.

## Wiring

| Breakout pin | SPI function | Nucleo |
|---|---|---|
| VCC | 3.3 V | 3V3 |
| GND | ground | GND |
| SCL | SCLK | PA5 |
| SDA | SDI, master out | PA7 |
| ADO | SDO, master in | PA6 |
| NCS | chip select | PA4 |

Pin names on the silkscreen are the I2C names. In SPI mode they carry different signals.

## Quick Start

```
git clone --recurse-submodules https://github.com/NawalFaisal/stm32-sensor-hub
```

| Command | Description |
|---|---|
| `git submodule update --init` | If cloned without `--recurse-submodules` |
| `make` | Build `firmware.elf` |
| `make flash` | Flash over OpenOCD |
| `screen /dev/ttyACM0 115200` | Open the console |

Output prints once at reset, so open the serial port before pressing the black button.

## Boot Sequence

| Step | Register | Detail |
|---|---|---|
| Identify | `WHO_AM_I` 0x75 | Returns `0x70` for an MPU-6500 |
| Wake | `PWR_MGMT_1` 0x6B | Clear `SLEEP`, the chip boots asleep |
| Read | `ACCEL_XOUT_H` 0x3B | 6 bytes, one burst, address auto-increments |

Bit 7 of the address byte means read. Data arrives high byte first, the opposite of the ADXL345.

## Measurements

**Byte order, checked against gravity.** At rest the only acceleration is 1g, which is 16384 counts at the default ±2g range. Magnitude should land near 16384 at any orientation.

```
x 1560   y -200   z -16564      magnitude 16639 = 1.016g
```

1.6% high, inside the chip's ±3% sensitivity tolerance. Swap the bytes and `0x4000` becomes `0x0040`, so Z reads 64 and the magnitude is nowhere near 1g.

**Cost of one 6-byte burst,** timed with the Cortex-M4 DWT cycle counter:

| Prescaler | SPI clock | Cycles |
|---|---|---|
| `/64` | 250 kHz | 4455 |
| `/8` | 2 MHz | 1347 |

Only wire time scales with the prescaler, so solving the two gives **903 cycles of driver overhead** and **3552 on the wire**. Theory is 7 bytes at 64 CPU cycles per bit, or 3584, so the measurement is 0.9% off.

That 903 is a floor no bus speed beats: 20% of the call at 250 kHz, 67% at 2 MHz.

## Design Notes

**Chip select lives in the application, not the driver.** SPI has no length field. A slave knows a transaction ended only because CS went high, and which pin that is depends on the board. A driver that toggles its own CS cannot share a bus.

**The register map is numbers, not a struct.** The library's peripherals are structs at fixed addresses because they sit in the MCU's address space. The MPU's registers don't; the only way to reach `0x75` is to send `0x75` down MOSI.

## Known Limitations

| Issue | Impact |
|---|---|
| Polled SPI | 80% of the call is spent waiting on `TXE` and `RXNE` at 250 kHz |
| No data-ready interrupt | Accelerometer is read once, at reset. No sampling |
| Gyro and temperature unread | Registers defined in the header, never used |
| Ranges left at boot defaults | ±2g and ±250 dps. `ACCEL_CONFIG` and `GYRO_CONFIG` never written |
| `SPI1` and PA4 hardcoded in the driver | A second sensor on the bus needs both parameterised |
| `mpu_init` uses an unmeasured busy loop | Should be a real delay |
| No error detection | SPI flags ignored; a disconnected sensor reads `0x00` rather than failing |
| No calibration or filtering | Raw counts only |
| Library's queue pulled in but unused | |
| One sensor | The layout anticipates more; there aren't any yet |

## AI Use

README drafted with AI. The register map, the read and write functions and the wiring are mine. The wake sequence and the burst read were written with AI assistance. Bugs in my own code were surfaced through AI review, and every number here came off the serial port on my own board.

## License

MIT — see LICENSE.