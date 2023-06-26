| Supported Targets | ESP32 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# ADC DMA Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to use ADC Continuous Read Mode (DMA Mode) to read from GPIO pins via on-chip ADC module(s).

## How to use example

### Hardware Required

* A development board with ESP32, ESP32-S and ESP32-C SoC
* A USB cable for power supply and programming

Please refer to the `channel` array defined in `adc_dma_example_main.c`. These GPIOs should be connected to voltage sources (0 ~ 3.3V). If other ADC unit(s) / channel(s) are selected in your application,
feel free to modify these definitions.

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
I (367) EXAMPLE: adc_pattern[0].atten is :0
I (377) EXAMPLE: adc_pattern[0].channel is :2
I (377) EXAMPLE: adc_pattern[0].unit is :0
I (387) EXAMPLE: adc_pattern[1].atten is :0
I (387) EXAMPLE: adc_pattern[1].channel is :3
I (397) EXAMPLE: adc_pattern[1].unit is :0
I (397) gpio: GPIO[3]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (407) gpio: GPIO[4]| InputEn: 0| OutputEn: 0| OpenDrain: 0| Pullup: 0| Pulldown: 0| Intr:0
I (417) TASK: ret is 0, ret_num is 256
I (427) EXAMPLE: Unit: ADC-1, Channel: 2, Value: 819
I (427) EXAMPLE: Unit: ADC-1, Channel: 3, Value: 7b9
I (437) EXAMPLE: Unit: ADC-1, Channel: 2, Value: 7ab
I (437) EXAMPLE: Unit: ADC-1, Channel: 3, Value: 74b
I (447) EXAMPLE: Unit: ADC-1, Channel: 2, Value: 74d
I (447) EXAMPLE: Unit: ADC-1, Channel: 3, Value: 6e5
I (457) EXAMPLE: Unit: ADC-1, Channel: 2, Value: 6ee
I (467) EXAMPLE: Unit: ADC-1, Channel: 3, Value: 680
I (467) EXAMPLE: Unit: ADC-1, Channel: 2, Value: 69a
I (477) EXAMPLE: Unit: ADC-1, Channel: 3, Value: 62f
...
```

## Troubleshooting

* program upload failure

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there are any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
