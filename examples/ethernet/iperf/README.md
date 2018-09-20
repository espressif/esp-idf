# Ethernet iperf example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates basic usage of [iperf](https://iperf.fr/) protocol to measure the throughout/bandwidth of Ethernet.

This example is based on esp-idf's console component. For more information about console you can read the [guide](https://docs.espressif.com/projects/esp-idf/en/latest/api-guides/console.html).

## How to use example

### Prepare work

1. Install iperf tool on PC
   * Debian/Ubuntu: `sudo apt-get install iperf` 
   * macOS: `brew install iperf`(if using Homebrew) or `sudo port install iperf`(if using MacPorts)
   * Windows(MSYS2): Downloads binaries from [here]( https://iperf.fr/iperf-download.php#windows)
2. Initialise Ethernet on ESP32, run `ethernet start`
3. Get ip information(optional), run `ethernet info`

### Configure the project

Enter `make menuconfig` if you are using GNU Make based build system or enter `idf.py menuconfig` if you' are using CMake based build system. Then go into `Example Configuration` menu.

* Check whether or not to store the history command into flash under `Store command history in flash` option

* Set PHY address under `PHY address` option, this address depends on the hardware and the PHY configuration. Consult the documentation/datasheet for the PHY hardware you have.

  - Address 31 (default) for Espressif's Ethernet board with TLK110 PHY by default
  - Address 1 for the common Waveshare LAN8720 PHY breakout
  - Address 0 for other LAN8720 breakouts

* Check whether or not to control PHY's power (if true, you need also to set PHY Power GPIO number later)

* Set SMI MDC/MDIO GPIO number according to board schemetic, default these two GPIOs are set as below:

  | Default Example GPIO | RMII Signal | Notes         |
  | -------------------- | ----------- | ------------- |
  | GPIO23               | MDC         | Output to PHY |
  | GPIO18               | MDIO        | Bidirectional |

* Select one kind of EMAC clock mode under `EMAC clock mode` option. Possible configurations of the clock are listed as below:

  | Mode     | GPIO Pin | Signal name  | Notes                                                        |
  | -------- | -------- | ------------ | ------------------------------------------------------------ |
  | external | GPIO0    | EMAC_TX_CLK  | Input of 50MHz PHY clock                                     |
  | internal | GPIO16   | EMAC_CLK_OUT | Output of 50MHz APLL clock                                   |
  | internal | GPIO17   | EMAC_CLK_180 | Inverted output of 50MHz APLL clock (suitable for long clock trace) |

  - The external reference clock of 50MHz must be supplied on `GPIO0`.
  - The ESP32 can generate a 50MHz clock using its APLL. When the APLL is already used as clock source for other purposes (most likely I²S), you have no choice but choose external clock.

### Build and Flash

Enter `make -j4 flash monitor` if you are using GNU Make based build system or enter `idf.py build flash monitor` if you' are using CMake based build system.

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
[  3]  0.0- 3.0 sec  24.8 MBytes  69.5 Mbits/sec   0.172 ms    1/17721 (0.0056%)
[  3]  3.0- 6.0 sec  24.8 MBytes  69.5 Mbits/sec   0.169 ms    0/17719 (0%)
[  3]  6.0- 9.0 sec  24.8 MBytes  69.5 Mbits/sec   0.170 ms    0/17719 (0%)
[  3]  9.0-12.0 sec  24.8 MBytes  69.5 Mbits/sec   0.170 ms    0/17718 (0%)
[  3] 12.0-15.0 sec  24.8 MBytes  69.5 Mbits/sec   0.169 ms    0/17717 (0%)
[  3] 15.0-18.0 sec  24.8 MBytes  69.5 Mbits/sec   0.170 ms    0/17720 (0%)
[  3] 18.0-21.0 sec  24.8 MBytes  69.5 Mbits/sec   0.170 ms    0/17721 (0%)
[  3] 21.0-24.0 sec  24.8 MBytes  69.5 Mbits/sec   0.169 ms    0/17720 (0%)
[  3] 24.0-27.0 sec  24.8 MBytes  69.5 Mbits/sec   0.169 ms    0/17723 (0%)
```

#### ESP32 output

```bash
mode=udp-client sip=192.168.2.156:5001, dip=192.168.2.160:5001, interval=3, time=30
    Interval           Bandwidth
   0-   3 sec       69.34 Mbits/sec
   3-   6 sec       69.55 Mbits/sec
   6-   9 sec       69.55 Mbits/sec
   9-  12 sec       69.55 Mbits/sec
  12-  15 sec       69.55 Mbits/sec
  15-  18 sec       69.56 Mbits/sec
  18-  21 sec       69.56 Mbits/sec
  21-  24 sec       69.56 Mbits/sec
  24-  27 sec       69.56 Mbits/sec
  27-  30 sec       69.56 Mbits/sec
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
[  3] 12.0-15.0 sec  28.6 MBytes  79.9 Mbits/sec
[  3] 15.0-18.0 sec  28.6 MBytes  80.0 Mbits/sec
[  3] 18.0-21.0 sec  28.6 MBytes  80.0 Mbits/sec
[  3] 21.0-24.0 sec  28.6 MBytes  80.0 Mbits/sec
[  3] 24.0-27.0 sec  28.6 MBytes  80.0 Mbits/sec
```

#### ESP32 output
```bash
mode=udp-server sip=192.168.2.156:5001, dip=0.0.0.0:5001, interval=3, time=30
    Interval           Bandwidth
I (2534456) iperf: want recv=16384
   0-  30 sec       72.67 Mbits/sec
   3-   6 sec       74.18 Mbits/sec
   6-   9 sec       73.14 Mbits/sec
   9-  12 sec       73.65 Mbits/sec
  12-  15 sec       72.87 Mbits/sec
  15-  18 sec       73.29 Mbits/sec
  18-  21 sec       74.35 Mbits/sec
  21-  24 sec       72.28 Mbits/sec
  24-  27 sec       73.39 Mbits/sec
  27-  30 sec       73.49 Mbits/sec
```

## Suggestions of getting higher bandwidth

1. Higher MCU working frequency will get higher bandwidth
2. Put frequency invoked function into IRAM via macro `IRAM_ATTR` in code.
3. Priority of iperf task may also have effect.

## Troubleshooting

- If the PHY address is incorrect then the EMAC will still be initialised, but all attempts to read/write configuration registers in the PHY's register will fail, for example, waiting for auto-negotiation done.

- Check PHY Clock

  > The ESP32's MAC and the External PHY device need a common 50MHz reference clock. This clock can either be provided externally by a crystal oscillator (e.g. crystal connected to the PHY or a seperate crystal oscillator) or internally by generating from EPS32's APLL. The signal integrity of this clock is strict, so it is highly recommended to add a 33Ω resistor in series to reduce ringing.

- Check GPIO connections, the RMII PHY wiring is fixed which can not be changed through either IOMUX or GPIO Matrix. They're described as below:

  | GPIO   | RMII Signal | ESP32 EMAC Function |
  | ------ | ----------- | ------------------- |
  | GPIO21 | TX_EN       | EMAC_TX_EN          |
  | GPIO19 | TX0         | EMAC_TXD0           |
  | GPIO22 | TX1         | EMAC_TXD1           |
  | GPIO25 | RX0         | EMAC_RXD0           |
  | GPIO26 | RX1         | EMAC_RXD1           |
  | GPIO27 | CRS_DV      | EMAC_RX_DRV         |

- Check GPIO0

  > GPIO0 is a strapping pin for entering UART flashing mode on reset, care must be taken when using this pin as `EMAC_TX_CLK`. If the clock output from the PHY is oscillating during reset, the ESP32 may randomly enter UART flashing mode. One solution is to use an additional GPIO as a "power pin", which either powers the PHY on/off or enables/disables the PHY's own oscillator. This prevents the clock signal from being active during a system reset. For this configuration to work, `GPIO0` also needs a pullup resistor and the "power pin" GPIO will need a pullup/pulldown resistor - as appropriate in order to keep the PHY clock disabled when the ESP32 is in reset. See the example source code to see how the "power pin" GPIO can be managed in software. The example defaults to using `GPIO17` for this function, but it can be overriden. On Espressif's Ethernet development board, `GPIO17` is the power pin used to enable/disable the PHY oscillator.