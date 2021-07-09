## LCD tjpgd example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example shows how to decode a jpeg image and display it on an SPI-interfaced LCD, and rotates the image periodically.

Due to the fact that ESP32S2 and ESP32C3 don't have enough memory to hold the decoded image, the graphic shown on the LCD is calculated randomly and has nothing to do with the picture in the example project.

If you want to adapt this example to another type of display or pinout, check [lcd_tjpgd_example_main.c](main/lcd_tjpgd_example_main.c) for comments with some implementation details.

## How to Use Example

### Hardware Required

* An ESP development board 
* An SPI-interfaced LCD
* An USB cable for power supply and programming

### Hardware Connection

If using default settings, the hardware connection can be as below:

```
        Board                               LCD Screen
      +--------+              +---------------------------------+
      |        |              |                                 |
      |    3V3 +--------------+ VCC   +----------------------+  |
      |        |              |       |                      |  |
      |    GND +--------------+ GND   |                      |  |
      |        |              |       |                      |  |
      |     23 +--------------+ MOSI  |                      |  |
      |        |              |       |                      |  |
      |     19 +--------------+ SCK   |                      |  |
      |        |              |       |                      |  |
      |     22 +--------------+ CS    |                      |  |
      |        |              |       |                      |  |
      |     21 +--------------+ DC    |                      |  |
      |        |              |       |                      |  |
      |     18 +--------------+ RST   |                      |  |
      |        |              |       |                      |  |
      |      5 +--------------+ BCKL  +----------------------+  |
      |        |              |                                 |
      +--------+              +---------------------------------+
```

If not using default settings, the interface GPIOs should be set by macros in [lcd_tjpgd_example_main.c](main/lcd_tjpgd_example_main.c), where:

| GPIO number   | LCD pin |
| :-----------: | :--: |
| PIN_NUM_CLK   | SCK  |
| PIN_NUM_CS    | CS   |
| PIN_NUM_DC    | DC   |
| PIN_NUM_RST   | RST  |
| PIN_NUM_BCKL  | LED  |
| PIN_NUM_MOSI  | MOSI |


### 8-line LCD Usage

Firstly, please run `idf.py menuconfig` and set the `Drive a LCD with 8 data lines` option at `Example Configuration`.

Check if the pins below are correctly connected as the settings by macros in [lcd_tjpgd_example_main.c](main/lcd_tjpgd_example_main.c), where:

| GPIO number   | LCD pin |
| :-----------: | :--: |
| PIN_NUM_CLK   | SCK  |
| PIN_NUM_CS    | CS   |
| PIN_NUM_DC    | DC   |
| PIN_NUM_RST   | RST  |
| PIN_NUM_BCKL  | LED  |
| PIN_NUM_MOSI  | D0   |
| PIN_NUM_DATA1 | D1   |
| PIN_NUM_DATA2 | D2   |
| PIN_NUM_DATA3 | D3   |
| PIN_NUM_DATA4 | D4   |
| PIN_NUM_DATA5 | D5   |
| PIN_NUM_DATA6 | D6   |
| PIN_NUM_DATA7 | D7   |


### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project. A flowing picture will be shown on the LCD screen. 

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Troubleshooting

For any technical queries, please open an [issue] (https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
