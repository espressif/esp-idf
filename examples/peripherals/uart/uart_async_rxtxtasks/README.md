| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# UART Asynchronous Example with Separate Receive and Transfer Tasks

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how two asynchronous tasks can use the same UART interface for communication. One can use
this example to develop more complex applications for serial communication.

The example starts two FreeRTOS tasks:
1. The first task periodically transmits `Hello world` via the UART.
2. The second task task listens, receives and prints data from the UART.

## How to use example

### Hardware Required

The example can be run on any commonly available development board, that is based on the Espressif SoC. You will need a
USB cable to connect the development board to a computer, and a simple one-wire cable for shorting two pins of the board.

### Setup the Hardware

The `RXD_PIN` and `TXD_PIN` which are configurable in the code (by default `GPIO4` and `GPIO5`) need to be shorted in
order to receive back the same data which were sent out.

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

You will receive the following repeating output from the monitoring console:
```
...
I (3261) TX_TASK: Wrote 11 bytes
I (4261) RX_TASK: Read 11 bytes: 'Hello world'
I (4261) RX_TASK: 0x3ffb821c   48 65 6c 6c 6f 20 77 6f  72 6c 64                 |Hello world|
...
```

## Troubleshooting

If you do not see any output from `RX_TASK` then check if you have the `RXD_PIN` and `TXD_PIN` pins shorted on the board.
