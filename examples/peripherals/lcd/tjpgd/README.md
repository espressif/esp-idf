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

    PIN_NUM_MOSI stands for the GPIO number connected to 'MOSI' pin at LCD screen;
    PIN_NUM_CLK stands for the GPIO number connected to 'SCK' pin at LCD screen;
    PIN_NUM_MOSI stands for the GPIO number connected to 'MOSI' pin at LCD screen;
    PIN_NUM_CS stands for the GPIO number connected to 'CS' pin at LCD screen;
    PIN_NUM_DC stands for the GPIO number connected to 'DC' pin at LCD screen;
    PIN_NUM_RST stands for the GPIO number connected to 'RST' pin at LCD screen;
    PIN_NUM_BCKL stands for the GPIO number connected to 'LED' pin at LCD screen;


### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project. A flowing picture will be shown on the LCD screen. 

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Troubleshooting

For any technical queries, please open an [issue] (https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
