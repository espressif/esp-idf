# UART Echo Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to utilize UART interfaces of ESP32 by echoing back to the sender any data received on
UART1.

## How to use example

### Hardware Required

The example can be run on any ESP32 development board connected to a PC with a single USB cable for flashing and
monitoring. The external interface should have 3.3V outputs. You may use e.g. 3.3V compatible USB-to-Serial dongle.

### Setup the Hardware

Connect the external serial interface to the ESP32 board as follows.

  | ESP32 Interface | #define | ESP32 Pin | External UART Pin |
  | --- | --- | --- | --- |
  | Transmit Data (TxD) | ECHO_TEST_TXD | GPIO4 | RxD |
  | Receive Data (RxD) | ECHO_TEST_RXD | GPIO5 | TxD |
  | Ground | n/a | GND | GND |

Optionally, you can set-up and use a serial interface that has RTS and CTS signals in order to verify that the
hardware control flow works. Connect the extra signals according to the following table, configure both extra pins in
the example code by replacing existing `UART_PIN_NO_CHANGE` macros with the appropriate pin numbers and configure
UART1 driver to use the hardware flow control by setting `.flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS` and adding
`.rx_flow_ctrl_thresh = 122`.

  | ESP32 Interface | #define | ESP32 Pin | External UART Pin |
  | --- | --- | --- | --- |
  | Request to Send (RTS) | ECHO_TEST_RTS | GPIO18 | CTS |
  | Clear to Send (CTS) | ECHO_TEST_CTS | GPIO19 | RTS |

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

Type some characters in the terminal connected to the external serial interface. As result you should see echo in the
terminal which is used for flashing and monitoring. You can verify if the echo indeed comes from ESP32 by
disconnecting either `TxD` or `RxD` pin: no characters will appear when typing.

## Troubleshooting

You are not supposed to see the echo in the terminal which is used for flashing and monitoring, but in the other one
which is connected to UART1.
