# Modbus Master Example

This example demonstrates using of FreeModbus stack port implementation for ESP32 as a master device. 
This implementation is able to read/write values of slave devices connected into Modbus segment. All parameters to be accessed are defined in data dictionary in the files /main/device_params.h/c.
The values represented as characteristics with its name and characteristic CID which are linked into registers of slave devices connected into Modbus segment. 

The example implements simple control algorithm  and checks humidity and temperature from two sensors and set alarm (relay in third device) when values exceeded limits. 

Device parameters definition:
--------------------------------------------------------------------------------------------------
| Slave address       | Characteristic ID    | Characteristic name  | Description                |
| --------------------|----------------------|----------------------|----------------------------|
| MB_DEVICE_ADDR1     | CID_DATA_CHAN_0,     | Data_channel_0       | Data channel 1             |
| MB_DEVICE_ADDR1     | CID_HUMIDITY_1,      | Humidity_1           | Humidity from sensor 1     |
|                     | CID_TEMPERATURE_1    | Temperature_1        | Sensor 1 temperature       |
| MB_DEVICE_ADDR2     | CID_HUMIDITY_2,      | Humidity_2           | Humidity from sensor 2     |
|                     | CID_TEMPERATURE_2    | Temperature_2        | Sensor 2 temperature       |
| MB_DEVICE_ADDR3     | CID_RELAY_P1         | RelayP1              | Alarm Relay outputs on/off |
--------------------------------------------------------------------------------------------------

Modbus segment device connection schematic:
```
   MB_DEVICE_ADDR1
    -------------
    |           |   
    |  Slave 1  |---<>--+
    |           |       |
    -------------       |
   MB_DEVICE_ADDR2      |
    -------------       |        -------------
    |           |       |        |           |
    |  Slave  2 |---<>--+---<>---|  Master   |
    |           |       |        |           |
    -------------       |        -------------
   MB_DEVICE_ADDR3      |
    -------------  RS485 network
    |           |       |
    |  Slave 3  |---<>--+
    |           |
    -------------
```
    
## Hardware required :
Option 1:
PC (Modbus Slave app) + USB Serial adapter connected to USB port + RS485 line drivers + ESP32 WROVER-KIT board. 
Option 2:
Three ESP32 WROVER-KIT board flashed with modbus_slave example software to represent slave device with specific slave address. The slave addresses for each board have to be configured as defined in "Device parameters definition" table above.
One ESP32 WROVER-KIT board flashed with modbus_master example. All the boards require connection of RS485 line drivers (see below).

The MAX485 line driver is used as an example below but other similar chips can be used as well.
RS485 example circuit schematic for connection of master and slave devices into segment:
```
         VCC ---------------+                               +--------------- VCC
                            |                               |
                    +-------x-------+               +-------x-------+
         RXD <------| RO            | DIFFERENTIAL  |             RO|-----> RXD
                    |              B|---------------|B              |
         TXD ------>| DI   MAX485   |    \  /       |    MAX485   DI|<----- TXD
ESP32 WROVER KIT 1  |               |   RS-485 side |               |      External PC (emulator) with USB to serial or
         RTS --+--->| DE            |    /  \       |             DE|---+  ESP32 WROVER KIT 2 (slave)     
               |    |              A|---------------|A              |   |
               +----| /RE           |    PAIR       |            /RE|---+-- RTS
                    +-------x-------+               +-------x-------+
                            |                               |
                           ---                             --- 
                    Modbus Master device             Modbus Slave device
                           
```

## How to setup and use an example:

### Configure the application
Configure the UART pins used for modbus communication using command and table below.
```
idf.py menuconfig
```

```
  ------------------------------------------------------------------------------------------------
  | ESP32 Interface       | #define                      | Default ESP32 Pin | External RS485 Pin|
  | ----------------------|------------------------------|-------------------|-------------------|
  | Transmit Data (TxD)   | CONFIG_MB_UART_TXD           | GPIO23            | DI                |
  | Receive Data (RxD)    | CONFIG_MB_UART_RXD           | GPIO22            | RO                | 
  | Request To Send (RTS) | CONFIG_MB_UART_RTS           | GPIO18            | ~RE/DE            |
  |                       |                              |                   |                   |
  | Ground                | n/a                          | GND               | GND               |
  ------------------------------------------------------------------------------------------------
```
The communication parameters below allow to configure Modbus stack appropriately but usually it is enough to use default settings.
See the help string of parameters for more information.

### Setup external Modbus slave devices or emulator
Option 1:
Configure the external Modbus master software according to port configuration parameters used in the example. The Modbus Slave application can be used with this example to emulate slave devices with its parameters. Use official documentation for software to setup emulation of slave devices.
Option 2:
Other option is to have the modbus_slave example flashed into ESP32 WROVER KIT board and connect boards together as showed in "Modbus segment connection schematic above". See the modbus slave API documentation to configure communication parameters and slave addresses as defined in "Device parameters definition" table above.

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
I (51493) SENSE_MAIN: cid: 0, Data_channel_0(Volts) = 6.00
I (51543) SENSE_MAIN: cid: 1, Humidity_1(%rH) = 22.00
I (51573) SENSE_MAIN: cid: 2, Temperature_1(C) = 0.00
I (51603) SENSE_MAIN: cid: 3, Humidity_2(%rH) = 1.00
I (51633) SENSE_MAIN: cid: 4, Temperature_2(C) = 33.00
I (51673) SENSE_MAIN: cid: 5, (RelayP1) = OFF
I (61713) SENSE_MAIN: cid: 0, Data_channel_0(Volts) = 50.00
I (61763) SENSE_MAIN: cid: 1, Humidity_1(%rH) = 22.00
I (61793) SENSE_MAIN: cid: 2, Temperature_1(C) = 0.00
I (61823) SENSE_MAIN: cid: 3, Humidity_2(%rH) = 1.00
I (61853) SENSE_MAIN: cid: 4, Temperature_2(C) = 33.00
I (61893) SENSE_MAIN: cid: 5, (RelayP1) = OFF
I (62893) SENSE_MAIN: The value exceeds limit, then set relay.
I (71953) SENSE_MAIN: cid: 0, Data_channel_0(Volts) = 50.00
I (71993) SENSE_MAIN: cid: 1, Humidity_1(%rH) = 22.00
I (72023) SENSE_MAIN: cid: 2, Temperature_1(C) = 0.00
I (72063) SENSE_MAIN: cid: 3, Humidity_2(%rH) = 1.00
I (72093) SENSE_MAIN: cid: 4, Temperature_2(C) = 33.00
I (73143) SENSE_MAIN: cid: 5, (RelayP1) = ON

```
The example refreshes the characteristics from devices every 10 seconds, verifies if they exceeded limits and sets alarm accordingly. The output line describes Timestamp, Cid of characteristic, Characteristic name(Units), Characteristic value.


