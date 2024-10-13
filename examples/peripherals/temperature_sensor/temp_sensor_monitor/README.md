| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 |
| ----------------- | -------- | -------- | -------- | -------- |

# Temperature Sensor Interrupt Example

(Refer to README.md file in path peripherals/temperature_sensor/temp_sensor to get some basic information about temperature sensor)

The ESP32-C6/H2 supports automatically monitor the temperature value continuously and gives interrupt when reaches specific value, which means user can register their callback functions when interrupt happens.

The interrupt happens in two ways.

* Absolute mode, which means when temperature reaches to a specific value, then interrupt triggered.
* Delta mode, which means the change between two consecutive samplings is larger/smaller than the settings.

## How to use example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

If you want to see clearer what interrupt did. You may need a heat gun or other heaters, when temperature reaches specific value you can see interrupt message.

### Hardware Required

* A development board with a supported target listed in the above table.
* A USB cable for power supply and programming

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (312) example: Install temperature sensor, expected temp ranger range: 10~50 ℃
I (322) temperature_sensor: Range [-10°C ~ 80°C], error < 1°C
I (332) example: Enable temperature sensor
I (332) example: Read temperature
I (342) example: Temperature value 20.27 ℃
I (1342) example: Temperature value 20.71 ℃
I (2342) example: Temperature value 22.46 ℃
I (3342) example: Temperature value 26.85 ℃
I (4342) example: Temperature value 29.92 ℃
I (5342) example: Temperature value 33.87 ℃
I (6342) example: Temperature value 41.76 ℃
I (7342) example: Temperature value 48.78 ℃
I tsens: Temperature value is higher or lower than threshold, value is 50
...


I tsens: Temperature value is higher or lower than threshold, value is 50
...


I tsens: Temperature value is higher or lower than threshold, value is 51
...


```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
