# RS485 Half duplex UART Echo Example

This is an example which echoes any data it receives on UART1 back to the sender.

## Setup

1. Connect an external serial RS485 transceiver (eg SparkFun Transceiver RS485 Breakout or XY-017 RS485 to TTL Module)
   to an ESP32 board. Note the RTS pin is optional depending on whether the transceiver requires data direction control or not.
   If using RTS ensure logic levels are correct way round for TX/RX.
   The external interface needs to be 3.3V compatible. Default baud is 115200 and parity is even.

  | ESP32 Interface | #define | ESP32 Pin | External UART Pin |
  | --- | --- | --- | --- |
  | Transmit Data (TxD) | ECHO_TEST_TXD | GPIO4 | RxD |
  | Receive Data (RxD) | ECHO_TEST_RXD | GPIO5 | TxD |
  | RTS (RTS) | ECHO_TEST_RTS | GPIO18 | RTS or direction control pin|
  | Ground | n/a | GND | GND |

2. Ensure baud rate and parity are correct, amend code as required.
3. Compile and load the example to the ESP32 board.
5. From a another node on the RS485 bus set up a serial terminal program to the same settings as of UART1 in ESP32
6. Open the external serial interface in the terminal
7. When typing any character in the terminal you should see it echoed back

See the README.md file in the upper level 'examples' directory for more information about examples.
