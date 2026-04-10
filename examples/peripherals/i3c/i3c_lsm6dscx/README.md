| Supported Targets | ESP32-P4 |
| ----------------- | -------- |

# I3C LSM6DSOX Sensor Example

This example demonstrates how to use the I3C master driver to communicate with an LSM6DSOX 6-axis IMU sensor. The example showcases advanced I3C features including:

* I3C device configuration and communication
* In-Band Interrupt (IBI) event handling for tap detection
* Continuous temperature reading
* Tap event parsing and reporting (single tap, double tap)
* Multi-axis tap detection (X, Y, Z axis with direction)

The LSM6DSOX is configured to generate IBI events when tap gestures are detected, demonstrating real-time interrupt-driven sensor data processing over the I3C bus.

## How to Use Example

### Hardware Required

To run this example, you should have an Espressif development board based on a chip listed in supported targets as well as a LSM6DSOX. The LSM6DSOX is a high-performance, low-power, 6-axis inertial measurement unit (IMU). It can be used as I3C bus communication, as well as IBI feature.

### Hardware Connection

Connect the LSM6DSOX sensor to the development board which contains I3C peripheral:

#### Pin Assignment

**Note:** The following pin assignments are used by default, you can change these in the `menuconfig` .

|                  | SDA             | SCL           |
| ---------------- | -------------- | -------------- |
| ESP I3C Master   | I3C_MASTER_SDA | I3C_MASTER_SCL |
| LSM6DSOX Sensor  | SDA            | SCL            |

For the actual default value of `I3C_MASTER_SDA` and `I3C_MASTER_SCL` see `Example Configuration` in `menuconfig`.

Pins should be pull-up because I3C include open-drain operation, you can enable internal pull-up but please note not all pins support internal pull-up, refer to API doc.

### Build and Flash

Make config option correct to your project:

- CONFIG_I3C_DYNAMIC_ADDRESS_ASSIGN_BY_ENTDAA: To select whether dynamic address assignment is by ENTDAA or SETDASA.

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

After flashing and running the example, you should see output similar to:

```
I (259) main_task: Started on CPU0
I (269) main_task: Calling app_main()
I (269) i3c-lsm6dsox: Waiting for IBI events... Tap the sensor to trigger interrupt
I (269) i3c-lsm6dsox: Temperature: 27.06°C
I (279) i3c-lsm6dsox: Temperature reading task started - will read temperature every second
I (1279) i3c-lsm6dsox: Temperature: 27.02°C
I (2279) i3c-lsm6dsox: Temperature: 27.02°C
I (3279) i3c-lsm6dsox: Temperature: 26.99°C
I (4279) i3c-lsm6dsox: Temperature: 27.00°C
I (5279) i3c-lsm6dsox: Temperature: 27.04°C
I (6279) i3c-lsm6dsox: Temperature: 27.04°C
```

When you tap the sensor, you'll see tap detection events:

```
I (259) main_task: Started on CPU0
I (269) main_task: Calling app_main()
I (269) i3c-lsm6dsox: Waiting for IBI events... Tap the sensor to trigger interrupt
I (269) i3c-lsm6dsox: Temperature: 27.00°C
I (279) i3c-lsm6dsox: Temperature reading task started - will read temperature every second
I (1279) i3c-lsm6dsox: Temperature: 26.99°C
I (2279) i3c-lsm6dsox: Temperature: 26.98°C
I (3279) i3c-lsm6dsox: Temperature: 26.98°C
I (3739) i3c-lsm6dsox: Single tap detected: X-axis negative direction
I (3739) i3c-lsm6dsox: Tap interrupt active
I (3819) i3c-lsm6dsox: Single tap detected: X-axis positive direction
I (3819) i3c-lsm6dsox: Tap interrupt active
I (4279) i3c-lsm6dsox: Temperature: 27.00°C
I (5279) i3c-lsm6dsox: Temperature: 26.98°C
I (5769) i3c-lsm6dsox: Single tap detected: X-axis negative direction
I (5769) i3c-lsm6dsox: Tap interrupt active
```

The example runs for 30 seconds, continuously reading temperature every second and reporting tap events as they occur.

## Troubleshooting

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
