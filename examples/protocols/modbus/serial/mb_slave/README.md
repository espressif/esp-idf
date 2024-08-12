| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Modbus Slave Example

This example demonstrates using the port of the FreeModbus stack on an ESP32 target where the ESP32 target is operating as a network slave. The example allows an external Modbus host to read/write device parameters on the ESP32 target using the Modbus protocol. The parameters accessible through Modbus are located in `mb_example_common/modbus_params.h\c` source/header files that users can update to add/remove their own custom parameters.
These are represented in structures `holding_reg_params`, `input_reg_params`, `coil_reg_params`, `discrete_reg_params` for holding registers, input parameters, coils and discrete inputs accordingly. The app_main application demonstrates how to setup Modbus stack and use notifications about parameters change from host system.
The FreeModbus stack located in `components/freemodbus` folder and contains the `/port` folder where the stack's port to the ESP32 is situated. There are some parameters of the port that can be configured in KConfig file to start stack correctly (See description below for more information).

The slave example uses shared parameter structures defined in `examples/protocols/modbus/mb_example_common` folder.

## Hardware required :
Option 1:
PC + USB Serial adapter connected to USB port + RS485 line drivers + ESP32 based board.
The MAX485 line driver is used as an example below but other similar chips can be used as well.

Option 2:
The modbus_master example application configured as described in its README.md file and flashed into ESP32 based board.
Note: The ```Example Data (Object) Dictionary``` in the modbus_master example can be edited to address parameters from other slaves connected into Modbus segment.

RS485 example circuit schematic:
```
         VCC ---------------+                               +--------------- VCC
                            |                               |
                    +-------x-------+               +-------x-------+
         RXD <------| RO            | DIFFERENTIAL  |             RO|-----> RXD
                    |              B|---------------|B              |
         TXD ------>| DI   MAX485   |    \  /       |    MAX485   DI|<----- TXD
ESP32 board         |               |   RS-485 side |               |    Modbus master
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
Configure the UART pins used for modbus communication using the command and table below.
```
  ------------------------------------------------------------------------------------------------------------------------------
  |  UART Interface       | #define            | Default pins for      | Default pins for ESP32-S2 | External RS485 Driver Pin |
  |                       |                    | ESP32 (C6, P4)        | (S3, C3, C2, C5, H2)      |                           |
  | ----------------------|--------------------|-----------------------|---------------------------|---------------------------|
  | Transmit Data (TxD)   | CONFIG_MB_UART_TXD | GPIO23                | GPIO9                     | DI                        |
  | Receive Data (RxD)    | CONFIG_MB_UART_RXD | GPIO22                | GPIO8                     | RO                        |
  | Request To Send (RTS) | CONFIG_MB_UART_RTS | GPIO18                | GPIO10                    | ~RE/DE                    |
  | Ground                | n/a                | GND                   | GND                       | GND                       |
  ------------------------------------------------------------------------------------------------------------------------------
```
Note: Each target chip has different GPIO pins available for UART connection. Please refer to UART documentation for selected target for more information.

Define the ```Modbus communiction mode``` for slave in Kconfig - CONFIG_MB_COMM_MODE (must be the same for master and slave application).
Set ```Modbus slave address``` for the example application (by default for example script is set to 1).
The communication parameters of freemodbus stack (Component config->Modbus configuration) allow to configure it appropriately but usually it is enough to use default settings.
See the help strings of parameters for more information.

### Setup external Modbus master software
Option 1:
Configure the external Modbus master software according to port configuration parameters used in application.
As an example the Modbus Poll application can be used with this example.
Option 2:
Setup ESP32 based board and set modbus_master example configuration as described in its README.md file.
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

