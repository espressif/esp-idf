| Supported Targets | ESP32-S2 | ESP32-S3 |
| ----------------- | -------- | -------- |

# ULP RISC-V I2C Example

This example demonstrates how to use the RTC I2C peripheral from the ULP RISC-V coprocessor in deep sleep.

The ULP program is based on the BMP180 Temperature and Pressure sensor (https://cz.mouser.com/datasheet/2/783/BST-BMP180-DS000-1509579.pdf) which has an I2C interface. The main CPU initializes the RTC I2C peripheral, the BMP180 sensor and loads the ULP program. It then goes into deep sleep.

The ULP program periodically measures the temperature and pressure values from the BMP180 sensor and wakesup the main CPU when the values are above a certain thershold.
### Hardware Required

* A development board with a SOC which has a RISC-V ULP coprocessor (e.g., ESP32-S2 Saola)
* A BMP180 sensor module
* A USB cable for power supply and programming

## Example output

Below is the output from this example.

```
Not a ULP-RISC V wakeup (cause = 0)
Initializing RTC I2C ...
RTC_I2C_STATUS_REG = 0x00000000
Reading calibration data from BMP180 ...
ac1 = 7819
ac2 = -1152
ac3 = -14317
ac4 = 34252
ac5 = 25122
ac6 = 14289
b1 = 6515
b2 = 44
mb = -32768
mc = -11786
md = 2746

Reading initial uncompensated temperature and pressure data ...
Uncompensated Temperature = 22865
Uncompensated Pressure = 41768

Real Temperature = 24.900000 deg celcius
Real Pressure = 990.640000 hPa

Entering in deep sleep

ESP-ROM:esp32s2-rc4-20191025
Build:Oct 25 2019
rst:0x5 (DSLEEP),boot:0x9 (SPI_FAST_FLASH_BOOT)
SPIWP:0xee
mode:DIO, clock div:1
load:0x3ffe6108,len:0x1298
load:0x4004c000,len:0x92c
load:0x40050000,len:0x2f04
entry 0x4004c154
W (76) spi_flash: Detected size(4096k) larger than the size in the binary image header(2048k). Using the size in the binary image header.
ULP RISC-V woke up the main CPU
Uncompensated Temperature = 22865
Uncompensated Pressure = 41765
Reading calibration data from BMP180 ...
ac1 = 7819
ac2 = -1152
ac3 = -14317
ac4 = 34252
ac5 = 25122
ac6 = 14289
b1 = 6515
b2 = 44
mb = -32768
mc = -11786
md = 2746

New Real Temperature = 24.900000 deg celcius
New Real Pressure = 990.550000 hPa
Entering in deep sleep
```
