# Ethernet iperf Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates basic usage of [iperf](https://iperf.fr/) protocol to measure the throughout/bandwidth of Ethernet.

The cli environment in the example is based on the [console component](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/console.html).

## How to use example

### Hardware Required

To run this example, it's recommended that you have an official ESP32 Ethernet development board - [ESP32-Ethernet-Kit](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/get-started-ethernet-kit.html). This example should also work for 3rd party ESP32 board as long as it's integrated with a supported Ethernet PHY chip. Up until now, ESP-IDF supports up to four Ethernet PHY: `LAN8720`, `IP101`, `DP83848` and `RTL8201`, additional PHY drivers should be implemented by users themselves.

`esp_eth` component not only supports ESP32 internal Ethernet MAC controller, but also can drive third-party Ethernet module which integrates MAC and PHY and provides SPI interface. This example also take the **DM9051** as an example, illustrating how to install the Ethernet driver with only a little different configuration.

### Other Preparation

1. Install iperf tool on PC
   * Debian/Ubuntu: `sudo apt-get install iperf` 
   * macOS: `brew install iperf`(if using Homebrew) or `sudo port install iperf`(if using MacPorts)
   * Windows(MSYS2): Downloads binaries from [here]( https://iperf.fr/iperf-download.php#windows)

### Project configuration in menuconfig

Enter `idf.py menuconfig` if you are using GNU Make based build system or enter `idf.py menuconfig` if you' are using CMake based build system.

1. In the `Example Configuration` menu:
    * Enable storing history commands in flash under `Store command history in flash`.
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

### Test uplink bandwidth

* PC: run command: `iperf -u -s -i 3` to start iperf server in UDP mode, and report interval is 3 seconds.
* ESP32: run command: `iperf -u -c PC_IP -i 3 -t 30` to start iperf client in UDP mode, and the test will last 30 seconds.

#### PC output

```bash
------------------------------------------------------------
Server listening on UDP port 5001
Receiving 1470 byte datagrams
UDP buffer size:  208 KByte (default)
------------------------------------------------------------
[  3] local 192.168.2.160 port 5001 connected with 192.168.2.156 port 49154
[ ID] Interval       Transfer     Bandwidth        Jitter   Lost/Total Datagrams
[  3]  0.0- 3.0 sec  26.1 MBytes  72.8 Mbits/sec   0.198 ms    1/18585 (0.0054%)
[  3]  3.0- 6.0 sec  26.3 MBytes  73.7 Mbits/sec   0.192 ms    0/18792 (0%)
[  3]  6.0- 9.0 sec  26.3 MBytes  73.5 Mbits/sec   0.189 ms    0/18741 (0%)
[  3]  9.0-12.0 sec  26.2 MBytes  73.3 Mbits/sec   0.191 ms   43/18739 (0.23%)
[  3] 12.0-15.0 sec  26.3 MBytes  73.5 Mbits/sec   0.194 ms    0/18739 (0%)
[  3] 15.0-18.0 sec  26.3 MBytes  73.5 Mbits/sec   0.191 ms    0/18741 (0%)
[  3] 18.0-21.0 sec  26.3 MBytes  73.5 Mbits/sec   0.187 ms    0/18752 (0%)
[  3] 21.0-24.0 sec  26.3 MBytes  73.4 Mbits/sec   0.192 ms    0/18737 (0%)
[  3] 24.0-27.0 sec  26.3 MBytes  73.5 Mbits/sec   0.188 ms    0/18739 (0%)
```

#### ESP32 output

```bash
mode=udp-client sip=192.168.2.156:5001, dip=192.168.2.160:5001, interval=3, time=30
    Interval           Bandwidth
   0-   3 sec       72.92 Mbits/sec
   3-   6 sec       73.76 Mbits/sec
   6-   9 sec       73.56 Mbits/sec
   9-  12 sec       73.56 Mbits/sec
  12-  15 sec       73.56 Mbits/sec
  15-  18 sec       73.56 Mbits/sec
  18-  21 sec       73.61 Mbits/sec
  21-  24 sec       73.55 Mbits/sec
  24-  27 sec       73.56 Mbits/sec
  27-  30 sec       73.56 Mbits/sec
   0-  30 sec       73.52 Mbits/sec
```

### Test downlink bandwidth

* PC: run command: `iperf -u -c ESP_IP -b 80M -t 30 -i 3` to start iperf client in UDP mode with estimated bandwidth 100M, and report interval is 3 seconds.
* ESP32: run command: `iperf -u -s -t 30 -i 3` to start iperf server in UDP mode, and the test will last 30 seconds.

#### PC output
```bash
------------------------------------------------------------
Client connecting to 192.168.2.156, UDP port 5001
Sending 1470 byte datagrams
UDP buffer size:  208 KByte (default)
------------------------------------------------------------
[  3] local 192.168.2.160 port 59581 connected with 192.168.2.156 port 5001
[ ID] Interval       Transfer     Bandwidth
[  3]  0.0- 3.0 sec  28.6 MBytes  80.0 Mbits/sec
[  3]  3.0- 6.0 sec  28.6 MBytes  80.0 Mbits/sec
[  3]  6.0- 9.0 sec  28.6 MBytes  80.0 Mbits/sec
[  3]  9.0-12.0 sec  28.6 MBytes  80.0 Mbits/sec
[  3] 12.0-15.0 sec  28.4 MBytes  79.5 Mbits/sec
[  3] 15.0-18.0 sec  28.6 MBytes  79.9 Mbits/sec
[  3] 18.0-21.0 sec  28.6 MBytes  79.9 Mbits/sec
[  3] 21.0-24.0 sec  28.6 MBytes  79.9 Mbits/sec
[  3] 24.0-27.0 sec  28.6 MBytes  80.0 Mbits/sec
[  3] 27.0-30.0 sec  28.5 MBytes  79.7 Mbits/sec
[  3]  0.0-30.0 sec   286 MBytes  79.9 Mbits/sec
```

#### ESP32 output
```bash
mode=udp-server sip=192.168.2.156:5001, dip=0.0.0.0:5001, interval=3, time=30
    Interval           Bandwidth
I (2534456) iperf: want recv=16384
   0-   3 sec       79.36 Mbits/sec
   3-   6 sec       79.56 Mbits/sec
   6-   9 sec       79.51 Mbits/sec
   9-  12 sec       79.24 Mbits/sec
  12-  15 sec       77.33 Mbits/sec
  15-  18 sec       79.01 Mbits/sec
  18-  21 sec       78.58 Mbits/sec
  21-  24 sec       78.24 Mbits/sec
  24-  27 sec       79.56 Mbits/sec
  27-  30 sec       77.20 Mbits/sec
   0-  30 sec       78.76 Mbits/sec
```

## Suggestions of getting higher bandwidth

1. Higher MCU working frequency will get higher bandwidth.
2. Put frequency invoked function into IRAM via macro `IRAM_ATTR` in code.
3. Priority of iperf task may also have effect.

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
