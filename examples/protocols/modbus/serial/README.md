# Modbus Master-Slave Example

## Overview

These two projects illustrate the communication between Modbus master and slave device in the segment.
Master initializes Modbus interface driver and then reads parameters from slave device in the segment.
After several successful read attempts slave sets the alarm relay (end of test condition). 
Once master reads the alarm it stops communication and destroy driver.

The examples:

* `examples/protocols/modbus/serial/mb_master` - Modbus serial master ASCII/RTU 
* `examples/protocols/modbus/serial/mb_slave` - Modbus serial slave ASCII/RTU 

See README.md for each individual project for more information.

## How to use example

### Hardware Required

This example can be run on any commonly available ESP32 development board.
The master and slave boards should be connected to each other through the RS485 interface line driver. 
See the connection schematic in README.md files of each example.

### Configure the project

This example test requires communication mode setting for master and slave be the same and slave address set to 1.
Please refer to README.md files of each example project for more information.

## About common_component in this example

The folder "mb_example_common" includes definitions of parameter structures for master and slave device (both projects share the same parameters).
However, currently it is for example purpose only and can be modified for particular application.

## Example Output

Example of Slave output:

```
I (343) SLAVE_TEST: Modbus slave stack initialized.
I (343) SLAVE_TEST: Start modbus test...
I (81463) SLAVE_TEST: HOLDING READ (81150420 us), ADDR:1, TYPE:2, INST_ADDR:0x3ffb2868, SIZE:6
I (82463) SLAVE_TEST: HOLDING READ (82150720 us), ADDR:1, TYPE:2, INST_ADDR:0x3ffb2868, SIZE:6
I (83573) SLAVE_TEST: HOLDING READ (83260630 us), ADDR:1, TYPE:2, INST_ADDR:0x3ffb2868, SIZE:6
I (84603) SLAVE_TEST: HOLDING READ (84290530 us), ADDR:1, TYPE:2, INST_ADDR:0x3ffb2868, SIZE:6
I (85703) SLAVE_TEST: HOLDING READ (85396692 us), ADDR:1, TYPE:2, INST_ADDR:0x3ffb2868, SIZE:6
```

Example of Modbus Master output:

```
I (399) MASTER_TEST: Modbus master stack initialized...
I (499) MASTER_TEST: Start modbus test...
I (549) MASTER_TEST: Characteristic #0 Data_channel_0 (Volts) value = 1.230000 (0x3f9d70a4) read successful.
I (629) MASTER_TEST: Characteristic #1 Humidity_1 (%rH) value = 12.100000 (0x4141999a) read successful.
I (709) MASTER_TEST: Characteristic #2 Temperature_1 (C) value = 3.560000 (0x4063d70a) read successful.
I (769) MASTER_TEST: Characteristic #3 Humidity_2 (%rH) value = 23.400000 (0x41bb3333) read successful.
I (829) MASTER_TEST: Characteristic #4 Temperature_2 (C) value = 5.890000 (0x40bc7ae1) read successful.
I (889) MASTER_TEST: Characteristic #5 Humidity_3 (%rH) value = 34.500000 (0x420a0000) read successful.
E (949) MB_CONTROLLER_MASTER: mbc_master_get_parameter(111): SERIAL master get parameter failure error=(0x108) (ESP_ERR_INVALID_RESPONSE).
E (949) MASTER_TEST: Characteristic #6 (RelayP1) read fail, err = 264 (ESP_ERR_INVALID_RESPONSE).
E (1029) MB_CONTROLLER_MASTER: mbc_master_get_parameter(111): SERIAL master get parameter failure error=(0x108) (ESP_ERR_INVALID_RESPONSE).
E (1029) MASTER_TEST: Characteristic #7 (RelayP2) read fail, err = 264 (ESP_ERR_INVALID_RESPONSE).
```

## Troubleshooting

If the examples do not work as expected and slave and master boards are not able to communicate correctly it is possible to find the reason for errors.
The most important errors are described in master example output and formatted as below:

```
E (1692332) MB_CONTROLLER_MASTER: mbc_master_get_parameter(111): SERIAL master get parameter failure error=(0x107) (ESP_ERR_TIMEOUT).
```

ESP_ERR_TIMEOUT (0x107) - Modbus slave device does not respond during configured timeout. Check the connection and ability for communication using uart_echo_rs485 example or increase
Kconfig value CONFIG_FMB_MASTER_TIMEOUT_MS_RESPOND (CONFIG_FMB_SERIAL_ASCII_TIMEOUT_RESPOND_MS).

ESP_ERR_NOT_SUPPORTED (0x106), ESP_ERR_INVALID_RESPONSE (0x108) - Modbus slave device does not support requested command or register and sent exeption response. 

ESP_ERR_INVALID_STATE (0x103) - Modbus stack is not configured correctly or can't work correctly due to critical failure.
 

