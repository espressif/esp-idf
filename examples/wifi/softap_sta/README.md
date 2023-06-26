| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# Wi-Fi SoftAP & Station Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates how to use the ESP Wi-Fi driver to act as both an Access Point and a Station simultaneously using the SoftAP and Station features.
With NAPT enabled on the softAP interface and the station interface set as the defaut interface this example can be used as Wifi nat router.

## How to use example
### Configure the project

Open the project configuration menu (`idf.py menuconfig`).

In the `Example Configuration` menu:

* Set the Wi-Fi SoftAP configuration.
    * Set `WiFi AP SSID`.
    * Set `WiFi AP Password`.

* Set the Wi-Fi STA configuration.
    * Set `WiFi Remote AP SSID`.
    * Set `WiFi Remote AP Password`.

Optional: If necessary, modify the other choices to suit your needs.

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

## Example Output

There is the console output for this example:

```
I (680) WiFi SoftAP: ESP_WIFI_MODE_AP
I (690) WiFi SoftAP: wifi_init_softap finished. SSID:myssid password:mypassword channel:1
I (690) WiFi Sta: ESP_WIFI_MODE_STA
I (690) WiFi Sta: wifi_init_sta finished.
I (700) phy_init: phy_version 4670,719f9f6,Feb 18 2021,17:07:07
I (800) wifi:mode : sta (58:bf:25:e0:41:00) + softAP (58:bf:25:e0:41:01)
I (800) wifi:enable tsf
I (810) wifi:Total power save buffer number: 16
I (810) wifi:Init max length of beacon: 752/752
I (810) wifi:Init max length of beacon: 752/752
I (820) WiFi Sta: Station started
I (820) wifi:new:<1,1>, old:<1,1>, ap:<1,1>, sta:<1,1>, prof:1
I (820) wifi:state: init -> auth (b0)
I (830) wifi:state: auth -> assoc (0)
E (840) wifi:Association refused temporarily, comeback time 1536 mSec
I (2380) wifi:state: assoc -> assoc (0)
I (2390) wifi:state: assoc -> run (10)
I (2400) wifi:connected with myssid_c3, aid = 1, channel 1, 40U, bssid = 84:f7:03:60:86:1d
I (2400) wifi:security: WPA2-PSK, phy: bgn, rssi: -14
I (2410) wifi:pm start, type: 1

I (2410) wifi:AP's beacon interval = 102400 us, DTIM period = 2
I (3920) WiFi Sta: Got IP:192.168.5.2
I (3920) esp_netif_handlers: sta ip: 192.168.5.2, mask: 255.255.255.0, gw: 192.168.5.1
I (3920) WiFi Sta: connected to ap SSID:myssid_c3 password:mypassword_c3
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
