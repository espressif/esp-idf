| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# ADC Single Read Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates the following:

- How to obtain a oneshot ADC reading from a GPIO pin using the ADC oneshot mode driver
- How to use the ADC Calibration functions to obtain a calibrated result (in mV)

## How to use example

### Hardware Required

* A development board with ESP SoC
* A USB cable for power supply and programming

In this example, you need to connect a voltage source (e.g. a DC power supply) to the GPIO pins specified in `oneshot_read_main.c` (see the macros defined on the top of the source file). Feel free to modify the pin setting.

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
I (304) ADC_ONESHOT: calibration scheme version is Curve Fitting
I (304) ADC_ONESHOT: calibration scheme version is Curve Fitting
I (314) ADC_ONESHOT: ADC1 Channel[2] Raw Data: 0
I (314) ADC_ONESHOT: ADC1 Channel[2] Cali Voltage: 0 mV
I (1324) ADC_ONESHOT: ADC1 Channel[3] Raw Data: 664
I (1324) ADC_ONESHOT: ADC1 Channel[3] Cali Voltage: 559 mV
I (2324) ADC_ONESHOT: ADC2 Channel[0] Raw Data: 580
I (2324) ADC_ONESHOT: ADC2 Channel[0] Cali Voltage: 498 mV
I (3324) ADC_ONESHOT: ADC1 Channel[2] Raw Data: 0
I (3324) ADC_ONESHOT: ADC1 Channel[2] Cali Voltage: 0 mV
I (4324) ADC_ONESHOT: ADC1 Channel[3] Raw Data: 666
I (4324) ADC_ONESHOT: ADC1 Channel[3] Cali Voltage: 561 mV
I (5324) ADC_ONESHOT: ADC2 Channel[0] Raw Data: 575
I (5324) ADC_ONESHOT: ADC2 Channel[0] Cali Voltage: 495 mV
...
```

## Troubleshooting

If following warning is printed out, it means the calibration required eFuse bits are not burnt correctly on your board. The calibration will be skipped. Only raw data will be printed out.
```
W (300) ADC_ONESHOT: eFuse not burnt, skip calibration
I (1310) ADC_ONESHOT: ADC1 Channel[2] Raw Data: 0
```