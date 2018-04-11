# UART RS485 Echo Example

This is an example which echoes any data it receives on UART2 back to the sender.

## Setup
This example uses external RS485 interface. The MAX485 line driver can be used for example.

RS485 example connection circuit schematic:

         VCC ---------------+                               +--------------- VCC
                            |                               |
                    +-------x-------+               +-------x-------+
         RXD <------| RO            |               |             RO|-----> RXD
                    |              B|---------------|B              |
         TXD ------>| DI  MAX485    |    \  /       |    MAX485   DI|<----- TXD
ESP32 WROVER KIT    |               |   RS-485 side |               |  SERIAL ADAPTER
         RTS --+--->| DE            |    /  \       |             DE|---+
               |    |              A|---------------|A              |   |
               +----| /RE           |               |            /RE|---+-- RTS
                    +-------x-------+               +-------x-------+
                            |                               |
                           ---                             --- 

1. Connect an external RS485 serial interface to an ESP32 board. 
  ----------------------------------------------------------------------
  | ESP32 Interface       | #define       | ESP32 Pin | External RS485 |
  | ----------------------|---------------|-----------| Driver Pin     |
  | Transmit Data (TxD)   | ECHO_TEST_TXD | GPIO23    | DI             |
  | Receive Data (RxD)    | ECHO_TEST_RXD | GPIO22    | RO             |
  | Request To Send (RTS) | ECHO_TEST_RTS | GPIO18    | ~RE/DE         |
  | Ground                | n/a           | GND       | GND            |
  ----------------------------------------------------------------------
2. Connect USB to RS485 adapter to computer and connect its D+, D- output lines with the D+, D- lines of RS485 driver connected to ESP32.
3. Compile and load the example to the ESP32 board
4. Refer to the example and set up a serial terminal program to the same settings as of UART in ESP32
5. Open the external serial interface in the terminal.
6. By default if no any symbols received the application sends character "." to check transmission side. When typing message and push send button in the terminal you should see the message "RS485 Received: [ your message ], where your message is the message you sent from terminal.
7. Verify if echo indeed comes from ESP32 by disconnecting either 'TxD' or 'RxD' pin. There should be no any "." once TxD pin is disconnected.

