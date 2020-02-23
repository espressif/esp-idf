# ENC28J60 Example
(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

ENC28J60 is a standalone Ethernet controller with a standard SPI interface. This example demonstrates how to drive this controller as an SPI device and then attach to TCP/IP stack.

This is also an example of how to integrate a new Ethernet MAC driver into the `esp_eth` component, without needing to modify the ESP-IDF component.

If you have a more complicated application to go (for example, connect to some IoT cloud via MQTT), you can always reuse the initialization codes in this example.

## How to use example

### Hardware Required

To run this example, you need to prepare following hardwares:
* [ESP32 board](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/modules-and-boards.html) (e.g. ESP32-PICO, ESP32 DevKitC, etc)
* ENC28J60 module (the latest revision should be 6)

#### Pin Assignment

* ENC28J60 Ethernet module consumes one SPI interface plus an interrupt GPIO. By default they're connected as follows:

| GPIO   | ENC28J60    |
| ------ | ----------- |
| GPIO19 | SPI_CLK     |
| GPIO23 | SPI_MOSI    |
| GPIO25 | SPI_MISO    |
| GPIO22 | SPI_CS      |
| GPIO4  | Interrupt   |

### Configure the project

```
idf.py menuconfig
```

In the `Example Configuration` menu, set SPI specific configuration, such as SPI host number, GPIO used for MISO/MOSI/CS signal, GPIO for interrupt event and the SPI clock rate.

**Note:** According to ENC28J60 data sheet, SPI clock could reach up to 20MHz, but in practice, the clock speed will depend on your PCB layout (in this example, the default clock rate is set to 6MHz, just to make sure that most modules on the market can work at this speed).

### Build, Flash, and Run

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```bash
I (0) cpu_start: Starting scheduler on APP CPU.
I (401) enc28j60: revision: 6
I (411) esp_eth.netif.glue: 00:04:a3:12:34:56
I (411) esp_eth.netif.glue: ethernet attached to netif
I (421) eth_example: Ethernet Started
I (2421) enc28j60: working in 10Mbps
I (2421) enc28j60: working in half duplex
I (2421) eth_example: Ethernet Link Up
I (2421) eth_example: Ethernet HW Addr 00:04:a3:12:34:56
I (4391) esp_netif_handlers: eth ip: 192.168.2.34, mask: 255.255.255.0, gw: 192.168.2.2
I (4391) eth_example: Ethernet Got IP Address
I (4391) eth_example: ~~~~~~~~~~~
I (4391) eth_example: ETHIP:192.168.2.34
I (4401) eth_example: ETHMASK:255.255.255.0
I (4401) eth_example: ETHGW:192.168.2.2
I (4411) eth_example: ~~~~~~~~~~~
```

Now you can ping your ESP32 in the terminal by entering `ping 192.168.2.34` (it depends on the actual IP address you get).

**Notes:**
1. ENC28J60 hasn't burned any valid MAC address in the chip, you need to write an unique MAC address into its internal MAC address register before any traffic happened on TX and RX line.
2. ENC28J60 does not support automatic duplex negotiation. If it is connected to an automatic duplex negotiation enabled network switch or Ethernet controller, then ENC28J60 will be detected as a half-duplex device. To communicate in Full-Duplex mode, ENC28J60 and the remote node (switch, router or Ethernet controller) must be manually configured for full-duplex operation.

## Troubleshooting

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
