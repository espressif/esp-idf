| Supported Targets | ESP32-C6 | ESP32-P4 |
| ----------------- | -------- | -------- |

# LP UART Echo Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates the usage of the LP UART driver from the LP core by reading data written to a serial console and echoing it back while the main core is in deepsleep.

## How to use example

### Hardware Required

To run this example, you should have an ESP32-C6 based development board and a host machine with a serial input connection.

#### Pin Assignment:

**Note:** The following pin assignments are used by default.

|                         | Rx    | Tx    |
| ----------------------- | ------| ------|
| ESP32-C6                | GPIO4 | GPIO5 |
| Host machine            | Tx    | Rx    |

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

The log output from the serial monitor connected to the LP core would be blank. Type using a keyboard and the monitor should echo the characters being typed.

## Troubleshooting

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
