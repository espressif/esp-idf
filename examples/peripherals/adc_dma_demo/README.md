| Supported Targets | ESP32-C3 |
| ----------------- | -------- |

# ADC DMA Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to use DMA to read voltage from a GPIO pin via ADC controller.

## How to use example

### Hardware Required

* A development board with ESP32C3 SoC
* A USB cable for power supply and programming

In this example, we use `ADC_UNIT_1` by default, we need to connect a voltage source (0 ~ 3.3v) to GPIO1. If another ADC unit is selected in your application, you need to change the GPIO pin (please refer to the `ESP32C3 Technical Reference Manual`).

### Configure the project

```
idf.py menuconfig
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

Running this example, you will see the following log output on the serial monitor:

[unit_channel_data](<all the data you read from the ADC>)
```
[0_1_54e](4e 25 00 00)
[0_1_548](48 25 00 00)
[0_1_556](56 25 00 00)
```

## Troubleshooting

* program upload failure

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there are any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
