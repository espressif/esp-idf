| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Modbus TCP Master Example

This example demonstrates using of FreeModbus stack port implementation for ESP32 targets as a TCP master device.
This implementation is able to read/write values of slave devices connected into Modbus segment. All parameters to be accessed are defined in data dictionary of the modbus master example source file.
The values represented as characteristics with its name and characteristic CID which are linked into registers of slave devices connected into Modbus segment.
The example implements simple control algorithm and checks parameters from slave device and gets alarm (relay in the slave device) when value of parameter exceeded limit.
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
Note: The Slave Address is the same for all parameters for example test but it can be changed in the `Example Data (Object) Dictionary` table of master example to address parameters from other slaves.
The Kconfig ```Modbus slave address``` - CONFIG_MB_SLAVE_ADDR parameter in slave example can be configured to create Modbus multi slave segment.

Simplified Modbus connection schematic for example test:
 ```
    MB_DEVICE_ADDR1
    -------------                -------------
    |           |     Network    |           |
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
    -------------     Network (Ethernet or WiFi connection)
    |           |       |
    |  Slave 3  |---<>--+
    |           |
    -------------
```

## Hardware required :
Option 1:
PC (Modbus TCP Slave application) + ESP32 based development board with modbus_tcp_slave example.

Option 2:
Several ESP32 based boards flashed with modbus_tcp_slave example software to represent slave devices. The IP slave addresses for each board have to be configured in `Modbus Example Configuration` menu according to the communication table of example.
One ESP32 based development board should be flashed with modbus_master example and connected to the same network. All the boards require configuration of network settings as described in `examples/common_components/protocol_examples_common`.

## How to setup and use an example:

### Configure the application
Start the command below to setup configuration:
```
idf.py menuconfig
```

The communication parameters of Modbus stack allow to configure it appropriately but usually it is enough to use default settings.
See the help string of parameters for more information.
There are three ways to configure how the master example will obtain slave IP addresses in the network:
* Enable CONFIG_MB_MDNS_IP_RESOLVER option allows to query for modbus services provided by Modbus slaves in the network and automatically configure IP table. This requires to activate the same option for each slave with unique modbus slave address configured in `Modbus Example Configuration` menu.
* Enable CONFIG_MB_SLAVE_IP_FROM_STDIN option to define IP addresses of slaves manually. In order to enter the IP addresses wait for the prompt and type the string with IP address following format. Prompt: "Waiting IPN from stdin:", then enter the IP address of the slave to connect: "IPN=192.168.1.21", where N = (configured slave address - 1).
* Configure slave addresses manually as below:
```
char* slave_ip_address_table[MB_DEVICE_COUNT] = {
    "192.168.1.21",     // Address corresponds to MB_DEVICE_ADDR1 and set to predefined value by user
    "192.168.1.22",     // Address corresponds to MB_DEVICE_ADDR2 of slave device in the Modbus data dictionary
    NULL                // Marker of end of list
    };
```

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
I (4644) esp_netif_handlers: example_connect: sta ip: 192.168.1.39, mask: 255.255.255.0, gw: 192.168.1.1
I (4644) example_connect: Got IPv4 event: Interface "example_connect: sta" address: 192.168.1.39
I (5644) example_connect: Got IPv6 event: Interface "example_connect: sta" address: fe80:0000:0000:0000:bedd:c2ff:fed1:b210, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5644) example_connect: Connected to example_connect: sta
I (5654) example_connect: - IPv4 address: 192.168.1.39
I (5664) example_connect: - IPv6 address: fe80:0000:0000:0000:bedd:c2ff:fed1:b210, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5674) uart: ESP_INTR_FLAG_IRAM flag not set while CONFIG_UART_ISR_IN_IRAM is enabled, flag updated
I (5684) MASTER_TEST: Leave IP(0) = [192.168.1.21] set by user.
I (5694) MASTER_TEST: IP(1) is not set in the table.
I (5694) MASTER_TEST: Configured 1 IP address(es).
I (5704) MASTER_TEST: Modbus master stack initialized...
I (5704) MB_TCP_MASTER_PORT: TCP master stack initialized.
I (5724) MB_TCP_MASTER_PORT: Host[IP]: "192.168.1.21"[192.168.1.21]
I (5724) MB_TCP_MASTER_PORT: Add slave IP: 192.168.1.21
I (5734) MB_TCP_MASTER_PORT: Connecting to slaves...
-.-.-.I (5844) MB_TCP_MASTER_PORT: Connected 1 slaves, start polling...
I (6004) MASTER_TEST: Start modbus test...
I (6044) MASTER_TEST: Characteristic #0 Data_channel_0 (Volts) value = 1.120000 (0x3f8f5c29) read successful.
I (6054) MASTER_TEST: Characteristic #1 Humidity_1 (%rH) value = 1.340000 (0x3fab851f) read successful.
I (6074) MASTER_TEST: Characteristic #2 Temperature_1 (C) value = 2.340000 (0x4015c28f) read successful.
I (6084) MASTER_TEST: Characteristic #3 Humidity_2 (%rH) value = 2.560000 (0x4023d70a) read successful.
I (6094) MASTER_TEST: Characteristic #4 Temperature_2 (C) value = 3.560000 (0x4063d70a) read successful.
I (6104) MASTER_TEST: Characteristic #5 Humidity_3 (%rH) value = 3.780000 (0x4071eb85) read successful.
I (6124) MASTER_TEST: Characteristic #6 RelayP1 (on/off) value = OFF (0x55) read successful.
I (6134) MASTER_TEST: Characteristic #7 RelayP2 (on/off) value = OFF (0xaa) read successful.
I (6854) MASTER_TEST: Characteristic #0 Data_channel_0 (Volts) value = 1.120000 (0x3f8f5c29) read successful.
I (7064) MASTER_TEST: Characteristic #1 Humidity_1 (%rH) value = 1.740000 (0x3fdeb852) read successful.
I (7264) MASTER_TEST: Characteristic #2 Temperature_1 (C) value = 2.340000 (0x4015c28f) read successful.
...
I (45974) MASTER_TEST: Characteristic #4 Temperature_2 (C) value = 3.560000 (0x4063d70a) read successful.
I (46174) MASTER_TEST: Characteristic #5 Humidity_3 (%rH) value = 3.780000 (0x4071eb85) read successful.
I (46384) MASTER_TEST: Characteristic #6 RelayP1 (on/off) value = OFF (0x55) read successful.
I (46584) MASTER_TEST: Characteristic #7 RelayP2 (on/off) value = ON (0xff) read successful.
I (47094) MASTER_TEST: Alarm triggered by cid #7.
I (47094) MASTER_TEST: Destroy master...
```
The example reads the characteristics from slave device(s), while alarm is not triggered in the slave device (See the "Example parameters definition"). The output line describes Timestamp, Cid of characteristic, Characteristic name (Units), Characteristic value (Hex data).
