| Supported Targets | ESP32-S2 | ESP32-C3 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- |

# Temperature Sensor Example

The ESP32-S2 and ESP32-C3 has a built-in temperature sensor. The temperature sensor module contains an 8-bit Sigma-Delta ADC and a temperature offset DAC.    

The conversion relationship is the first two columns of the table below. Among them, `offset = 0`(default) is the main measurement option, and other values are extended measurement options.  

| DAC level | offset | measure range(℃) | measure error(℃) |
| :-------: | :----: | :--------------: | :--------------: |
|     0     |   -2   |     50 ~ 125     |       < 3        |
|     1     |   -1   |     20 ~ 100     |       < 2        |
|     2     |   0    |     -10 ~ 80     |       < 1        |
|     3     |   1    |     -30 ~ 50     |       < 2        |
|     4     |   2    |     -40 ~ 20     |       < 3        |

## How to use example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

* A development board with ESP32-S2 or ESP32-C3 SoC (e.g., ESP32-S2-Saola-1, ESP32-S2-DevKitM-1, ESP32-C3-DevKitM-1, etc.)
* A USB cable for power supply and programming

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (243) TempSensor: Initializing Temperature sensor
I (243) TempSensor: default dac 2, clk_div 6
I (243) TempSensor: Config temperature range [-10°C ~ 80°C], error < 1°C
I (253) TempSensor: Temperature sensor started
I (1253) TempSensor: Temperature out celsius 27.287399°C
I (2253) TempSensor: Temperature out celsius 26.848801°C
I (3253) TempSensor: Temperature out celsius 26.848801°C
I (4253) TempSensor: Temperature out celsius 27.287399°C
I (5253) TempSensor: Temperature out celsius 27.287399°C
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
