# Modbus Slave Example

This example demonstrates using of FreeModbus stack port implementation for ESP32. The external Modbus host is able to read/write device parameters using Modbus protocol transport. The parameters accessible thorough Modbus are located in deviceparams.h/c files and can be updated by user. 
These are represented in structures holding_reg_params, input_reg_params, coil_reg_params, discrete_reg_params for holding registers, input parameters, coils and discrete inputs accordingly. The app_main application demonstrates how to setup Modbus stack and use notifications about parameters change from host system. 
The FreeModbus stack located in components\freemodbus\ folder and contain \port folder inside which contains FreeModbus stack port for ESP32. There are some parameters that can be configured in KConfig file to start stack correctly (See description below for more information).

The slave example uses shared parameter structures defined in examples\protocols\modbus\serial\common_components folder.

## Hardware required :
Option 1:
PC + USB Serial adapter connected to USB port + RS485 line drivers + ESP32 WROVER-KIT board. 
The MAX485 line driver is used as an example below but other similar chips can be used as well.

Option 2:
The modbus_master example application configured as described in its README.md file and flashed into ESP32 WROVER-KIT board.
Note: The ```Example Data (Object) Dictionary``` in the modbus_master example can be edited to address parameters from other slaves connected into Modbus segment.

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
Start the command below to show the configuration menu:
```
idf.py menuconfig
```
Select Modbus Example Configuration menu item.
Configure the UART pins used for modbus communication using command and table below.
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
Define the ```Modbus communiction mode``` for slave in Kconfig - CONFIG_MB_COMM_MODE (must be the same for master and slave application).
Set ```Modbus slave address``` for the example application (by default for example script is set to 1).
The communication parameters of freemodbus stack (Component config->Modbus configuration) allow to configure it appropriately but usually it is enough to use default settings.
See the help strings of parameters for more information.

### Setup external Modbus master software
Option 1:
Configure the external Modbus master software according to port configuration parameters used in application.
As an example the Modbus Poll application can be used with this example.
Option 2:
Setup ESP32 WROVER-KIT board and set modbus_master example configuration as described in its README.md file.
Setup one or more slave boards with different slave addresses and connect them into the same Modbus segment (See configuration above). 
Note: The ```Modbus communiction mode``` parameter must be the same for master and slave example application to be able to communicate with each other.

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
I (13941) SLAVE_TEST: INPUT READ (13651163 us), ADDR:1, TYPE:8, INST_ADDR:0x3ffb2fd0, SIZE:2
I (13951) SLAVE_TEST: HOLDING READ (13656431 us), ADDR:1, TYPE:2, INST_ADDR:0x3ffb2fe0, SIZE:2
I (13961) SLAVE_TEST: INPUT READ (13665877 us), ADDR:3, TYPE:8, INST_ADDR:0x3ffb2fd4, SIZE:2
I (13971) SLAVE_TEST: HOLDING READ (13676010 us), ADDR:3, TYPE:2, INST_ADDR:0x3ffb2fe4, SIZE:2
I (13981) SLAVE_TEST: INPUT READ (13686130 us), ADDR:5, TYPE:8, INST_ADDR:0x3ffb2fd8, SIZE:2
I (13991) SLAVE_TEST: HOLDING READ (13696267 us), ADDR:5, TYPE:2, INST_ADDR:0x3ffb2fe8, SIZE:2
I (14001) SLAVE_TEST: COILS READ (13706331 us), ADDR:0, TYPE:32, INST_ADDR:0x3ffb2fcc, SIZE:8
I (14001) SLAVE_TEST: Modbus controller destroyed.
```
The output lines describe type of operation, its timestamp, modbus address, access type, storage address in parameter structure and number of registers accordingly.

