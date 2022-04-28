# ADC Single Read Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates the following:

- How to obtain a single ADC reading from a GPIO pin using the ADC Driver's Single Read function
- How to use the ADC Calibration functions to obtain a calibrated result (in mV)

## How to use example

### Hardware Required

* A development board with ESP32 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for power supply and programming

In this example, you need to connect a voltage source (e.g. a DC power supply) to the GPIO pins specified in `single_read.c` (see the macros defined on the top of the source file). Feel free to modify the pin setting.

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
I (302) ADC1_CH2: raw  data: 4095
I (302) ADC1_CH2: cali data: 3109 mV
I (1302) ADC2_CH0: raw  data: 0
I (1302) ADC2_CH0: cali data: 0 mV
I (2302) ADC1_CH2: raw  data: 4095
I (2302) ADC1_CH2: cali data: 3109 mV
I (3302) ADC2_CH0: raw  data: 0
I (3302) ADC2_CH0: cali data: 0 mV
I (4302) ADC1_CH2: raw  data: 4095
I (4302) ADC1_CH2: cali data: 3109 mV
...
```

## Troubleshooting

If following warning is printed out, it means the calibration required eFuse bits are not burnt correctly on your board. The calibration will be skipped. Only raw data will be printed out.
```
W (300) ADC SINGLE: eFuse not burnt, skip calibration
I (1310) ADC2_CH0: raw  data: 2715
```