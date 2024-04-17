| Supported Targets | ESP32-C6 | ESP32-P4 |
| ----------------- | -------- | -------- |

# LP UART Print Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates how to use print statements from a program running on the LP core.

## How to use example

### Hardware Required

To run this example, you should have an ESP32-C6 based development board and a host machine with a serial input connection.

#### Pin Assignment:

**Note:** The following pin assignments are used by default.


|                         | Tx    |
| ----------------------- | ------|
| ESP32-C6                | GPIO5 |
| Host machine            | Rx    |

### Build and Flash

Enter `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

Use another serial monitor program/instance such as idf.py monitor, minicom or miniterm to send and receive data from the LP core.
The default baudrate used for the example is 115200. Care must be taken that the configuration matches on both the device and the serial terminal.

## Example Output

The log output from the serial monitor connected to the main core should indicate that the LP core and the LP UART peripheral have been successfully initialized. The main CPU would then enter deep sleep mode.

```bash
Not an LP core wakeup. Cause = 0
Initializing...
LP UART initialized successfully
LP core loaded with firmware and running successfully
Entering deep sleep...
```

The log output from the serial monitor connected to the LP core should display output as below -

```bash
Hello from the LP core!!
This program has run 1 times
**************************
Hello from the LP core!!
This program has run 2 times
**************************
Hello from the LP core!!
This program has run 3 times
**************************
```

## Troubleshooting

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
