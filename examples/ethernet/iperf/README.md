| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Ethernet iperf Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

## Overview

This example demonstrates basic usage of [iperf](https://iperf.fr/) protocol to measure the throughout/bandwidth of Ethernet.

The cli environment in the example is based on the [console component](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/system/console.html).

## How to use example

### Hardware Required

To run this example, it's recommended that you have an official ESP32 Ethernet development board - [ESP32-Ethernet-Kit](https://docs.espressif.com/projects/esp-idf/en/latest/hw-reference/get-started-ethernet-kit.html). This example should also work for 3rd party ESP32 board as long as it's integrated with a supported Ethernet PHY chip. Up until now, ESP-IDF supports up to four Ethernet PHY: `LAN8720`, `IP101`, `DP83848` and `RTL8201`, additional PHY drivers should be implemented by users themselves.

Besides that, `esp_eth` component can drive third-party Ethernet module which integrates MAC and PHY and provides common communication interface (e.g. SPI, USB, etc). This example will take the **DM9051** as an example, illustrating how to install the Ethernet driver in the same manner.

#### Pin Assignment

See common pin assignments for Ethernet examples from [upper level](../README.md#common-pin-assignments).

### Software Tools Preparation

1. Install iperf tool on PC
   * Debian/Ubuntu: `sudo apt-get install iperf`
   * macOS: `brew install iperf`(if using Homebrew) or `sudo port install iperf`(if using MacPorts)
   * Windows(MSYS2): Downloads binaries from [here]( https://iperf.fr/iperf-download.php#windows)

### Configure the project

```
idf.py menuconfig
```

In addition to the common configurations for Ethernet examples from [upper level](../README.md#common-configurations), you might also need to update the default value of following configurations:

1. In the `Example Configuration` menu:
    * Enable storing history commands in flash under `Store command history in flash`.

### Build, Flash, and Run

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT build flash monitor
```

(Replace PORT with the name of the serial port to use.)

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
2. Put frequently invoked functions into IRAM via macro `IRAM_ATTR` in code. Note that the lwIP IRAM optimization is already enabled by default.
3. Priority of iperf task may also have effect.

## Troubleshooting

See common troubleshooting for Ethernet examples from [upper level](../README.md#common-troubleshooting).

(For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you as soon as possible.)
