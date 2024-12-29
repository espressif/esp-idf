| Supported Targets | ESP32-P4 |
| ----------------- | -------- |

# LP I2C Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates the basic usage of the LP SPI driver from the LP core by reading to and writing from a sensor connected over SPI. The ULP will periodically read temperature and humidity measurements from the sensor and wake up the HP CPU if `WAKEUP_HP_CPU_LIMIT_CELSIUS` (30 degrees by default) is exceeded.

## How to use example

### Hardware Required

To run this example, you should have an ESP based development board that supports the LP SPI peripheral on the LP Core as well as a BME280 sensor. BME280 is a combined temperature, humidity and pressure sensor. More information about it can be found in at [BME280](https://www.bosch-sensortec.com/products/environmental-sensors/humidity-sensors-bme280/).

#### Pin Assignment:

**Note:** The following pin assignments are used by default.

|                         | SDI(MISO) | SDO(MOSI) | SCK   | CSB (CS) |
| ----------------------- | ----------| ----------| ----- | -------- |
| ESP32-P4 LP SPI Master  | GPIO6     | GPIO7     | GPIO8 | GPIO4    |

### Build and Flash

Enter `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

The log output should indicate that the LP core and the LP SPI peripheral have been successfully initialized. The main CPU would then enter deep sleep mode.

```bash
Not an LP core wakeup. Cause = 0
Initializing...
LP SPI initialized successfully
LP core loaded with firmware successfully
Entering deep sleep...

(When the BME280 sensor is exposed to a temperature above normal room temperature, defined as 30 degree by default in the ULP code, it will wake up the HP CPU)

LP core woke up the main CPU
Temperature 31.31 degree celsius, humidity 66.01%RH
Entering deep sleep...
```

## Troubleshooting

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
