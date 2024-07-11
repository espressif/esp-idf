| Supported Targets | ESP32-C6 |
| ----------------- | -------- |

# LP I2C Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates the basic usage of the LP I2C driver from the LP core by reading to and writing from a sensor connected over I2C.

## How to use example

### Hardware Required

To run this example, you should have an ESP based development board that supports the LP I2C peripheral on the LP Core as well as a BH1750 sensor. BH1750 is an ambient light sensor. More information about it can be found in the [BH1750 datasheet](https://www.mouser.com/datasheet/2/348/bh1750fvi-e-186247.pdf).

#### Pin Assignment:

**Note:** The following pin assignments are used by default.

|                         | SDA   | SCL   |
| ----------------------- | ------| ------|
| ESP32-C6 LP I2C Master  | GPIO6 | GPIO7 |
| ESP32-C5 LP I2C Master  | GPIO6 | GPIO7 |
| BH1750 Sensor           | SDA   | SCL   |

**Note:** There's no need to add external pull-up resistors for SDA/SCL pin, because the driver enables the internal pull-up resistors.

### Build and Flash

Enter `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

The log output should indicate that the LP core and the LP I2C peripheral have been successfully initialized. The main CPU would then enter deep sleep mode. The LP Core should wakeup the main CPU if the Light Intensity Value breaches the set thresholds.

```bash
Not an LP core wakeup. Cause = 0
Initializing...
LP core loaded with firmware successfully
LP I2C initialized successfully
Entering deep sleep...

(When the BH1750 sensor is shielded from a light source, the Lux value should be smaller and the LP core should wakeup the main CPU)

LP core woke up the main CPU
Lux = 3
Entering deep sleep...

(When the BH1750 sensor is exposed to a direct light source, the Lux value should be larger and the LP core should wakeup the main CPU)

LP core woke up the main CPU
Lux = 1222
Entering deep sleep...
```

## Troubleshooting

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
