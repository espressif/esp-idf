| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# HAL I2C test

This code demonstrates how to use hal functions to write an I2C master driver.

Note: This test is a pure hal i2c test. It CANNOT work with i2c driver in `esp_driver_i2c`.

## Overview

This test demonstrates basic usage of HAL (Hardware Abstract Layer) functions to write a simplest I2C master driver. This driver only relies on the I2C, GPIO and XTAL hardware features. Therefore, it can work on a bare metal environment, without any interrupt or memory allocation support. This example shows:
    1. How to write an I2C driver for bare metal applications, such as in a bootloader.
    2. How to use IDF HAL functions if you want to use the hardware I2C component to any 3rd party system or application.

Restricted by source we can use, this test focuses on the usage of hal functions instead of providing perfect APIs. You can arrange better APIs based on your application usage.

## How to use test

### Hardware Required

To run this test, you should have one ESP32, ESP32-S, ESP32-C, ESP32-P or ESP32-H based development board, call functions you can see the correct result. If possible, you can also see wave on logic analyzer or oscilloscope.

#### Pin Assignment:

**Note:** The following pin assignments are used by default, you can change these in the `menuconfig` .

|                  | SDA             | SCL           |
| ---------------- | -------------- | -------------- |
| ESP I2C Master   | I2C_MASTER_SDA | I2C_MASTER_SCL |
| DEVICE1          | SDA            | SCL            |
| DEVICEn          | SDA            | SCL            |

For the actual default value of `I2C_MASTER_SDA` and `I2C_MASTER_SCL` see `test Configuration` in `menuconfig`.

**Note:** There's no need to add an external pull-up resistors for SDA/SCL pin, because the driver will enable the internal pull-up resistors.

### Build and Flash

Enter `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## test Output

```bash
I (300) hal_i2c_main: HAL I2C initialized successfully
I (320) hal_i2c_main: HAL I2C write-read successfully
```

## Troubleshooting

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
