| Supported Targets | ESP32-S2 |
| ----------------- | -------- |

# Matrix Keyboard Example (based on Dedicated GPIO)

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example mainly illustrates how to drive a common matrix keyboard using the dedicated GPIO APIs, including:

* Manipulate the level on a group of GPIOs
* Trigger edge interrupt
* Read level on a group of GPIOs

Dedicated GPIO is designed to speed up CPU operations on one or a group of GPIOs by writing assembly codes with Espressif customized instructions (please refer TRM to get more information about these instructions).

This matrix keyboard driver is interrupt-driven, supports a configurable debounce time. GPIOs used by row and column lines are also configurable during driver installation stage.

## How to use example

### Hardware Required

This example can run on any target that has the dedicated feature (e.g. ESP32-S2). It's not necessary for your matrix board to have pull-up resisters on row/column lines. The driver has enabled internal pull-up resister by default. A typical matrix board should look as follows:

```
row_0   +--------+-------------------+------------------------------+-----------------+
                 |                   |                              |
                 |       +           |       +                      |       +
                 |     +-+-+         |     +-+-+          ......    |     +-+-+
  .              +-----+   +-----+   +-----+   +-----+              +-----+   +-----+
  .                              |                   |                              |
  .                      .       |           .       |                      .       |
                         .       |           .       |    ......            .       |
                         .       |           .       |                      .       |
                         .       |           .       |                      .       |
                                 |                   |                              |
row_n   +--------+-------------------+------------------------------+-----------------+
                 |               |   |               |              |               |
                 |       +       |   |       +       |              |       +       |
                 |     +-+-+     |   |     +-+-+     |    ......    |     +-+-+     |
                 +-----+   +-----+   +-----+   +-----+              +-----+   +-----+
                                 |                   |                              |
                                 |                   |                              |
                                 |                   |                              |
                                 +                   +                              +
                                col_0               col_1          ......          col_m
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (2883) example: press event, key code = 0002
I (3003) example: release event, key code = 0002
I (5053) example: press event, key code = 0001
I (5203) example: release event, key code = 0001
I (6413) example: press event, key code = 0000
I (6583) example: release event, key code = 0000
I (7963) example: press event, key code = 0003
I (8113) example: release event, key code = 0003
I (8773) example: press event, key code = 0103
I (8923) example: release event, key code = 0103
I (9543) example: press event, key code = 0203
I (9683) example: release event, key code = 0203
```
