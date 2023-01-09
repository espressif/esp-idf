| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# Wi-Fi Fast Scan Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to use the scan functionality of the Wi-Fi driver of ESP for connecting to an AP.

Two scan methods are supported: fast scan and all channel scan.

* `fast scan`: in this mode, scan finishes right after a matching AP is detected, even if channels are not completely scanned. You can set thresholds for signal strength, as well as select desired authentication modes provided by the AP's. The Wi-Fi driver will ignore AP's that fail to meet mentioned criteria.

* `all channel scan`: scan will end only after all channels are scanned; the Wi-Fi driver will store 4 of the fully matching AP's. Sort methods for AP's include rssi and authmode. After the scan, the Wi-Fi driver selects the AP that fits best based on the sort.

After the scan, the Wi-Fi driver will try to connect. Because it needs to to allocate precious dynamic memory to store matching AP's, and, most of the cases, connect to the AP with the strongest reception, it does not need to record all the AP's matched. The number of matches stored is limited to 4 in order to limit dynamic memory usage. Among the 4 matches,  AP's are allowed to carry the same SSID name and all possible authentication modes - Open, WEP, WPA and WPA2.

## How to use example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

* A development board with ESP32/ESP32-S2/ESP32-C3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

### Configure the project

Open the project configuration menu (`idf.py menuconfig`).

In the `Example Configuration` menu:

* Use `WiFi SSID` to set the SSID.
* Use `WiFi Password` to set the password.

Optional: If you need, change the other options according to your requirements.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for all the steps to configure and use the ESP-IDF to build projects.

* [ESP-IDF Getting Started Guide on ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)
* [ESP-IDF Getting Started Guide on ESP32-S2](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/index.html)
* [ESP-IDF Getting Started Guide on ESP32-C3](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/get-started/index.html)

## Example Output

As you run the example, you will see the following log:

```
I (616) wifi:wifi firmware version: 6bff005
I (616) wifi:wifi certification version: v7.0
I (616) wifi:config NVS flash: enabled
I (616) wifi:config nano formating: disabled
I (626) wifi:Init data frame dynamic rx buffer num: 32
I (626) wifi:Init management frame dynamic rx buffer num: 32
I (636) wifi:Init management short buffer num: 32
I (636) wifi:Init dynamic tx buffer num: 32
I (646) wifi:Init static rx buffer size: 1600
I (646) wifi:Init static rx buffer num: 10
I (646) wifi:Init dynamic rx buffer num: 32
I (656) wifi_init: rx ba win: 6
I (656) wifi_init: tcpip mbox: 32
I (666) wifi_init: udp mbox: 6
I (666) wifi_init: tcp mbox: 6
I (666) wifi_init: tcp tx win: 5744
I (676) wifi_init: tcp rx win: 5744
I (676) wifi_init: tcp mss: 1440
I (686) wifi_init: WiFi IRAM OP enabled
I (686) wifi_init: WiFi RX IRAM OP enabled
I (696) phy_init: phy_version 4660,0162888,Dec 23 2020
I (796) wifi:mode : sta (xx:xx:xx:xx:xx:xx)
I (796) wifi:enable tsf
I (806) wifi:new:<8,1>, old:<1,0>, ap:<255,255>, sta:<8,1>, prof:1
I (806) wifi:state: init -> auth (b0)
I (826) wifi:state: auth -> assoc (0)
I (836) wifi:state: assoc -> run (10)
I (876) wifi:connected with SSID, aid = 1, channel 8, 40U, bssid = xx:xx:xx:xx:xx:xx
I (876) wifi:security: WPA2-PSK, phy: bgn, rssi: -56
I (886) wifi:pm start, type: 1

I (966) wifi:AP's beacon interval = 102400 us, DTIM period = 1
W (1106) wifi:<ba-add>idx:0 (ifx:0, xx:xx:xx:xx:xx:xx), tid:0, ssn:0, winSize:64
I (2086) scan: got ip:192.168.68.110
I (2086) esp_netif_handlers: sta ip: 192.168.68.110, mask: 255.255.255.0, gw: 192.168.68.1

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
