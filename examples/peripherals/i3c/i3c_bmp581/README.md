| Supported Targets | ESP32-P4 |
| ----------------- | -------- |

# I3C BMP581 Sensor Example

Inspired by [Sparkfun's BMP581 Arduino Library](https://github.com/sparkfun/SparkFun_BMP581_Arduino_Library).

This example demonstrates how to use the I3C master driver to communicate with an Bosch BMP581 pressure and temperature sensor. The example showcases I3C features such as:

* I3C device configuration and communication
* In-Band Interrupt (IBI) event handling
* Continuous pressure and temperature reading

The BMP581 may be configured to generate IBI events when new measurement data is available, demonstrating real-time interrupt-driven sensor data processing over the I3C bus.

## How to Use Example

### Hardware Required

To run this example, you should have an Espressif development board based on a chip listed in supported targets as well as a BMP581. The BMP581 is a high-performance, low-power, absolute barometric pressure sensor. It can be used as I3C bus target, including IBI features.

### Hardware Connection

Connect the BMP581 sensor to the development board which contains I3C peripheral:

#### Pin Assignment

**Note:** The following pin assignments are used by default, you can change these in the `menuconfig` .

|                  | SDA             | SCL           |
| ---------------- | -------------- | -------------- |
| ESP I3C Master   | I3C_MASTER_SDA | I3C_MASTER_SCL |
| BMP581 Sensor    | SDA            | SCL            |

For the actual default value of `I3C_MASTER_SDA` and `I3C_MASTER_SCL` see `Example Configuration` in `menuconfig`.

Pins should be pull-up because I3C include open-drain operation, you can enable internal pull-up but please note not all pins support internal pull-up, refer to API doc.

### Build and Flash

Make config options correct to your project:

- CONFIG_I3C_DYNAMIC_ADDRESS_ASSIGN_BY_ENTDAA: To select whether dynamic address assignment is by ENTDAA or SETDASA.
- CONFIG_I3C_USE_IN_BAND_INTERRUPTS: To select whether polling or in-band interrupts are used to check for new data.

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

In polling mode, after flashing and running the example, you should see output similar to:

```
I (273) main_task: Started on CPU0
I (283) main_task: Calling app_main()
I (283) i3c-bmp581: Pressure and temperature reading task started
I (283) i3c-bmp581: Measurements will be polled/read every second
I (293) i3c-bmp581: Temperature: 24.94°C, Pressure: 92723.78Pa
I (1293) i3c-bmp581: Temperature: 24.94°C, Pressure: 92723.78Pa
I (2293) i3c-bmp581: Temperature: 24.93°C, Pressure: 92723.05Pa
I (3293) i3c-bmp581: Temperature: 24.92°C, Pressure: 92725.16Pa
I (4293) i3c-bmp581: Temperature: 24.92°C, Pressure: 92724.05Pa
I (5293) i3c-bmp581: Temperature: 24.93°C, Pressure: 92724.41Pa
I (6293) i3c-bmp581: Temperature: 24.93°C, Pressure: 92724.92Pa
I (7293) i3c-bmp581: Temperature: 24.94°C, Pressure: 92726.56Pa
```

When enabling in-band interrupts, you'll see data ready events:

```
I (273) main_task: Started on CPU0
I (283) main_task: Calling app_main()
I (283) i3c-bmp581: Pressure and temperature reading task started
I (283) i3c-bmp581: Waiting for IBI events...
I (293) i3c-bmp581: Data ready event received!
I (293) i3c-bmp581: Temperature: 24.96°C, Pressure: 92721.69Pa
I (1293) i3c-bmp581: Data ready event received!
I (1293) i3c-bmp581: Temperature: 24.97°C, Pressure: 92721.81Pa
I (2293) i3c-bmp581: Data ready event received!
I (2293) i3c-bmp581: Temperature: 24.96°C, Pressure: 92722.02Pa
I (3293) i3c-bmp581: Data ready event received!
I (3293) i3c-bmp581: Temperature: 24.97°C, Pressure: 92722.44Pa
I (4293) i3c-bmp581: Data ready event received!
I (4293) i3c-bmp581: Temperature: 24.96°C, Pressure: 92720.08Pa
I (5293) i3c-bmp581: Data ready event received!
I (5293) i3c-bmp581: Temperature: 24.95°C, Pressure: 92722.36Pa
```

The example runs for 30 seconds, continuously reading temperature every second and reporting tap events as they occur.

## Troubleshooting

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
