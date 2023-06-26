| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# ENC28J60 Example
(See the README.md file in the upper level 'examples' directory for more information about examples.)

---

## !!! Warning !!!

Espressif doesn't recommend using ENC28J60 Ethernet controller in new designs based on ESP32 series of chips. This is due to the following facts:
* ENC28J60 has low performance in half-duplex mode and various errata exist to the half-duplex mode.
* ENC28J60 does not support automatic duplex negotiation when configured to full-duplex mode.
* ENC28J60 has high current consumption - up to 180mA in comparison to e.g. 79mA of `W5500` or 75mA of `KSZ8851SNL` @ 10Mbps Tx.

Therefore, we rather recommend using `W5500`, `KSZ8851SNL` or `DM9051`, which are also supported in ESP-IDF.

---

## Overview

ENC28J60 is a standalone Ethernet controller with a standard SPI interface. This example demonstrates how to drive this controller as an SPI device and then attach to TCP/IP stack.

This is also an example of how to integrate a new Ethernet MAC driver into the `esp_eth` component, without needing to modify the ESP-IDF component.

If you have a more complicated application to go (for example, connect to some IoT cloud via MQTT), you can always reuse the initialization codes in this example.

## How to use example

### Hardware Required

To run this example, you need to prepare following hardwares:
* [ESP32 dev board](https://www.espressif.com/en/products/devkits) (e.g. ESP32-PICO, ESP32 DevKitC, etc)
* ENC28J60 Ethernet module (the latest revision should be 6)
* **!! IMPORTANT !!** Proper input power source since ENC28J60 is quite power consuming device (it consumes more than 200 mA in peaks when transmitting). If improper power source is used, input voltage may drop and ENC28J60 may either provide nonsense response to host controller via SPI (fail to read registers properly) or it may enter to some strange state in the worst case. There are several options how to resolve it:
  * Power ESP32 dev board from `USB 3.0`, if the dev board is used as source of power to the ENC28J60 module.
  * Power ESP32 dev board from external 5 V power supply with current limit at least 1 A, if the dev board is used as source of power to the ENC28J60 module.
  * Power ENC28J60 from external 3.3 V power supply with common GND to ESP32 dev board. Note that there might be some ENC28J60 modules with integrated voltage regulator on market and so powered by 5 V. Please consult documentation of your board for details.

  If an ESP32 dev board is used as the source of power to the ENC28J60 module, ensure that that the particular dev board is assembled with a voltage regulator capable to deliver current of 1 A. This is a case of ESP32-DevKitC or ESP-WROVER-KIT, for example. Such setup was tested and works as expected. Other dev boards may use different voltage regulators and may perform differently.
  **WARNING:** Always consult documentation/schematics associated with particular ENC28J60 and ESP32 dev boards used in your use-case first.

#### Pin Assignment

* ENC28J60 Ethernet module consumes one SPI interface plus an interrupt GPIO. By default they're connected as follows, in case of ESP32 dev boards:

| ESP32 GPIO | ENC28J60    |
| ---------- | ----------- |
| GPIO14     | SPI_CLK     |
| GPIO13     | SPI_MOSI    |
| GPIO12     | SPI_MISO    |
| GPIO15     | SPI_CS      |
| GPIO4      | Interrupt   |

### Configure the project

```
idf.py menuconfig
```

In the `Example Configuration` menu, set SPI specific configuration, such as SPI host number, GPIO used for MISO/MOSI/CS signal, GPIO for interrupt event and the SPI clock rate, duplex mode.

**Note:** According to ENC28J60 data sheet and our internal testing, SPI clock could reach up to 20MHz, but in practice, the clock speed may depend on your PCB layout/wiring/power source. In this example, the default clock rate is set to 8 MHz since some ENC28J60 silicon revisions may not properly work at frequencies less than 8 MHz.

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
2. It is recommended to operate the ENC28J60 in full-duplex mode since various errata exist to the half-duplex mode (even though addressed in the example) and due to its poor performance in the half-duplex mode (especially in TCP connections). However, ENC28J60 does not support automatic duplex negotiation. If it is connected to an automatic duplex negotiation enabled network switch or Ethernet controller, then ENC28J60 will be detected as a half-duplex device. To communicate in Full-Duplex mode, ENC28J60 and the remote node (switch, router or Ethernet controller) **must be manually configured for full-duplex operation**:
   * The ENC28J60 can be set to full-duplex in the `Example Configuration` menu.
   * On Ubuntu/Debian Linux distribution use:
    ```
    sudo ethtool -s YOUR_INTERFACE_NAME speed 10 duplex full autoneg off
    ```
   * On Windows, go to `Network Connections` -> `Change adapter options` -> open `Properties` of selected network card -> `Configure` -> `Advanced` -> `Link Speed & Duplex` -> select `10 Mbps Full Duplex in dropdown menu`.
3. Ensure that your wiring between ESP32 dev board and the ENC28J60 module is realized by short wires with the same length and no wire crossings.
4. CS Hold Time needs to be configured to be at least 210 ns to properly read MAC and MII registers as defined by ENC28J60 Data Sheet. This is automatically configured in the example based on selected SPI clock frequency by computing amount of SPI bit-cycles the CS should stay active after the transmission. However, if your PCB design/wiring requires different value, please update `cs_ena_posttrans` member of `devcfg` structure per your actual needs.


## Troubleshooting

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
