| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-P4 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Iperf Example

## Note about iperf version
The iperf example doesn't support all features in standard iperf. It's compatible with iperf version 2.x.

- Refer to the ESP Component Registry iperf-cmd page for more information: https://components.espressif.com/components/espressif/iperf-cmd

## Note about 80MHz flash frequency (ESP32)
The iperf can get better throughput if the SPI flash frequency is set to 80MHz, but the system may crash in 80MHz mode for ESP-WROVER-KIT.
Removing R140~R145 from the board can fix this issue. Currently the default SPI frequency is set to 40MHz, if you want to change the SPI flash
frequency to 80MHz, please make sure R140~R145 are removed from ESP-WROVER-KIT or use ESP32 DevKitC.

## Introduction
This example implements the protocol used by the common performance measurement tool [iPerf](https://iperf.fr/).
Performance can be measured between two ESP targets running this example, or between a single ESP target and a computer running the iPerf tool

Demo steps to test station TCP Tx performance:

- Configure in `menuconfig` which serial output you are using. Execute `idf.py menuconfig` and go to `Component config/ESP System Settings/Channel for console output`, then select the appropriate interface. By default the UART0 interface is used, this means that for example in the ESP32-S3-DevKitC-1 or ESP32-C6-DevKitC-1 you should connect to the micro-usb connector labeled as UART and not to the one labeled as USB. To use the one labeled as USB you should change the aforementioned setting to `USB Serial/JTAG Controller`.

- Build and flash the iperf example with `sdkconfig.defaults`, which contains performance test specific configurations
  - Use `help` for detailed command usage information.

- Run the demo as station mode and join the target AP
  - `sta_connect <ssid> <password>`
  - NOTE: the dut is started in station mode by default. If you want to use the dut as softap, please set wifi mode first:
    - `wifi_mode ap`
    - `ap_set <dut_ap_ssid> <dut_ap_password>`

- Run iperf as server on AP side
  - `iperf -s -i 3`

- Run iperf as client on ESP side
  - `iperf -c 192.168.10.42 -i 3 -t 60`

The console output, which is printed by station TCP RX throughput test, looks like:

  ```
  iperf> sta_connect testap-11 ********
  I (36836) WIFI: Connecting to testap-11...
  I (36839) WIFI: DONE.WIFI_CONNECT_START,OK.
  iperf> I (39248) WIFI: WIFI_EVENT_STA_DISCONNECTED! reason: 201
  I (39249) WIFI: trying to reconnect...
  I (41811) wifi:new:<11,2>, old:<1,0>, ap:<255,255>, sta:<11,2>, prof:1, snd_ch_cfg:0x0
  I (41813) wifi:state: init -> auth (0xb0)
  I (41816) wifi:state: auth -> assoc (0x0)
  I (41840) wifi:state: assoc -> run (0x10)
  I (41847) wifi:<ba-add>idx:0 (ifx:0, 30:5a:3a:74:90:f0), tid:0, ssn:0, winSize:64
  I (41914) wifi:connected with testap-11, aid = 1, channel 11, 40D, bssid = 30:5a:3a:74:90:f0
  I (41915) wifi:security: WPA2-PSK, phy: bgn, rssi: -34
  I (41926) wifi:pm start, type: 0
  
  I (41927) wifi:dp: 1, bi: 102400, li: 3, scale listen interval from 307200 us to 307200 us
  I (41929) WIFI: WIFI_EVENT_STA_CONNECTED!
  I (41983) wifi:AP's beacon interval = 102400 us, DTIM period = 3
  I (42929) esp_netif_handlers: sta ip: 192.168.1.79, mask: 255.255.255.0, gw: 192.168.1.1
  I (42930) WIFI: IP_EVENT_STA_GOT_IP: Interface "sta" address: 192.168.1.79
  I (42942) WIFI: - IPv4 address: 192.168.1.79,
  iperf> 
  iperf> iperf -s -i 2
  I (84810) IPERF: mode=tcp-server sip=0.0.0.0:5001,             dip=0.0.0.0:5001,               interval=2, time=30
  I (84812) iperf: Socket created
  iperf> I (87967) iperf: accept: 192.168.1.2,43726
  
  Interval       Bandwidth
  0.0- 2.0 sec  24.36 Mbits/sec
  2.0- 4.0 sec  23.38 Mbits/sec
  4.0- 6.0 sec  24.02 Mbits/sec
  6.0- 8.0 sec  25.27 Mbits/sec
  8.0-10.0 sec  23.84 Mbits/sec
  ```

Steps to test station/soft-AP TCP/UDP RX/TX throughput are similar as test steps in station TCP TX.

See the README.md file in the upper level 'examples' directory for more information about examples.
