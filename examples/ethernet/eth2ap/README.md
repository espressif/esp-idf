# eth2ap Example
(See the README.md file in the upper level 'examples' directory for more information about examples. To try a more complex application about Ethernet to WiFi data forwarding, please go to [iot-solution](https://github.com/espressif/esp-iot-solution/tree/master/examples/eth2wifi).)

**Note:** This example uses some internal APIs (e.g. `esp_wifi_internal_tx`) which might get changed between minor versions of ESP-IDF.

## Overview
![eth2ap](eth2ap.png)

The similarities on MAC layer between Ethernet and Wi-Fi make it easy to forward packets from Ethernet to Wi-Fi and vice versa. This example illustrates how to implement a simple "router" which only supports forwarding packets between Ethernet port and Wi-Fi AP interface. In this case, the Ethernet should play the role of WAN (i.e. it can access outside network) so that a mobile device could get access to the Internet when it gets connected to ESP32 through Wi-Fi.

**Note:** In this example, ESP32 works like a *bridge* between Ethernet and Wi-Fi, and it won't perform any actions on Layer3 and higher layer, which means there's no need to initialize the TCP/IP stack.

## How to use this example

### Hardware Required

To run this example, it's recommended that you have an official ESP32 Ethernet development board - [ESP32-Ethernet-Kit](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/get-started-ethernet-kit.html). This example should also work for 3rd party ESP32 board as long as it's integrated with a supported Ethernet PHY chip. Up until now, ESP-IDF supports up to four Ethernet PHY: `LAN8720`, `IP101`, `DP83848` and `RTL8201`, additional PHY drivers should be implemented by users themselves.

`esp_eth` component not only supports ESP32 internal Ethernet MAC controller, but also can drive third-party Ethernet module which integrates MAC and PHY and provides SPI interface. This example also take the **DM9051** as an example, illustrating how to install the Ethernet driver with only a little different configuration.

### Project configuration in menuconfig

Open the project configuration menu (`idf.py menuconfig`).

1. In the `Example Configuration` menu:
    * Set the SSID and password for Wi-Fi ap interface under `Wi-Fi SSID` and `Wi-Fi Password`.
    * Set the maximum connection number under `Maximum STA connections`.
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

* By default Ethernet driver will assume the PHY address to `1`, but you can alway reconfigure this value after `eth_phy_config_t phy_config = ETH_PHY_DEFAULT_CONFIG();`. The actual PHY address should depend on the hardware you use, so make sure to consult the schematic and datasheet.peripheral (e.g. I²S), you'd better choose the external clock.

**Note:** DM9051 has a fixed PHY address `1`, which cannot be modified.

### Build and Flash

To build and flash the example, enter `idf.py -p PORT flash monitor`.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://docs.espressif.com/projects/esp-idf/en/latest/get-started/index.html) for full steps to configure and use ESP-IDF to build projects.

## Example Output

### Step 1: Initialize Ethernet and Wi-Fi (AP mode)

```bash
I (508) example: Power On Ethernet PHY
I (518) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (518) emac: emac reset done
I (518) example: Ethernet Started
......
I (538) wifi: wifi driver task: 3ffc7fbc, prio:23, stack:3584, core=0
I (538) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (538) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (568) wifi: wifi firmware version: ec61a20
I (568) wifi: config NVS flash: enabled
I (568) wifi: config nano formating: disabled
I (568) wifi: Init dynamic tx buffer num: 32
I (568) wifi: Init data frame dynamic rx buffer num: 32
I (578) wifi: Init management frame dynamic rx buffer num: 32
I (588) wifi: Init management short buffer num: 32
I (588) wifi: Init static rx buffer size: 1600
I (588) wifi: Init static rx buffer num: 10
I (598) wifi: Init dynamic rx buffer num: 32
```

### Step 2: Ethernet Connects to Router/Switch/PC (with DHCP server enabled)

```bash
I (4518) example: Ethernet Link Up
```

### Step 3: Start Wi-Fi AP

```bash
I (4618) phy: phy_version: 4100, 2a5dd04, Jan 23 2019, 21:00:07, 0, 0
I (4618) wifi: mode : softAP (30:ae:a4:c6:87:5b)
I (4628) wifi: Total power save buffer number: 16
I (4628) wifi: Init max length of beacon: 752/752
I (4628) wifi: Init max length of beacon: 752/752
```

### Step 4: Wi-Fi station (e.g. mobile phone) connects to ESP32's Wi-Fi

```bash
I (10168) wifi: new:<1,0>, old:<1,0>, ap:<1,1>, sta:<255,255>, prof:1
I (10168) wifi: station: c4:0b:cb:ec:9a:84 join, AID=1, bgn, 20
I (10258) example: AP got a station connected
```

Now your mobile phone should get access to the Internet.

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

* Got error message `WiFi send packet failed` when running the example.
    * Ethernet process packets faster than Wi-Fi on ESP32, so have a try to enlarge the value of `FLOW_CONTROL_WIFI_SEND_DELAY_MS`.

* Got error message `send flow control message failed or timeout` when running the example.
    * Enlarge the length of `FLOW_CONTROL_QUEUE_LENGTH`.

* Wi-Fi station doesn't receive any IP via DHCP.
    * All Layer 3 (TCP/IP functions) on the ESP32 are disabled, including the SoftAP DHCP server. This means that devices must be able to access another DHCP server (for example on a Wi-Fi router connected via ethernet) or should use statically assigned IP addresses.

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
