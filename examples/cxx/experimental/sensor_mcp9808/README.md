# Example: C++ I2C sensor read for MCP9808

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates usage of C++ exceptions in ESP-IDF.

In this example, the `sdkconfig.defaults` file sets the `CONFIG_COMPILER_CXX_EXCEPTIONS` option. 
This enables both compile time support (`-fexceptions` compiler flag) and run-time support for C++ exception handling.
This is necessary for the C++ I2C API.

## How to use example

### Hardware Required

An MCP9808 sensor and any commonly available ESP32 development board.
Pullups aren't necessary as the default pullups are enabled in the I2CMaster class.

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
Current temperature: 24.875
```

If something went wrong:
```
I2C Exception with error: ESP_FAIL (-1)
Coulnd't read sensor!
```

