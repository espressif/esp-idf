| Supported Targets | ESP32-P4 |
| ----------------- | -------- |

# Basic I3C_I2C Master Example

This code demonstrates how to make the I3C controller work in the legacy I2C mode and read/write an I2C sensor on the bus.

## Overview

This example demonstrates basic usage of I3C driver by reading and writing from a I2C connected sensor. Please note that according to I3C protocol, I2C sensor connects to I3C bus should only supports 7-bits mode, and no clock stretch is allowed. For more restrictions, please refer to I3C protocol.

## How to use example

### Hardware Required

To run this example, you should have an Espressif development board based on a chip listed in supported targets as well as a ICM42688. The ICM-42688 is a high-performance, low-power, 6-axis inertial measurement unit (IMU). It integrates a 3-axis gyroscope and a 3-axis accelerometer in a compact package, making it suitable for a wide range of motion-tracking applications.

#### Pin Assignment

**Note:** The following pin assignments are used by default, you can change these in the `menuconfig` .

|                  | SDA             | SCL           |
| ---------------- | -------------- | -------------- |
| ESP I3C Master   | I3C_MASTER_SDA | I3C_MASTER_SCL |
| ICM42688 Sensor  | SDA            | SCL            |

For the actual default value of `I3C_MASTER_SDA` and `I3C_MASTER_SCL` see `Example Configuration` in `menuconfig`.

### Build and Flash

Enter `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```bash
I (23676) example: temp is 28.804348, accel (x,y,z) is (-1.033691, -1.349121, 1.077637)g, gyro (x,y,z) is (-11.718750, 8.789062, 1.647949)dps
I (23776) example: temp is 28.442028, accel (x,y,z) is (-1.248047, -1.398438, 1.038574)g, gyro (x,y,z) is (-29.418945, 20.324707, -0.610352)dps
I (23876) example: temp is 28.442028, accel (x,y,z) is (-1.100586, -1.314453, 1.043457)g, gyro (x,y,z) is (-1.953125, 12.512207, -2.746582)dps
I (23976) example: temp is 28.623188, accel (x,y,z) is (-1.093750, -1.402832, 0.952148)g, gyro (x,y,z) is (-9.521484, 40.344238, -12.939453)dps
I (24076) example: temp is 28.623188, accel (x,y,z) is (-1.229004, -1.395508, 0.868652)g, gyro (x,y,z) is (-3.173828, 44.921875, -15.380859)dps
I (24176) example: temp is 28.623188, accel (x,y,z) is (-1.069824, -1.367676, 0.959473)g, gyro (x,y,z) is (-10.253906, 12.268066, 0.366211)dps
I (24276) example: temp is 28.562801, accel (x,y,z) is (-1.075195, -1.382324, 0.959961)g, gyro (x,y,z) is (-7.080078, 6.591797, -0.305176)dps
I (24376) example: temp is 28.442028, accel (x,y,z) is (-1.081543, -1.389648, 0.953125)g, gyro (x,y,z) is (-6.958008, 4.943848, 0.610352)dps
I (24476) example: temp is 28.502415, accel (x,y,z) is (-1.090820, -1.385742, 0.945801)g, gyro (x,y,z) is (-6.591797, 4.150391, 0.000000)dps
```

## Troubleshooting

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
