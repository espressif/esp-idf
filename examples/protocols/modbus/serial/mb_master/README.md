| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Modbus Master Example

This example demonstrates using of FreeModbus stack port implementation for ESP32 as a master device. 
This implementation is able to read/write values of slave devices connected into Modbus segment. All parameters to be accessed are defined in data dictionary of the modbus master example source file.
The values represented as characteristics with its name and characteristic CID which are linked into registers of slave devices connected into Modbus segment. 
The example implements simple control algorithm and checks parameters from slave device and gets alarm (relay in the slave device) when value of holding_data0 parameter exceeded limit.
The instances for the modbus parameters are common for master and slave examples and located in `examples/protocols/modbus/mb_example_common` folder.

Example parameters definition:
--------------------------------------------------------------------------------------------------
| Slave Address       | Characteristic ID    | Characteristic name  | Description                |
|---------------------|----------------------|----------------------|----------------------------|
| MB_DEVICE_ADDR1     | CID_INP_DATA_0,      | Data_channel_0       | Data channel 1             |
| MB_DEVICE_ADDR1     | CID_HOLD_DATA_0,     | Humidity_1           | Humidity 1                 |
| MB_DEVICE_ADDR1     | CID_INP_DATA_1       | Temperature_1        | Sensor temperature         |
| MB_DEVICE_ADDR1     | CID_HOLD_DATA_1,     | Humidity_2           | Humidity 2                 |
| MB_DEVICE_ADDR1     | CID_INP_DATA_2       | Temperature_2        | Ambient temperature        |
| MB_DEVICE_ADDR1     | CID_HOLD_DATA_2      | Humidity_3           | Humidity 3                 |
| MB_DEVICE_ADDR1     | CID_RELAY_P1         | RelayP1              | Alarm Relay outputs on/off |
| MB_DEVICE_ADDR1     | CID_RELAY_P2         | RelayP2              | Alarm Relay outputs on/off |
--------------------------------------------------------------------------------------------------
Note: The Slave Address is the same for all parameters for example test but it can be changed in the ```Example Data (Object) Dictionary``` table of master example to address parameters from other slaves.
The Kconfig ```Modbus slave address``` - CONFIG_MB_SLAVE_ADDR parameter in slave example can be configured to create Modbus multi slave segment.

Simplified Modbus connection schematic for example test:
 ```
    MB_DEVICE_ADDR1
    -------------                -------------
    |           | RS485 network  |           |
    |  Slave  1 |---<>--+---<>---|  Master   |
    |           |                |           |
    -------------                -------------
```
Modbus multi slave segment connection schematic:
```
    MB_DEVICE_ADDR1
    -------------
    |           |   
    |  Slave 1  |---<>--+
    |           |       |
    -------------       |
    MB_DEVICE_ADDR2     |
    -------------       |        -------------
    |           |       |        |           |
    |  Slave  2 |---<>--+---<>---|  Master   |
    |           |       |        |           |
    -------------       |        -------------
    MB_DEVICE_ADDR3     |
    -------------  RS485 network
    |           |       |
    |  Slave 3  |---<>--+
    |           |
    -------------
```

## Hardware required :
Option 1:
PC (Modbus Slave app) + USB Serial adapter connected to USB port + RS485 line drivers + ESP32 based board  

Option 2:
Several ESP32 boards flashed with modbus_slave example software to represent slave device with specific slave address (See CONFIG_MB_SLAVE_ADDR). The slave addresses for each board have to be configured as defined in "connection schematic" above.
One ESP32 board flashed with modbus_master example. All the boards require connection of RS485 line drivers (see below).

The MAX485 line driver is used as an example below but other similar chips can be used as well.
RS485 example circuit schematic for connection of master and slave devices into segment:
```
         VCC ---------------+                               +--------------- VCC
                            |                               |
                    +-------x-------+               +-------x-------+
         RXD <------| RO            | DIFFERENTIAL  |             RO|-----> RXD
                    |              B|---------------|B              |
         TXD ------>| DI   MAX485   |    \  /       |    MAX485   DI|<----- TXD
ESP32 BOARD         |               |   RS-485 side |               |      External PC (emulator) with USB to serial or
         RTS --+--->| DE            |    /  \       |             DE|---+  ESP32 BOARD (slave)
               |    |              A|---------------|A              |   |
               +----| /RE           |    PAIR       |            /RE|---+-- RTS
                    +-------x-------+               +-------x-------+
                            |                               |
                           ---                             --- 
                    Modbus Master device             Modbus Slave device
                           
```

## How to setup and use an example:

### Configure the application
Start the command below to setup configuration:
```
idf.py menuconfig
```
Configure the UART pins used for modbus communication using and table below.
Define the communication mode parameter for master and slave in Kconfig - CONFIG_MB_COMM_MODE (must be the same for master and slave devices in one segment).
Configure the slave address for each slave in the Modbus segment (the CONFIG_MB_SLAVE_ADDR in Kconfig).
```
  ------------------------------------------------------------------------------------------------------------------------------
  |  UART Interface       | #define            | Default pins for      | Default pins for          | External RS485 Driver Pin |
  |                       |                    | ESP32 (C6)            | ESP32-S2 (S3, C3, C2, H2) |                           |
  | ----------------------|--------------------|-----------------------|---------------------------|---------------------------|
  | Transmit Data (TxD)   | CONFIG_MB_UART_TXD | GPIO23                | GPIO9                     | DI                        |
  | Receive Data (RxD)    | CONFIG_MB_UART_RXD | GPIO22                | GPIO8                     | RO                        |
  | Request To Send (RTS) | CONFIG_MB_UART_RTS | GPIO18                | GPIO10                    | ~RE/DE                    |
  | Ground                | n/a                | GND                   | GND                       | GND                       |
  ------------------------------------------------------------------------------------------------------------------------------
```
Note: Each target chip has different GPIO pins available for UART connection. Please refer to UART documentation for selected target for more information.

Connect a USB-to-RS485 adapter to a computer, then connect the adapter's A/B output lines with the corresponding A/B output lines of the RS485 line driver connected to the ESP32 chip (see figure above).

The communication parameters of Modbus stack allow to configure it appropriately but usually it is enough to use default settings.
See the help string of parameters for more information.

### Setup external Modbus slave devices or emulator
Option 1:
Configure the external Modbus master software according to port configuration parameters used in the example. The Modbus Slave application can be used with this example to emulate slave devices with its parameters. Use official documentation for software to setup emulation of slave devices.

Option 2:
Other option is to have the modbus_slave example application flashed into ESP32 based board and connect boards together as showed on the Modbus connection schematic above. See the Modbus slave API documentation to configure communication parameters and slave addresses as defined in "Example parameters definition" table above.

### Build and flash software of master device
Build the project and flash it to the board, then run monitor tool to view serial output:
```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output
Example output of the application:
```
I (9035) MASTER_TEST: Characteristic #0 Data_channel_0 (Volts) value = 1.120000 (0x3f8f5c29) read successful.
I (9045) MASTER_TEST: Characteristic #1 Humidity_1 (%rH) value = 5.539999 (0x40b147ac) read successful.
I (9045) MASTER_TEST: Characteristic #2 Temperature_1 (C) value = 2.340000 (0x4015c28f) read successful.
I (9055) MASTER_TEST: Characteristic #3 Humidity_2 (%rH) value = 2.560000 (0x4023d70a) read successful.
I (9065) MASTER_TEST: Characteristic #4 Temperature_2 (C) value = 3.560000 (0x4063d70a) read successful.
I (9075) MASTER_TEST: Characteristic #5 Humidity_3 (%rH) value = 3.780000 (0x4071eb85) read successful.
I (9085) MASTER_TEST: Characteristic #6 RelayP1 (on/off) value = OFF (0x55) read successful.
I (9095) MASTER_TEST: Characteristic #7 RelayP2 (on/off) value = OFF (0xaa) read successful.
I (9605) MASTER_TEST: Characteristic #0 Data_channel_0 (Volts) value = 1.120000 (0x3f8f5c29) read successful.
I (9615) MASTER_TEST: Characteristic #1 Humidity_1 (%rH) value = 5.739999 (0x40b7ae12) read successful.
I (9615) MASTER_TEST: Characteristic #2 Temperature_1 (C) value = 2.340000 (0x4015c28f) read successful.
I (9625) MASTER_TEST: Characteristic #3 Humidity_2 (%rH) value = 2.560000 (0x4023d70a) read successful.
I (9635) MASTER_TEST: Characteristic #4 Temperature_2 (C) value = 3.560000 (0x4063d70a) read successful.
I (9645) MASTER_TEST: Characteristic #5 Humidity_3 (%rH) value = 3.780000 (0x4071eb85) read successful.
I (9655) MASTER_TEST: Characteristic #6 RelayP1 (on/off) value = OFF (0x55) read successful.
I (9665) MASTER_TEST: Characteristic #7 RelayP2 (on/off) value = ON (0xff) read successful.
I (10175) MASTER_TEST: Alarm triggered by cid #7.
I (10175) MASTER_TEST: Destroy master...

```
The example reads the characteristics from slave device(s), while alarm is not triggered in the slave device (See the "Example parameters definition"). The output line describes Timestamp, Cid of characteristic, Characteristic name (Units), Characteristic value (Hex).

