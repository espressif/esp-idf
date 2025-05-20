| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# Roaming Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates a roaming example using 802.11k and 802.11v APIs. 802.11r(FT-PSK) is enabled by default in this example.

## How to use example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

* A development board with ESP32/ESP32-S2/ESP32-C3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for all the steps to configure and use the ESP-IDF to build projects.

* [ESP-IDF Getting Started Guide on ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)
* [ESP-IDF Getting Started Guide on ESP32-S2](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/index.html)
* [ESP-IDF Getting Started Guide on ESP32-C3](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/get-started/index.html)

## Example Output

All the logs are taken after debug enabled:

Beacon request:
```
I (11125) wpa: scan issued at time=7579442263
I (11125) wpa: BSS: Add new id 1 BSSID b6:fb:e4:4d:6e:22 SSID 'ESPTest' chan 1
I (11135) wpa: BSS: Add new id 2 BSSID d6:fb:e4:4d:6e:22 SSID 'ESPDedicated' chan 1
I (11165) wpa: BSS: Add new id 3 BSSID c6:fb:e4:4d:6e:22 SSID 'ESPGuestNetwork' chan 1
I (11185) wpa: BSS: Add new id 4 BSSID b4:fb:e4:4d:6e:22 SSID 'ESPIndia' chan 1
I (11245) wpa: BSS: Add new id 5 BSSID b8:27:eb:3b:4a:59 SSID 'wpa2_enterprise' chan 6
I (11365) wpa: BSS: Add new id 6 BSSID 38:94:ed:34:07:66 SSID 'Nighthawk' chan 6
I (11365) wpa: BSS: Add new id 7 BSSID 10:da:43:dc:99:20 SSID 'NETGEAR40' chan 6
I (12435) wpa: BSS: Add new id 8 BSSID c6:fb:e4:4d:7c:89 SSID 'ESPGuestNetwork' chan 11
I (12445) wpa: BSS: Add new id 9 BSSID d6:fb:e4:4d:7c:89 SSID 'ESPDedicated' chan 11
I (12455) wpa: BSS: Add new id 10 BSSID b6:fb:e4:4d:7c:89 SSID 'ESPTest' chan 11
I (12485) wpa: BSS: Add new id 11 BSSID b4:fb:e4:4d:7c:89 SSID 'ESPIndia' chan 11
I (12825) wpa: scan done received
I (12845) wpa: action frame sent
I (12845) wpa: action frame sent
```

Sta's moving from one AP to another on BTM request:

```
I (379479) wpa: WNM: RX action 7 from 50:3e:aa:26:35:42
I (379479) wpa: WNM: BSS Transition Management Request: dialog_token=1 request_mode=0x1 disassoc_timer=0 validity_interval=1
I (379489) wpa: WNM: Neighbor report tag 52
I (379499) wpa: WNM: Subelement id=3 len=1
I (379499) wpa: WNM: BSS Transition Candidate List
I (379509) wpa: 0: b4:e6:2d:eb:1d:76 info=0x0 op_class=81 chan=1 phy=7 pref=255
I (379519) wpa: WNM: Candidate list valid for 102 ms
I (379519) wpa: WNM: Scan only for a specific BSSID since there is only a single candidate b4:e6:2d:eb:1d:76
I (379539) wpa: scan issued at time=9979439869
I (379539) wpa: BSS: Add new id 12 BSSID b4:e6:2d:eb:1d:76 SSID 'roaming_test123' chan 6
I (381979) wpa: scan done received
I (381979) wpa: WNM: Process scan results for BSS Transition Management
I (381979) wpa: WNM: Found an acceptable preferred transition candidate BSS b4:e6:2d:eb:1d:76 (RSSI -7)
I (381989) wpa: WNM: Transition to BSS b4:e6:2d:eb:1d:76 based on BSS Transition Management Request after_new_scan=1)
I (381999) wpa: WNM: Sending successful BSS Transition Management Response
I (382009) wpa: WNM: Send BSS Transition Management Response to 50:3e:aa:26:35:42 dialog_token=1 status=0 reason=0 delay=0
I (382019) wpa: action frame sent
I (382029) wpa: WNM: Issuing connect
I (382029) wifi:state: run -> init (0)
I (382029) wifi:pm stop, total sleep time: 102136757 us / 109258805 us

I (382039) wifi:new:<6,0>, old:<6,0>, ap:<255,255>, sta:<6,0>, prof:1
I (382769) wifi:new:<6,1>, old:<6,0>, ap:<255,255>, sta:<6,1>, prof:1
I (384689) wifi:state: init -> auth (b0)
I (384699) wifi:state: auth -> assoc (0)
I (384709) wifi:state: assoc -> run (10)
I (384729) wifi:connected with roaming_test123, aid = 1, channel 6, 40U, bssid = b4:e6:2d:eb:1d:76
I (384729) wifi:security: WPA2-PSK, phy: bgn, rssi: -7
I (384729) wifi:pm start, type: 1

I (384789) wifi:AP's beacon interval = 102400 us, DTIM period = 2
I (385609) esp_netif_handlers: sta ip: 192.168.4.2, mask: 255.255.255.0, gw: 192.168.4.1
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
