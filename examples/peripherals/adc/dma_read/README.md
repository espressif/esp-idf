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
I (338) ADC DMA: adc_pattern[0].atten is :0
I (348) ADC DMA: adc_pattern[0].channel is :2
I (348) ADC DMA: adc_pattern[0].unit is :0
I (358) ADC DMA: adc_pattern[1].atten is :0
I (358) ADC DMA: adc_pattern[1].channel is :3
I (368) ADC DMA: adc_pattern[1].unit is :0
I (368) ADC DMA: adc_pattern[2].atten is :0
I (378) ADC DMA: adc_pattern[2].channel is :0
I (378) ADC DMA: adc_pattern[2].unit is :1
I (388) TASK:: ret is 0, ret_num is 256
I (388) ADC DMA: Unit: 1,_Channel: 2, Value: bec
I (398) ADC DMA: Unit: 2,_Channel: 0, Value: 9cb
I (398) ADC DMA: Unit: 1,_Channel: 3, Value: acb
I (408) ADC DMA: Unit: 2,_Channel: 0, Value: 966
I (408) ADC DMA: Unit: 1,_Channel: 2, Value: b63
I (418) ADC DMA: Unit: 2,_Channel: 0, Value: 8ff
I (418) ADC DMA: Unit: 1,_Channel: 3, Value: a6b
I (428) ADC DMA: Unit: 2,_Channel: 0, Value: 8a2
...
```

## Troubleshooting

* program upload failure

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there are any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
