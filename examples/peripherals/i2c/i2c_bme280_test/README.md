# I2C BME280 Test

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates basic usage of I2C driver by communicating with the BME280 sensor.

## How to use example

### Hardware Required

To run this example, you should have one ESP32 dev board (e.g. ESP32-WROVER Kit) or ESP32 core board (e.g. ESP32-DevKitC).  
You'd also need a BME280 sensor. BME280 is a digital humidity, pressure and temperature sensor produced by Bosch Sensortec.  
For more information about it, you can read its datasheet (https://www.mouser.com/datasheet/2/783/BST-BME280-DS002-1509607.pdf)

#### Pin Assignment:

**Note:** The following pin assignments are used by default, you can change these in code.  

|                  | SDA    | SCL    |
| ---------------- | ------ | ------ |
| ESP32 I2C Master | GPIO18 | GPIO19 |
| BME280 Sensor    | SDA    | SCL    |

- master:
  - GPIO18 is assigned as the data signal of I2C master port
  - GPIO19 is assigned as the clock signal of I2C master port

- Connection:
  - connect SDA/SCL of BME280 sensor with GPIO18/GPIO19

**Note: ** There’s no need to add an external pull-up resistors for SDA/SCL pin, because the BME280 has onboard pull-up resistors.  
Although pull-up resistors can be configured by the I2C driver, this code disables them by default.  
Custom I2C configurations for the sensor can be made in the code by replacing `bme280_init()` with `bme280_custom_init()`. Full documentation  
can be found in the `bme280.c` file.  

### Build and Flash

Enter `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```bash
I (1341) HUM_TAG: humidity = 74

I (2341) TEMP_TAG: temperature = 27

I (2341) HUM_TAG: humidity = 74

I (3341) TEMP_TAG: temperature = 27

I (3341) HUM_TAG: humidity = 74

I (4341) TEMP_TAG: temperature = 27

I (4341) HUM_TAG: humidity = 74

I (5341) TEMP_TAG: temperature = 27

I (5341) HUM_TAG: humidity = 74

I (6341) TEMP_TAG: temperature = 27

I (6341) HUM_TAG: humidity = 74
```

## Additional

- BME280 has an I2C address of 0x76.

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
