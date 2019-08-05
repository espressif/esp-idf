# ADC2 Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

In this example, we use ADC2 to measure the output of DAC.

## How to use example

### Hardware Required

* A development board with ESP32 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for power supply and programming

We use ADC1_CHANNEL_7 (GPIO27) and DAC_CHANNEL_1 (GPIO25) by default, you need to short the two GPIOs (if you have changed the ADC2 and DAC channel, please refer to Chapter 4.11 of the `ESP32 Technical Reference Manual` to get the pin number).

### Configure the project

```
idf.py menuconfig
```

* Set serial port under Serial Flasher Options.
* Set ADC2 and DAC channel in "Example Configuration"

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
ADC channel 7 @ GPIO 27, DAC channel 1 @ GPIO 25.
adc2_init...
start conversion.
1: 150
2: 203
3: 250
4: 300
5: 351
6: 400
7: 441
8: 491
9: 547
10: 595
...
```

## Troubleshooting

* program upload failure

    * Hardware connection is not correct: run `idf.py -p PORT monitor`, and reboot your board to see if there are any output logs.
    * The baud rate for downloading is too high: lower your baud rate in the `menuconfig` menu, and try again.

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
