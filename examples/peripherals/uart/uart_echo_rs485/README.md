# UART RS485 Echo Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This is an example which echoes any data it receives on UART2 back to the sender in the RS485 network.
It uses ESP-IDF UART software driver in RS485 half duplex transmission mode and requires external connection of bus drivers.
The approach demonstrated in this example can be used in user application to transmit/receive data in RS485 networks.

## How to use example

### Hardware Required
PC + USB Serial adapter connected to USB port + RS485 line drivers + ESP32, ESP32-S or ESP32-C series based board.
The MAX485 line driver is used for example below but other similar chips can be used as well.

#### RS485 example connection circuit schematic:
```
         VCC ---------------+                               +--------------- VCC
                            |                               |
                    +-------x-------+               +-------x-------+
         RXD <------| RO            |               |             RO|-----> RXD
                    |              B|---------------|B              |
         TXD ------>| DI  MAX485    |    \  /       |    MAX485   DI|<----- TXD
ESP dev kit         |               |   RS-485 side |               |  SERIAL ADAPTER SIDE
         RTS --+--->| DE            |    /  \       |             DE|---+
               |    |              A|---------------|A              |   |
               +----| /RE           |               |            /RE|---+-- RTS
                    +-------x-------+               +-------x-------+
                            |                               |
                           ---                             ---
```

#### Connect an external RS485 serial interface to an ESP board
Connect USB to RS485 adapter to computer and connect its D+, D- output lines with the D+, D- lines of RS485 line driver connected to the ESP board (See picture above). To view or adjust default pins please see the `Echo RS485 Example Configuration` submenu in `idf.py menuconfig`.
```
  --------------------------------------------------------------------------------------------------
  | ESP Interface         | #define            | Default ESP Pin       | External RS485 Driver Pin |
  | ----------------------|--------------------|-----------------------|---------------------------|
  | Transmit Data (TxD)   | CONFIG_MB_UART_TXD | CONFIG_ECHO_UART_TXD  | DI                        |
  | Receive Data (RxD)    | CONFIG_MB_UART_RXD | CONFIG_ECHO_UART_RXD  | RO                        |
  | Request To Send (RTS) | CONFIG_MB_UART_RTS | CONFIG_ECHO_UART_RTS  | ~RE/DE                    |
  | Ground                | n/a                | GND                   | GND                       |
  --------------------------------------------------------------------------------------------------
```
Note: Some GPIOs can not be used with some chip because they are used for flash chip connection. Please refer to UART documentation for selected target.

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

#### Setup external terminal software
Refer to the example and set up a serial terminal program to the same settings as of UART in ESP32-WROVER-KIT board.
Open the external serial interface in the terminal. By default if no any symbols are received, the application sends character `.` to check transmission side.
When typing message and push send button in the terminal you should see the message `RS485 Received: [ your message ]`, where "your message" is the message you sent from terminal.
Verify if echo indeed comes from your board by disconnecting either `TxD` or `RxD` pin. Once done there should be no any `.` displayed.

## Example Output
Example output of the application:
```
I (655020) RS485_ECHO_APP: Received 12 bytes:
[ 0x79 0x6F 0x75 0x72 0x20 0x6D 0x65 0x73 0x73 0x61 0x67 0x65 ]
```
The received message is showed in hexadecimal form in the brackets.

## Troubleshooting
When example software does not show the `.` symbol, the issue is most likely related to connection errors of the external RS485 interface.
Check the RS485 interface connection with the environment according to schematic above and restart the application.
Then start terminal software and open the appropriate serial port.

