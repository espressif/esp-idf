| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# WiFi station example

(See the README.md file in the upper level 'examples' directory for more information about examples.)


## How to use example

### Configure the project

```
idf.py menuconfig
```

Set following parameters under Example Configuration Options:

* Set `WiFi SSID` and `WiFi Password` and `Maximal retry` under Example Configuration Options.

* Set `Static IP address` of your device static IP.

* Set `Static netmask address` of your device static netmask address.

* Set `Static gateway address` of your device static gateway address.

* Set `Choose DNS server` with `Set manual value as DNS server` or `Use gateway address as DNS server` to configure your DNS server.

* Choose `Set manual value as DNS server` to configure manual DNS server with `Main DNS server address` and `Backup DNS server address`.

* Set `Enable DNS resolve test` to resolve your host which input in `Domain name to resolve`.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output
Note that the output, in particular the order of the output, may vary depending on the environment.

Console output if station connects to AP successfully:
```
I (563) static_ip: ESP_WIFI_MODE_STA
I (573) wifi:wifi driver task: 3ffc1bfc, prio:23, stack:6656, core=0
I (573) system_api: Base MAC address is not set
I (573) system_api: read default base MAC address from EFUSE
I (603) wifi:wifi firmware version: 6b2834e
I (603) wifi:wifi certification version: v7.0
I (603) wifi:config NVS flash: enabled
I (603) wifi:config nano formating: disabled
I (613) wifi:Init data frame dynamic rx buffer num: 32
I (613) wifi:Init management frame dynamic rx buffer num: 32
I (623) wifi:Init management short buffer num: 32
I (623) wifi:Init dynamic tx buffer num: 32
I (633) wifi:Init static rx buffer size: 1600
I (633) wifi:Init static rx buffer num: 10
I (643) wifi:Init dynamic rx buffer num: 32
I (643) wifi_init: rx ba win: 6
I (643) wifi_init: tcpip mbox: 32
I (653) wifi_init: udp mbox: 6
I (653) wifi_init: tcp mbox: 6
I (653) wifi_init: tcp tx win: 5744
I (663) wifi_init: tcp rx win: 5744
I (663) wifi_init: tcp mss: 1440
I (673) wifi_init: WiFi IRAM OP enabled
I (673) wifi_init: WiFi RX IRAM OP enabled
I (723) phy_init: phy_version 4660,0162888,Dec 23 2020
W (723) phy_init: failed to load RF calibration data (0xffffffff), falling back to full calibration
I (1093) wifi:mode : sta (4c:11:ae:ea:69:fc)
I (1093) wifi:enable tsf
I (1093) static_ip: wifi_init_sta finished.
I (2313) wifi:new:<11,0>, old:<1,0>, ap:<255,255>, sta:<11,0>, prof:1
I (3053) wifi:state: init -> auth (b0)
I (3083) wifi:state: auth -> assoc (0)
I (3213) wifi:new:<11,2>, old:<11,0>, ap:<255,255>, sta:<11,2>, prof:1
I (3213) wifi:state: assoc -> run (10)
I (3233) wifi:connected with myssid, aid = 2, channel 11, 40D, bssid = 34:29:12:43:c5:40
I (3233) wifi:security: WPA2-PSK, phy: bgn, rssi: -7
I (3243) wifi:pm start, type: 1

I (3253) esp_netif_handlers: sta ip: 192.168.4.2, mask: 255.255.255.0, gw: 192.168.4.1
I (3253) static_ip: static ip:192.168.4.2
I (3253) static_ip: connected to ap SSID:myssid password:mypassword
W (3273) wifi:<ba-add>idx:0 (ifx:0, 34:29:12:43:c5:40), tid:5, ssn:2, winSize:64
I (3303) wifi:AP's beacon interval = 102400 us, DTIM period = 1
I (3303) wifi:new:<11,0>, old:<11,2>, ap:<255,255>, sta:<11,0>, prof:1
I (5203) static_ip: Resolved IPv4 address: 119.9.92.99
```
