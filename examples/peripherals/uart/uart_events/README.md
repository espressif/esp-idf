| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# UART Events Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to use the UART driver to handle special UART events. It also reads data from `UART0` directly,
and echoes it back to the monitoring console.

## How to use example

### Hardware Required

The example can be run on any development board, that is based on the Espressif SoC. The board shall be connected to a computer with a single USB cable for flashing and monitoring.

### Configure the project

```
idf.py menuconfig
```

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

Pushing `a` followed by a `b` on the keyboard will generate the following output:
```
...
I (0) cpu_start: Starting scheduler on APP CPU.
I (299) uart: queue free spaces: 20
I (2249) uart_events: uart[0] event:
I (2249) uart_events: [UART DATA]: 1
I (2249) uart_events: [DATA EVT]:
aI (12089) uart_events: uart[0] event:
I (12089) uart_events: [UART DATA]: 1
I (12089) uart_events: [DATA EVT]:
b
```
