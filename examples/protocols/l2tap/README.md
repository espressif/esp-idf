| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP-NETIF L2 TAP Interface Example
(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates basic usage of ESP-NETIF L2 TAP Interface. The ESP-NETIF L2 TAP interface is ESP-IDF mechanism utilized to access Data Link Layer (L2 per OSI/ISO) for frame reception and transmission from user application. Its typical usage in embedded world might be implementation of non-IP related protocols such as PTP, Wake on LAN and others.

---

**Note:** Only Ethernet (IEEE 802.3) is currently supported.

---

The ESP-NETIF L2 TAP interface is accessed using file descriptors of VFS which provides a file-like interfacing (using functions like ``open()``, ``read()``, ``write()``, etc.) and its usage is specifically demonstrated by the following:

1) Waiting for a frame using blocking ``read()`` and echoing it back to the originator.
2) Waiting for a frame using ``select()`` with timeout and non-blocking ``read()``, and echoing it back to the originator.
3) Periodical broadcast transmission of "Hello message" to show proper construction of the Ethernet frame to be transmitted.

The work flow of the example is then as follows:

1. Install Ethernet driver.
2. Send DHCP requests and wait for a DHCP lease.
3. If get IP address successfully, then you will be able to ping the device.
4. Initialize ESP-NETIF L2 TAP interface, then you will be able to echo Ethernet frames with specific EthTypes (non-IP traffic).

## How to use example

### Hardware Required

To run this example, it is recommended to use official ESP32 Ethernet development board - [ESP32-Ethernet-Kit](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/get-started-ethernet-kit.html). This example should also work for 3rd party ESP32 board as long as it is integrated with a supported Ethernet PHY chip. Besides that, several third-party Ethernet modules which integrate MAC and PHY and provides common communication interface (e.g. SPI, USB, etc) is supported too. Consult "Example Connection Configuration" option in IDF project configuration tool for specific list of supported devices.

#### Pin Assignment

See common pin assignments for [Ethernet examples](../../ethernet/README.md#common-pin-assignments).

### Configure the project

```
idf.py menuconfig
```

See common configurations for [Ethernet examples](../../ethernet/README.md#common-configurations).

### Build, Flash, and Run

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

### Send a message from Host PC

Once the board is flashed and runs, you can send a message to it via raw socket and expect echo of that message. You can utilize ``pytest_example_l2tap_echo.py``. The scripts takes two optional parameters as (order of parameters needs to be kept):

```bash
python3 pytest_example_l2tap_echo.py [<dest_mac_address>] [<host_eth_interface>]
```

* `dest_mac_address` - MAC address of the destination device (the ESP32 board with flashed example). ESP 32 board MAC address is printed by the example so you can find it in monitor output under "Ethernet HW Addr". Broadcast address is used when this parameter is omitted.

* `host_eth_interface` - name of host PC Ethernet device on which the frame is to be transmitted/received. When this parameter is omitted, the script tries to find Ethernet interface automatically. If you need, you can find names of installed Network interfaces by running either of below commands on Linux:

```bash
$ ifconfig
$ ip address
$ ls /sys/class/net/
```

---

**Note:** ``pytest_example_l2tap_echo.py`` needs to be run with appropriate rights (e.g. use ``sudo``) since it accesses Network interface at Link (L2) layer, which may be considered harmful in general and so not allowed for regular user accounts.

---
---

**Warning:** ``pytest_example_l2tap_echo.py`` can be currently used only on Linux.

---

## Example Output

Host PC output:

```bash
$ sudo python3 pytest_example_l2tap_echo.py 08:3a:f2:31:20:f7 enx00e04c6801ac
2022-03-31 09:21:19,699 Use enx00e04c6801ac for testing
2022-03-31 09:21:19,737 Sent 52 bytes to 08:3a:f2:31:20:f7
2022-03-31 09:21:19,737 Sent msg: "ESP32 test message with EthType 0x2220"
2022-03-31 09:21:19,739 Received 60 bytes echoed from 08:3a:f2:31:20:f7
2022-03-31 09:21:19,739 Echoed msg: "ESP32 test message with EthType 0x2220"
2022-03-31 09:21:19,764 Use enx00e04c6801ac for testing
2022-03-31 09:21:19,804 Sent 52 bytes to 08:3a:f2:31:20:f7
2022-03-31 09:21:19,805 Sent msg: "ESP32 test message with EthType 0x2221"
2022-03-31 09:21:19,806 Received 60 bytes echoed from 08:3a:f2:31:20:f7
2022-03-31 09:21:19,806 Echoed msg: "ESP32 test message with EthType 0x2221"
```

ESP32 L2 TAP Example output:

```bash
I (5402) example_connect: Connected to example_connect: eth
I (5402) example_connect: - IPv4 address: 192.168.20.103
I (5412) example_connect: - IPv6 address: fe80:0000:0000:0000:0a3a:f2ff:fe31:20f7, type: ESP_IP6_ADDR_IS_LINK_LOCAL
I (5422) l2tap_example: Ethernet HW Addr 08:3a:f2:31:20:f7
I (5432) l2tap_example: /dev/net/tap fd 4 successfully opened
I (5432) l2tap_example: /dev/net/tap fd 3 successfully opened
I (5442) l2tap_example: L2 TAP fd 3 configured in blocking mode
I (5442) l2tap_example: L2 TAP fd 4 configured in non-blocking mode
I (5442) l2tap_example: L2 TAP fd 3 successfully bound to `ETH_DEF`
I (5462) l2tap_example: L2 TAP fd 4 successfully bound to `ETH_DEF`
I (5462) l2tap_example: L2 TAP fd 3 Ethernet type filter configured to 0x2220
I (5472) l2tap_example: L2 TAP fd 4 Ethernet type filter configured to 0x2221
I (5442) l2tap_example: /dev/net/tap fd 5 successfully opened
I (5492) l2tap_example: L2 TAP fd 5 configured in blocking mode
I (5492) l2tap_example: L2 TAP fd 5 successfully bound to `ETH_DEF`
I (5502) l2tap_example: L2 TAP fd 5 Ethernet type filter configured to 0x2223
I (12102) l2tap_example: fd 3 received 60 bytes from 00:e0:4c:68:01:ac
I (12162) l2tap_example: fd 4 received 60 bytes from 00:e0:4c:68:01:ac
```

## Troubleshooting

See common troubleshooting for examples using [Ethernet](../../ethernet/README.md#common-troubleshooting).

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
