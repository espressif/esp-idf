| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- | -------- | -------- |

## LCD tjpgd example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example shows how to decode a jpeg image and display it on an SPI-interfaced LCD, and rotates the image periodically.

If you want to adapt this example to another type of display or pinout, check [lcd_tjpgd_example_main.c](main/lcd_tjpgd_example_main.c) for comments with some implementation details.

## How to Use Example

### Hardware Required

* An ESP development board
* An SPI-interfaced LCD
* An USB cable for power supply and programming

### Hardware Connection

The connection between ESP Board and the LCD is as follows:

```
      ESP Board                            LCD Screen
      +---------+              +---------------------------------+
      |         |              |                                 |
      |     3V3 +--------------+ VCC   +----------------------+  |
      |         |              |       |                      |  |
      |     GND +--------------+ GND   |                      |  |
      |         |              |       |                      |  |
      |   DATA0 +--------------+ MOSI  |                      |  |
      |         |              |       |                      |  |
      |    PCLK +--------------+ SCK   |                      |  |
      |         |              |       |                      |  |
      |      CS +--------------+ CS    |                      |  |
      |         |              |       |                      |  |
      |     D/C +--------------+ D/C   |                      |  |
      |         |              |       |                      |  |
      |     RST +--------------+ RST   |                      |  |
      |         |              |       |                      |  |
      |BK_LIGHT +--------------+ BCKL  +----------------------+  |
      |         |              |                                 |
      +---------+              +---------------------------------+
```

The GPIO number used by this example can be changed in [lcd_tjpgd_example_main.c](main/lcd_tjpgd_example_main.c), where:

| GPIO number              | LCD pin |
| ------------------------ | ------- |
| EXAMPLE_PIN_NUM_PCLK     | SCK     |
| EXAMPLE_PIN_NUM_CS       | CS      |
| EXAMPLE_PIN_NUM_DC       | DC      |
| EXAMPLE_PIN_NUM_RST      | RST     |
| EXAMPLE_PIN_NUM_DATA0    | MOSI    |
| EXAMPLE_PIN_NUM_BK_LIGHT | BCKL    |

Especially, please pay attention to the level used to turn on the LCD backlight, some LCD module needs a low level to turn it on, while others take a high level. You can change the backlight level macro `EXAMPLE_LCD_BK_LIGHT_ON_LEVEL` in [lcd_tjpgd_example_main.c](main/lcd_tjpgd_example_main.c).


#### Extra connections for 8-line LCD (Octal SPI)

Firstly, please run `idf.py menuconfig` and set the `Drive a LCD with 8 data lines` option at `Example Configuration`.

Change the extra GPOIs used by octal SPI in [lcd_tjpgd_example_main.c](main/lcd_tjpgd_example_main.c), where:

| GPIO number   | LCD pin |
| ------------- | ------- |
| PIN_NUM_DATA1 | D1      |
| PIN_NUM_DATA2 | D2      |
| PIN_NUM_DATA3 | D3      |
| PIN_NUM_DATA4 | D4      |
| PIN_NUM_DATA5 | D5      |
| PIN_NUM_DATA6 | D6      |
| PIN_NUM_DATA7 | D7      |

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project. A flowing picture will be shown on the LCD screen.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Troubleshooting

For any technical queries, please open an [issue] (https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
