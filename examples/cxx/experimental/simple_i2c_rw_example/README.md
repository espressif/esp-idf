| Supported Targets | ESP32 | ESP32-C3 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- |

# Example: C++ I2C sensor read for MPU9250

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates usage of C++ exceptions in ESP-IDF. It is the C++ equivalent to the [I2C Simple Example](../../../peripherals/i2c/i2c_simple/) which is written in C.

In this example, the `sdkconfig.defaults` file sets the `CONFIG_COMPILER_CXX_EXCEPTIONS` option. This enables both compile time support (`-fexceptions` compiler flag) and run-time support for C++ exception handling. This is necessary for the C++ I2C API.

## How to Use This Example

### Hardware Required

To run this example, you should have one ESP32, ESP32-S series or ESP32-C series based development board as well as an MPU9250. MPU9250 is an inertial measurement unit, which contains an accelerometer, gyroscope as well as a magnetometer, for more information about it, you can read the [datasheet of the MPU9250 sensor](https://invensense.tdk.com/wp-content/uploads/2015/02/PS-MPU-9250A-01-v1.1.pdf).

#### Pin Assignment:

**Note:** The following pin assignments are used by default, you can change these in the `menuconfig` .

|                  | SDA             | SCL           |
| ---------------- | -------------- | -------------- |
| ESP I2C Master   | I2C_MASTER_SDA | I2C_MASTER_SCL |
| MPU9250 Sensor   | SDA            | SCL            |


For the actual default value of `I2C_MASTER_SDA` and `I2C_MASTER_SCL`, see `Example Configuration` in `menuconfig`.

**Note:** There's no need to add external pull-up resistors for SDA/SCL pins, because the driver will enable the internal pull-up resistors.

### Configure the project

```
idf.py menuconfig
```

### Build and Flash

```
idf.py -p <PORT> flash monitor
```

Replace <PORT> with the name of the serial port. To exit the serial monitor, type ``Ctrl-]``.

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

If the sensor is read correctly:

```bash
I (328) i2c-simple-example: I2C initialized successfully
I (338) i2c-simple-example: WHO_AM_I = 71
I (338) i2c-simple-example: I2C de-initialized successfully
```

If something went wrong:
```
I2C Exception with error: ESP_FAIL (-1)
Couldn't read sensor!
```

