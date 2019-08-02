# Modbus Slave Example

This example demonstrates using of FreeModbus stack port implementation for ESP32. The external Modbus host is able to read/write device parameters using Modbus protocol transport. The parameters accessible thorough Modbus are located in deviceparams.h/c files and can be updated by user. 
These are represented in structures holding_reg_params, input_reg_params, coil_reg_params, discrete_reg_params for holding registers, input parameters, coils and discrete inputs accordingly. The app_main application demonstrates how to setup Modbus stack and use notifications about parameters change from host system. 
The FreeModbus stack located in components\freemodbus\ folder and contain \port folder inside which contains FreeModbus stack port for ESP32. There are some parameters that can be configured in KConfig file to start stack correctly (See description below for more information).

## Hardware required :
PC + USB Serial adapter connected to USB port + RS485 line drivers + ESP32 WROVER-KIT board. 
The MAX485 line driver is used as an example below but other similar chips can be used as well.

RS485 example circuit schematic:
```
         VCC ---------------+                               +--------------- VCC
                            |                               |
                    +-------x-------+               +-------x-------+
         RXD <------| RO            | DIFFERENTIAL  |             RO|-----> RXD
                    |              B|---------------|B              |
         TXD ------>| DI   MAX485   |    \  /       |    MAX485   DI|<----- TXD
ESP32 WROVER KIT 1  |               |   RS-485 side |               |    Modbus master
         RTS --+--->| DE            |    /  \       |             DE|---+       
               |    |              A|---------------|A              |   |
               +----| /RE           |    PAIR       |            /RE|---+-- RTS
                    +-------x--------+              +-------x-------+
                            |                               |
                           ---                             --- 
```

## How to setup and use an example:

### Configure the application
Configure the UART pins used for modbus communication using command and table below.
```
idf.py menuconfig
```

```
  -----------------------------------------------------------------------------------
  | ESP32 Interface       | #define            | Default ESP32 Pin | External RS485 |
  | ----------------------|--------------------|-------------------| Driver Pin     |
  | Transmit Data (TxD)   | CONFIG_MB_UART_TXD | GPIO23            | DI             |
  | Receive Data (RxD)    | CONFIG_MB_UART_RXD | GPIO22            | RO             |
  | Request To Send (RTS) | CONFIG_MB_UART_RTS | GPIO18            | ~RE/DE         |
  | Ground                | n/a                | GND               | GND            |
  -----------------------------------------------------------------------------------
```
The communication parameters below allow to configure freemodbus stack appropriately but usually it is enough to use default settings.
See the help string of parameters for more information.

### Setup external Modbus master software
Configure the external Modbus master software according to port configuration parameters used in application.
As an example the Modbus Poll application can be used with this example.

### Build and flash software
Build the project and flash it to the board, then run monitor tool to view serial output:
```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output
Example output of the application:
```
INPUT READ: time_stamp(us):565240387, mb_addr:1, type:8, st_address:0x3ffb385c, size:8

HOLDING READ/WRITE: time_stamp(us):12104081, mb_addr:1, type:2, st_address:0x3ffb386c, size:8
```
The output lines describe type of operation, its timestamp, modbus address, access type, storage address in parameter structure and number of registers accordingly.



