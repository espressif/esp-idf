# Ethernet Example
(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates basic usage of `Ethernet driver` together with `tcpip_adapter`. The work flow of the example could be as follows:

1. Initialize the Ethernet interface and enable it
2. Send DHCP requests and wait for a DHCP lease
3. If get IP address successfully, then you will be able to ping the device

If you have a new Ethernet application to go (for example, connect to IoT cloud via Ethernet), try this as a basic template, then add your own code.

## How to use example

### Hardware Required

To run this example, you should have one ESP32 dev board integrated with an Ethernet interface, or just connect your ESP32 core board to a breakout board which featured with RMII Ethernet PHY. **Currently ESP-IDF officially supports three Ethernet PHY: `TLK110`, `LAN8720` and `IP101`, additional PHY drivers can be implemented by users themselves.**

### Configure the project

Enter `make menuconfig` if you are using GNU Make based build system or enter `idf.py menuconfig` if you' are using CMake based build system. Then go into `Example Configuration` menu.

* Choose PHY device under `Ethernet PHY Device` option

* Set PHY address under `Ethernet PHY address` option, this address depends on the hardware and the PHY configuration. Consult the documentation/datasheet for the PHY hardware you have.
  * Address 1 for the common Waveshare LAN8720 PHY breakout and the official ESP32-Ethernet-Kit board

* Check whether or not to control PHY's power under `Use PHY Power (enable / disable) pin` option, (if set true, you also need to set PHY Power GPIO number under `PHY Power GPIO` option)

* Set SMI MDC/MDIO GPIO number according to board schematic, default these two GPIOs are set as below:

  | Default Example GPIO | RMII Signal | Notes         |
  | -------------------- | ----------- | ------------- |
  | GPIO23               | MDC         | Output to PHY |
  | GPIO18               | MDIO        | Bidirectional |

* Select one kind of EMAC clock mode under `Ethernet PHY Clock Mode` option. Possible configurations of the clock are listed as below:

  | Mode     | GPIO Pin | Signal name  | Notes                                                        |
  | -------- | -------- | ------------ | ------------------------------------------------------------ |
  | external | GPIO0    | EMAC_TX_CLK  | Input of 50MHz PHY clock                                     |
  | internal | GPIO0    | CLK_OUT1     | Output of 50MHz APLL clock                                   |
  | internal | GPIO16   | EMAC_CLK_OUT | Output of 50MHz APLL clock                                   |
  | internal | GPIO17   | EMAC_CLK_180 | Inverted output of 50MHz APLL clock (suitable for long clock trace) |

  * The external reference clock of 50MHz must be supplied on `GPIO0`.
  * The ESP32 can generate a 50MHz clock using internal APLL. When the APLL is already used as clock source for other purposes (most likely I²S), you have no choice but choose external clock.

### Build and Flash

Enter `make -j4 flash monitor` if you are using GNU Make based build system or enter `idf.py build flash monitor` if you' are using CMake based build system.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```bash
I (0) cpu_start: App cpu up.
I (265) heap_init: Initializing. RAM available for dynamic allocation:
I (272) heap_init: At 3FFAE6E0 len 00001920 (6 KiB): DRAM
I (278) heap_init: At 3FFB3FF0 len 0002C010 (176 KiB): DRAM
I (284) heap_init: At 3FFE0440 len 00003AE0 (14 KiB): D/IRAM
I (291) heap_init: At 3FFE4350 len 0001BCB0 (111 KiB): D/IRAM
I (297) heap_init: At 4008851C len 00017AE4 (94 KiB): IRAM
I (303) cpu_start: Pro cpu start user code
I (322) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
I (325) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (325) emac: emac reset done
I (335) eth_example: Ethernet Started
I (4335) eth_example: Ethernet Link Up
I (6325) tcpip_adapter: eth ip: 192.168.2.164, mask: 255.255.255.0, gw: 192.168.2.2
I (6325) eth_example: Ethernet Got IP Address
I (6325) eth_example: ~~~~~~~~~~~
I (6325) eth_example: ETHIP:192.168.2.164
I (6335) eth_example: ETHMASK:255.255.255.0
I (6335) eth_example: ETHGW:192.168.2.2
I (6345) eth_example: ~~~~~~~~~~~
```

## Troubleshooting

* If the PHY address is incorrect then the EMAC will still be initialized, but all attempts to read/write configuration registers in the PHY's register will fail, for example, waiting for auto-negotiation done.

* Check PHY Clock

  > The ESP32's MAC and the External PHY device need a common 50MHz reference clock. This clock can either be provided externally by a crystal oscillator (e.g. crystal connected to the PHY or a separate crystal oscillator) or internally by generating from EPS32's APLL. The signal integrity of this clock is strict, so it is highly recommended to add a 33Ω resistor in series to reduce ringing.

* Check GPIO connections, the RMII PHY wiring is fixed which can not be changed through either IOMUX or GPIO Matrix. They're described as below:

  | GPIO   | RMII Signal | ESP32 EMAC Function |
  | ------ | ----------- | ------------------- |
  | GPIO21 | TX_EN       | EMAC_TX_EN          |
  | GPIO19 | TX0         | EMAC_TXD0           |
  | GPIO22 | TX1         | EMAC_TXD1           |
  | GPIO25 | RX0         | EMAC_RXD0           |
  | GPIO26 | RX1         | EMAC_RXD1           |
  | GPIO27 | CRS_DV      | EMAC_RX_DRV         |

* Check GPIO0

  > GPIO0 is a strapping pin for entering UART flashing mode on reset, care must be taken when using this pin as `EMAC_TX_CLK`. If the clock output from the PHY is oscillating during reset, the ESP32 may randomly enter UART flashing mode. One solution is to use an additional GPIO as a "power pin", which either powers the PHY on/off or enables/disables the PHY's own oscillator. This prevents the clock signal from being active during a system reset. For this configuration to work, `GPIO0` also needs a pullup resistor and the "power pin" GPIO will need a pullup/pulldown resistor - as appropriate in order to keep the PHY clock disabled when the ESP32 is in reset. See the example source code to see how the "power pin" GPIO can be managed in software.

