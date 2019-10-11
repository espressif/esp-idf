# Ethernet Example
(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates basic usage of `Ethernet driver` together with `tcpip_adapter`. The work flow of the example could be as follows:

1. Install Ethernet driver
2. Send DHCP requests and wait for a DHCP lease
3. If get IP address successfully, then you will be able to ping the device

If you have a new Ethernet application to go (for example, connect to IoT cloud via Ethernet), try this as a basic template, then add your own code.

## How to use example

### Hardware Required

To run this example, it's recommended that you have an official ESP32 Ethernet development board - [ESP32-Ethernet-Kit](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/get-started-ethernet-kit.html). This example should also work for 3rd party ESP32 board as long as it's integrated with a supported Ethernet PHY chip. Up until now, ESP-IDF supports up to four Ethernet PHY: `LAN8720`, `IP101`, `DP83848` and `RTL8201`, additional PHY drivers should be implemented by users themselves.

`esp_eth` component not only supports ESP32 internal Ethernet MAC controller, but also can drive third-party Ethernet module which integrates MAC and PHY and provides SPI interface. This example also take the **DM9051** as an example, illustrating how to install the Ethernet driver with only a little different configuration.

### Project configuration in menuconfig

Enter `idf.py menuconfig` if you are using GNU Make based build system or enter `idf.py menuconfig` if you' are using CMake based build system.

1. In the `Example Configuration` menu:
    * Choose the kind of Ethernet this example will run on under `Ethernet Type`.
    * If `Internal EMAC` is selected:
        * Choose PHY device under `Ethernet PHY Device`, by default, the **ESP32-Ethernet-Kit** has an `IP101` on board.

    * If `SPI Ethernet Module` is selected:
        * Set SPI specific configuration, including GPIO and clock speed.

2. In the `Component config > Ethernet` menu:
    * If `Internal EMAC` is selected:
        * Enable `Use ESP32 internal EMAC controller`, and then go into this menu.
        * In the `PHY interface`, it's highly recommended that you choose `Reduced Media Independent Interface (RMII)` which will cost fewer pins.
        * In the `RMII clock mode`, you can choose the source of RMII clock (50MHz): `Input RMII clock from external` or `Output RMII clock from internal`.
        * Once `Output RMII clock from internal` is enabled, you also have to set the number of the GPIO used for outputting the RMII clock under `RMII clock GPIO number`. In this case, you can set the GPIO number to 16 or 17.
        * Once `Output RMII clock from GPIO0 (Experimental!)` is enabled, then you have no choice but GPIO0 to output the RMII clock.
        * Set SMI MDC/MDIO GPIO number according to board schematic, by default these two GPIOs are set as below:

            | Default Example GPIO | RMII Signal | Notes         |
            | -------------------- | ----------- | ------------- |
            | GPIO23               | MDC         | Output to PHY |
            | GPIO18               | MDIO        | Bidirectional |

        * If you have connect a GPIO to the PHY chip's RST pin, then you need to enable `Use Reset Pin of PHY Chip` and set the GPIO number under `PHY RST GPIO number`.

    * If `SPI Ethernet Module` is selected:
        * Set the GPIO number used by interrupt pin under `DM9051 Interrupt GPIO number`.


### Extra configuration in the code (Optional)

* By default Ethernet driver will assume the PHY address to `1`, but you can alway reconfigure this value after `eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();`. The actual PHY address should depend on the hardware you use, so make sure to consult the schematic and datasheet.

**Note:** DM9051 has a fixed PHY address `1`, which cannot be modified.
  
### Build and Flash

Enter `idf.py -p PORT flash monitor` if you are using GNU Make based build system or enter `idf.py build flash monitor` if you' are using CMake based build system.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```bash
I (394) eth_example: Ethernet Started
I (3934) eth_example: Ethernet Link Up
I (3934) eth_example: Ethernet HW Addr 30:ae:a4:c6:87:5b
I (5864) tcpip_adapter: eth ip: 192.168.2.151, mask: 255.255.255.0, gw: 192.168.2.2
I (5864) eth_example: Ethernet Got IP Address
I (5864) eth_example: ~~~~~~~~~~~
I (5864) eth_example: ETHIP:192.168.2.151
I (5874) eth_example: ETHMASK:255.255.255.0
I (5874) eth_example: ETHGW:192.168.2.2
I (5884) eth_example: ~~~~~~~~~~~
```

Now you can ping your ESP32 in the terminal by entering `ping 192.168.2.151` (it depends on the actual IP address you get).

## Troubleshooting

* RMII Clock
  * ESP32's MAC and the external PHY device need a common 50MHz reference clock (aka RMII clock). This clock can either be provided by an externally oscillator or generated from internal APLL. The signal integrity of RMII clock is strict, so it is highly recommended to add a 33Ω resistor in series to reduce possible ringing.
  * ESP32 can generate a 50MHz clock using internal APLL. But if the APLL is already used for other purposes (e.g. I2S peripheral), then you have no choice but use an external RMII clock.

* GPIO connections
  * RMII PHY wiring is fixed and can not be changed through either IOMUX or GPIO Matrix. They're described as below:

  | GPIO   | RMII Signal | ESP32 EMAC Function |
  | ------ | ----------- | ------------------- |
  | GPIO21 | TX_EN       | EMAC_TX_EN          |
  | GPIO19 | TX0         | EMAC_TXD0           |
  | GPIO22 | TX1         | EMAC_TXD1           |
  | GPIO25 | RX0         | EMAC_RXD0           |
  | GPIO26 | RX1         | EMAC_RXD1           |
  | GPIO27 | CRS_DV      | EMAC_RX_DRV         |

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
