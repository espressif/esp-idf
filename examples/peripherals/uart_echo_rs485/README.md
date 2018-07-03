# UART RS485 Echo Example

This is an example which echoes any data it receives on UART2 back to the sender in the RS485 network. 
It uses ESP-IDF UART software driver in RS485 half duplex transmission mode and requires external connection of bus drivers.
The approach demonstrated in this example can be used in user application to transmit/receive data in RS485 networks.

## Hardware required :
PC + USB Serial adapter connected to USB port + RS485 line drivers + ESP32 WROVER-KIT board. 
The MAX485 line driver is used for example below but other similar chips can be used as well.

### RS485 example connection circuit schematic:
```
         VCC ---------------+                               +--------------- VCC
                            |                               |
                    +-------x-------+               +-------x-------+
         RXD <------| RO            |               |             RO|-----> RXD
                    |              B|---------------|B              |
         TXD ------>| DI  MAX485    |    \  /       |    MAX485   DI|<----- TXD
ESP32 WROVER-KIT    |               |   RS-485 side |               |  SERIAL ADAPTER SIDE
         RTS --+--->| DE            |    /  \       |             DE|---+
               |    |              A|---------------|A              |   |
               +----| /RE           |               |            /RE|---+-- RTS
                    +-------x-------+               +-------x-------+
                            |                               |
                           ---                             --- 
```         

## How to setup and use an example:

### Connect an external RS485 serial interface to an ESP32 board.
```
  ----------------------------------------------------------------------
  | ESP32 Interface       | #define       | ESP32 Pin | External RS485 |
  | ----------------------|---------------|-----------| Driver Pin     |
  | Transmit Data (TxD)   | ECHO_TEST_TXD | GPIO23    | DI             |
  | Receive Data (RxD)    | ECHO_TEST_RXD | GPIO22    | RO             |
  | Request To Send (RTS) | ECHO_TEST_RTS | GPIO18    | ~RE/DE         |
  | Ground                | n/a           | GND       | GND            |
  ----------------------------------------------------------------------
```
Connect USB to RS485 adapter to computer and connect its D+, D- output lines with the D+, D- lines of RS485 line driver connected to ESP32 (See picture above).

### Configure the application
```
make menuconfig
```
* Set serial port under Serial Flasher Options to the serial port of ESP32 WROVER-KIT board.

### Build and flash software
Build the project and flash it to the board, then run monitor tool to view serial output:
```
make -j4 flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

### Setup external terminal software 
Refer to the example and set up a serial terminal program to the same settings as of UART in ESP32 WROVER-KIT BOARD.
Open the external serial interface in the terminal. By default if no any symbols received the application sends character "." to check transmission side. 
When typing message and push send button in the terminal you should see the message "RS485 Received: [ your message ], where your message is the message you sent from terminal.
Verify if echo indeed comes from ESP32 by disconnecting either 'TxD' or 'RxD' pin. There should be no any "." once TxD pin is disconnected.

## Example Output
Example output of the application:
```
I (655020) RS485_ECHO_APP: Received 12 bytes:
[ 0x79 0x6F 0x75 0x72 0x20 0x6D 0x65 0x73 0x73 0x61 0x67 0x65 ]
```
The received message is showed in HEX form in the brackets.

## Troubleshooting
Most of the issues when example software does not show the '.' symbol related to connection errors of external RS485 interface. 
Check the RS485 interface connection with the environment according to schematic above and restart the application. 
Then start terminal software and open appropriate serial port.

