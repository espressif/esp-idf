| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Modbus Slave Example

This example demonstrates using of FreeModbus TCP slave stack port implementation for supported ESP32 target chips. The external Modbus host is able to read/write device parameters using Modbus protocol transport. The parameters accessible thorough Modbus are located in `mb_example_common/modbus_params.h\c` files and can be updated by user.
These are represented in structures holding_reg_params, input_reg_params, coil_reg_params, discrete_reg_params for holding registers, input parameters, coils and discrete inputs accordingly. The app_main application demonstrates how to setup Modbus stack and use notifications about parameters change from host system.
The FreeModbus stack located in `components/freemodbus` folder and contain `/port` folder inside which contains FreeModbus stack port for ESP32 target chips. There are some parameters that can be configured in KConfig file to start stack correctly (See description below for more information).

The slave example uses shared parameter structures defined in ```examples/protocols/modbus/mb_example_common``` folder.

## Hardware required :
Option 1:
The ESP32 based development board flashed with modbus_tcp_slave example + external Modbus master host software.

Option 2:
The modbus_tcp_master example application configured as described in its README.md file and flashed into ESP32 based board.
Note: The ```Example Data (Object) Dictionary``` in the modbus_tcp_master example can be edited to address parameters from other slaves connected into Modbus segment.

## How to setup and use an example:

### Configure the application
Start the command below to show the configuration menu:
```
idf.py menuconfig
```

To configure the example to use Wi-Fi or Ethernet connection, open the project configuration menu and navigate to "Example Connection Configuration" menu. Select either "Wi-Fi" or "Ethernet" in the "Connect using" choice.
Follow the instructions in `examples/common_components/protocol_examples_common` for further configuration.

The communication parameters of freemodbus stack (Component config->Modbus configuration) allow to configure it appropriately but usually it is enough to use default settings.
See the help strings of parameters for more information.

### Setup external Modbus master software
Option 1:
Configure the external Modbus master software according to port configuration parameters used in application.
As an example the Modbus Poll application can be used with this example.
Option 2:
Setup ESP32 based development board and set modbus_tcp_master example configuration as described in its README.md file.
Setup one or more slave boards and connect them into the same Modbus segment (See configuration above).

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
I (4235) esp_netif_handlers: example_connect: sta ip: 192.168.1.21, mask: 255.255.255.0, gw: 192.168.1.1
I (4235) example_connect: Got IPv4 event: Interface "example_connect: sta" address: 192.168.1.21
I (4465) example_connect: Got IPv6 event: Interface "example_connect: sta" address: fe80:0000:0000:0000:7edf:a1ff:fe00:4039, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (4465) example_connect: Connected to example_connect: sta
I (4475) example_connect: - IPv4 address: 192.168.1.21
I (4475) example_connect: - IPv6 address: fe80:0000:0000:0000:7edf:a1ff:fe00:4039, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (4495) MB_TCP_SLAVE_PORT: Socket (#54), listener  on port: 502, errno=0
I (4495) MB_TCP_SLAVE_PORT: Protocol stack initialized.
I (4505) SLAVE_TEST: Modbus slave stack initialized.
I (4505) SLAVE_TEST: Start modbus test...
I (41035) MB_TCP_SLAVE_PORT: Socket (#55), accept client connection from address: 192.168.1.39
I (41225) SLAVE_TEST: INPUT READ (41704766 us), ADDR:1, TYPE:8, INST_ADDR:0x3ffcb878, SIZE:2
I (41235) SLAVE_TEST: HOLDING READ (41719746 us), ADDR:1, TYPE:2, INST_ADDR:0x3ffcb9b4, SIZE:2
I (41255) SLAVE_TEST: INPUT READ (41732965 us), ADDR:3, TYPE:8, INST_ADDR:0x3ffcb87c, SIZE:2
I (41265) SLAVE_TEST: HOLDING READ (41745923 us), ADDR:3, TYPE:2, INST_ADDR:0x3ffcb9b8, SIZE:2
I (41275) SLAVE_TEST: INPUT READ (41759563 us), ADDR:5, TYPE:8, INST_ADDR:0x3ffcb880, SIZE:2
I (41295) SLAVE_TEST: HOLDING READ (41772568 us), ADDR:5, TYPE:2, INST_ADDR:0x3ffcb9bc, SIZE:2
I (41305) SLAVE_TEST: COILS WRITE (41785889 us), ADDR:0, TYPE:16, INST_ADDR:0x3ffcb874, SIZE:8
I (41315) SLAVE_TEST: COILS WRITE (41799175 us), ADDR:8, TYPE:16, INST_ADDR:0x3ffcb875, SIZE:8
I (41945) SLAVE_TEST: INPUT READ (42421629 us), ADDR:1, TYPE:8, INST_ADDR:0x3ffcb878, SIZE:2
I (42145) SLAVE_TEST: HOLDING READ (42626497 us), ADDR:1, TYPE:2, INST_ADDR:0x3ffcb9b4, SIZE:2
I (42345) SLAVE_TEST: INPUT READ (42831315 us), ADDR:3, TYPE:8, INST_ADDR:0x3ffcb87c, SIZE:2
I (42555) SLAVE_TEST: HOLDING READ (43036111 us), ADDR:3, TYPE:2, INST_ADDR:0x3ffcb9b8, SIZE:2
I (42755) SLAVE_TEST: INPUT READ (43240950 us), ADDR:5, TYPE:8, INST_ADDR:0x3ffcb880, SIZE:2
I (42865) SLAVE_TEST: HOLDING READ (43343204 us), ADDR:5, TYPE:2, INST_ADDR:0x3ffcb9bc, SIZE:2
......
I (81265) SLAVE_TEST: HOLDING READ (81743698 us), ADDR:5, TYPE:2, INST_ADDR:0x3ffcb9bc, SIZE:2
I (81465) SLAVE_TEST: COILS WRITE (81948482 us), ADDR:0, TYPE:16, INST_ADDR:0x3ffcb874, SIZE:8
I (81465) SLAVE_TEST: Modbus controller destroyed.
```
The output lines describe type of operation, its timestamp, modbus address, access type, storage address in parameter structure and number of registers accordingly.

