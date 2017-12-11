# UART Echo Example

This is an example which echoes any data it receives on UART1 back to the sender.

## Setup

1. Connect an external serial interface to an ESP32 board. The external interface should have 3.3V outputs. You may use e.g. 3.3V compatible USB-to-serial dongle:

  | ESP32 Interface | #define | ESP32 Pin | External UART Pin |
  | --- | --- | --- | --- |
  | Transmit Data (TxD) | ECHO_TEST_TXD | GPIO4 | RxD |
  | Receive Data (RxD) | ECHO_TEST_RXD | GPIO5 | TxD |
  | Ground | n/a | GND | GND |

2. Compile and load the example to the ESP32 board
3. Refer to the example and set up a serial terminal program to the same settings as of UART1 in ESP32
4. Open the external serial interface in the terminal
5. When typing any character in the terminal you should see it echoed back
6. Verify if echo indeed comes from ESP32 by disconnecting either 'TxD' or 'RxD' pin. There should be no any echo once any pin is disconnected.

## Using a hardware flow control

This is an optional check to verify if the hardware flow control works. To set it up you need an external serial interface that has RTS and CTS signals. 

1. Connect the extra RTS/CTS signals as below

  | ESP32 Interface | #define | ESP32 Pin | External UART Pin |
  | --- | --- | --- | --- |
  | Request to Send (RTS) | ECHO_TEST_RTS | GPIO18 | CTS |
  | Clear to Send (CTS) | ECHO_TEST_CTS | GPIO19 | RTS |

2. Configure both extra pins in the example code by replacing existing `UART_PIN_NO_CHANGE` macros with the above pin numbers
3. Configure UART1 driver to use the hardware flow control by setting `.flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS` and adding `.rx_flow_ctrl_thresh = 122`
4. Repeat tests described in 'Setup' above starting from step 3

See the README.md file in the upper level 'examples' directory for more information about examples.
