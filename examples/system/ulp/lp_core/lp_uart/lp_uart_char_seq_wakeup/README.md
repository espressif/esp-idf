| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-P4 |
| ----------------- | -------- | -------- | -------- |

# LP UART specific character sequence wakeup mode Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates how to use LP UART in specific character sequence wakeup mode.

## How to use example

### Hardware Required

To run this example, you should have an ESP based development board which supports LP UART wakeup such as the ESP32-C6 and a host machine with a serial input connection.

#### Pin Assignment:

**Note:** The following pin assignments are used by default.

|                         | Rx     | Tx     |
| ----------------------- | -------| -------|
| ESP32-C6                | GPIO4  | GPIO5  |
| ESP32-C5                | GPIO4  | GPIO5  |
| ESP32-P4                | GPIO15 | GPIO14 |
| Host machine            | Tx     | Rx     |

### Build and Flash

Enter `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

Use another serial monitor program/instance such as idf.py monitor, minicom or miniterm to send and receive data from the LP core. Send "hello" or "he**o" to wake up the LP core.
The default baudrate used for the example is 115200. Care must be taken that the configuration matches on both the device and the serial terminal.

## Example Output

The log output from the serial monitor connected to the main core should indicate that the LP core and the LP UART peripheral have been successfully initialized. 

```bash
Initializing...
LP UART initialized successfully
LP core loaded with firmware and running successfully
```

The log output from the serial monitor connected to the LP core after inputting "hello" will be:
```bash
Hello world
```

## Troubleshooting

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
