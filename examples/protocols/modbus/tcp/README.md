# Modbus TCP Master-Slave Example

## Overview

These two projects illustrate the communication between Modbus master and slave device in the segment.
Master initializes Modbus interface driver and then reads parameters from slave device in the segment.
After several successful read attempts slave sets the alarm relay (end of test condition). 
Once master reads the alarm it stops communication and destroy driver.

The examples:

* `examples/protocols/modbus/tcp/mb_tcp_master` - Modbus TCP master
* `examples/protocols/modbus/tcp/mb_tcp_slave` - Modbus TCP slave

See README.md for each individual project for more information.

## How to use example

### Hardware Required

This example can be run on any commonly available ESP32(-S2) development board.
The master and slave boards should be connected to the same network (see the README.md file in example folder) and slave address `CONFIG_MB_SLAVE_ADDR` be defined for slave board(s). 
See the connection schematic in README.md files of each example.

### Configure the project

This example test requires communication mode setting for master and slave be the same and slave address set to 1.
Please refer to README.md files of each example project for more information. This example uses the default option `CONFIG_MB_SLAVE_IP_FROM_STDIN` to resolve slave IP address and supports IPv4 address type for communication in this case.

## About common_component in this example

The folder "mb_example_common" one level above includes definitions of parameter structures for master and slave device (both projects share the same parameters).
However, currently it is for example purpose only and can be modified for particular application.

## Example Output

Refer to README.md file in the appropriate example folder for more information about master and slave log output.

## Troubleshooting

If the examples do not work as expected and slave and master boards are not able to communicate correctly it is possible to find the reason for errors.
The most important errors are described in master example output and formatted as below:

```
E (1692332) MB_CONTROLLER_MASTER: mbc_master_get_parameter(111): SERIAL master get parameter failure error=(0x107) (ESP_ERR_TIMEOUT).
```

ESP_ERR_TIMEOUT (0x107) - Modbus slave device does not respond during configured timeout. 
Check ability for communication pinging each slave configured in the master parameter description table or use command on your host machine to find modbus slave using mDNS (requires `CONFIG_MB_MDNS_IP_RESOLVER` option be enabled): 
```>dns-sd -L mb_slave_tcp_XX _modbus._tcp .```
where XX is the short slave address (index) of the slave configured in the Kconfig of slave example. 
Also it is possible to increase Kconfig value `CONFIG_FMB_MASTER_TIMEOUT_MS_RESPOND` to compensate network communication delays between master and slaves.

ESP_ERR_NOT_SUPPORTED (0x106), ESP_ERR_INVALID_RESPONSE (0x108) - Modbus slave device does not support requested command or register and sent exeption response. 

ESP_ERR_INVALID_STATE (0x103) - Modbus stack is not configured correctly or can't work correctly due to critical failure.
 

