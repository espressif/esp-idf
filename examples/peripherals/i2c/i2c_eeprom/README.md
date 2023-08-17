| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# I2C EEPROM example

This code demonstrates how to use the I2C master mode to read/write I2C EEPROM.

## Overview

This example demonstrates basic usage of I2C driver by reading and writing from a I2C connected eeprom:

If you have a new I2C application to go (for example, read the temperature data from external sensor with I2C interface), try this as a basic template, then add your own code.

## How to use example

### Hardware Required

To run this example, you should have one ESP32, ESP32-S, ESP32-C or ESP32-H based development board as well as EEPROM(s). I2C bus allow use several EEPROMs, and some of the EEPROMs' address are configurable. Some are not, for which you can use 74LS138 to set different EEPROM different slave address(It is also can be used on other types of I2C slaves).

For some I2C EEPROM slaves, they have A0, A1, A2 pins for address configuration.
For some I2C slaves, They don't have address configuration pins but have enable pins. For these kind of slave, you can use GPIO pins and external decoder to select which slave you are using.

#### Pin Assignment:

**Note:** The following pin assignments are used by default, you can change these in the `menuconfig` .

|                  | SDA             | SCL           |
| ---------------- | -------------- | -------------- |
| ESP I2C Master   | I2C_MASTER_SDA | I2C_MASTER_SCL |
| EEPROM1          | SDA            | SCL            |
| EEPROMn          | SDA            | SCL            |

For the actual default value of `I2C_MASTER_SDA` and `I2C_MASTER_SCL` see `Example Configuration` in `menuconfig`.

**Note:** There's no need to add an external pull-up resistors for SDA/SCL pin, because the driver will enable the internal pull-up resistors.

### Build and Flash

Enter `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```bash
00 01 02 03 04 05 06 07 08 09 0a 0b 0c 0d 0e 0f 
10 11 12 13 14 15 16 17 18
```

## Troubleshooting

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)