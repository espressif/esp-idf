# Example: C++ SPI sensor read for MCU9250 inertial/giroscope sensor

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates usage of C++ SPI classes in ESP-IDF to read the `WHO_AM_I` register of the sensor.

In this example, the `sdkconfig.defaults` file sets the `CONFIG_COMPILER_CXX_EXCEPTIONS` option. 
This enables both compile time support (`-fexceptions` compiler flag) and run-time support for C++ exception handling.
This is necessary for the C++ SPI API.

## How to use example

### Hardware Required

An MCU9250 sensor and any commonly available ESP32 development board.

### Configure the project

```
idf.py menuconfig
```

### Build and Flash

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

If the sensor is read correctly:

```
...
I (0) cpu_start: Starting scheduler on APP CPU.
Result of WHO_AM_I register: 0x71
I (437) gpio: GPIO[23]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (447) gpio: GPIO[25]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (457) gpio: GPIO[26]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (467) gpio: GPIO[27]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0

Done
```

If there's an error with the SPI peripheral:
```
...
I (0) cpu_start: Starting scheduler on APP CPU.
E (434) spi: spicommon_bus_initialize_io(429): mosi not valid
Couldn't read SPI!
```

If the SPI pins are not connected properly, the resulting read may just return 0, this error can not be detected:
```
...
I (0) cpu_start: Starting scheduler on APP CPU.
Result of WHO_AM_I register: 0x00
I (437) gpio: GPIO[23]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (447) gpio: GPIO[25]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (457) gpio: GPIO[26]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
I (467) gpio: GPIO[27]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 1| Pulldown: 0| Intr:0
```
