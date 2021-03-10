| Supported Targets | ESP32-C3 |
| ----------------- | -------- |

# ADC DMA Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to use DMA-Read-APIs and Single-Read-APIs to read voltage from GPIO pins via ADC controller.

## How to use example

### Hardware Required

* A development board with ESP32C3 SoC
* A USB cable for power supply and programming

For `single_read` (Single-Read-APIs example), we use `ADC1_CHANNEL_2`, `ADC1_CHANNEL_3`, `ADC1_CHANNEL_4`, `ADC2_CHANNEL_0`. Hence we need to connect voltage sources (0 ~ 3.3V) to GPIO2, GPIO3, GPIO4, GPIO5 respectively.

For `continuous_read` (DMA-Read-APIs example), we use `ADC1_CHANNEL_0`, `ADC1_CHANNEL_1` and `ADC2_CHANNEL_0`. Therefore, GPIO0, GPIO1 and GPIO5 should be connected to voltage sources (0 ~ 3.3V).

If other ADC units/channels are selected in your application, you need to change the GPIO pin (please refer to the `ESP32C3 Technical Reference Manual`).

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
```
I (322) ADC1_CH2: 7c8
I (322) ADC1_CH3: 278
I (322) ADC1_CH4: d4b
I (322) ADC2_CH0: 48
```
```
ADC1_CH0: 61b
ADC1_CH1: 39b
ADC2_CH0: 4b
```

## Troubleshooting

* program upload failure

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there are any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
