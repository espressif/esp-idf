| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Ethernet VLAN Support Example

See the [README.md](../../ethernet/README.md) file in the upper level [examples](../../ethernet) directory for more information about examples.

## Overview

This example demonstrates the creation of one virtual network interface over Ethernet, an additional VLAN interface(disabled by default), and an interface without vlan tag. The work flow of the example could be as follows:

1. Install Ethernet driver for standard interface and both the virtual network interface.
2. Setup static IP address to both the virtual network interface.
3. Send DHCP requests and wait for a DHCP lease on the standard interface
4. If get IP address successfully, then you will be able to ping the standard interface of the device.
5. The virtual interfaces can be pinged over the configured static IP address.

Note: The code in vlan_support_main.c can be modified to add more vlan interfaces. The maximum number of interfaces that can be added is specified by the macro `MAX_ETH_NETIF_COUNT`.
## How to use example

### Hardware Required

To run this example, it's recommended that you have an official ESP32 Ethernet development board - [ESP32-Ethernet-Kit](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/get-started-ethernet-kit.html). This example should also work for 3rd party ESP32 board as long as it's integrated with a supported Ethernet PHY chip.

### Configure the project

```
idf.py menuconfig
```
See common configurations for Ethernet examples from [upper level](../../ethernet/README.md#common-configurations).

The Virtual network interface is enabled by default.
An additional virtual network interface can also be enabled.
The VLAN tag, Static IP address, network mask and default gateway of these interfaces are also configurable.

To configure the virtual network interfaces select:
```
Example Configuration  --->
```

### Build, Flash, and Run

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (0) cpu_start: Starting scheduler on APP CPU.
I (423) system_api: Base MAC address is not set
I (423) system_api: read default base MAC address from EFUSE
I (443) esp_eth.netif.netif_glue: 24:d7:eb:bb:f2:1b
I (443) esp_eth.netif.netif_glue: ethernet attached to netif
I (443) esp_eth.netif.netif_glue: 24:d7:eb:bb:f2:1b
I (443) esp_eth.netif.netif_glue: ethernet attached to netif
I (453) esp_eth.netif.netif_glue: 24:d7:eb:bb:f2:1b
I (463) esp_eth.netif.netif_glue: ethernet attached to netif
I (3563) eth_vlan_example: Ethernet Started
I (3563) eth_vlan_example: Ethernet Link Up
I (3563) eth_vlan_example: Ethernet HW Addr 24:d7:eb:bb:f2:1b
I (3563) eth_vlan_example: Ethernet interface(24:d7:eb:bb:f2:1b): ETH_VLAN20, Got IP Address
I (3573) eth_vlan_example: ~~~~~~~~~~~
I (3573) eth_vlan_example: ETHIP:192.168.20.10
I (3583) eth_vlan_example: ETHMASK:192.168.20.1
I (3583) eth_vlan_example: ETHGW:255.255.255.0
I (3593) eth_vlan_example: ~~~~~~~~~~~
I (3593) esp_netif_handlers: eth ip: 192.168.20.10, mask: 192.168.20.1, gw: 255.255.255.0
I (3603) eth_vlan_example: Ethernet interface(24:d7:eb:bb:f2:1b): ETH_VLAN30, Got IP Address
I (3613) eth_vlan_example: ~~~~~~~~~~~
I (3623) eth_vlan_example: ETHIP:192.168.30.10
I (3623) eth_vlan_example: ETHMASK:192.168.30.1
I (3633) eth_vlan_example: ETHGW:255.255.255.0
I (3633) eth_vlan_example: ~~~~~~~~~~~
I (3643) esp_netif_handlers: eth ip: 192.168.30.10, mask: 192.168.30.1, gw: 255.255.255.0
I (7943) eth_vlan_example: Ethernet interface(24:d7:eb:bb:f2:1b): ETH_DEF, Got IP Address
I (7943) eth_vlan_example: ~~~~~~~~~~~
I (7943) eth_vlan_example: ETHIP:192.168.10.100
I (7943) eth_vlan_example: ETHMASK:255.255.255.0
I (7953) eth_vlan_example: ETHGW:192.168.10.1
I (7953) eth_vlan_example: ~~~~~~~~~~~
I (7963) esp_netif_handlers: eth ip: 192.168.10.100, mask: 255.255.255.0, gw: 192.168.10.1
```

Now you can ping your ESP32 in the terminal.
You can ping the ESP32 VLAN interface from the VLAN interface of your system with default VLAN tag 20.

## Troubleshooting
### Setup VLAN on Linux
```
vconfig add eth0 20
ip addr add 192.168.20.30/24 dev eth0.20
ip link set eth0.20 up
```
Replace interface eth0 with your system ethernet interface.
### Setup VLAN on Mac
```
sudo ifconfig vlan20 create
sudo ifconfig vlan20 vlan 20 vlandev en5
sudo ifconfig vlan20 inet 192.168.20.30 netmask 255.255.255.0
```
Replace interface en5 with your system ethernet interface.

See common troubleshooting for Ethernet examples from [upper level](../../ethernet/README.md#common-troubleshooting).

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
