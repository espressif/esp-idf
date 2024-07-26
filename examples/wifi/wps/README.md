| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# Wi-Fi WPS Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to use WPS enrollee in ESP. The WPS protocol simplifies the process of connecting to a Wi-Fi router.

## How to use example

Before project configuration and build, be sure to set the correct chip target using `idf.py set-target <chip_name>`.

### Hardware Required

* A development board with ESP32/ESP32-S2/ESP32-C3/ESP32-S3/ESP32-C2 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

### Configure the project

Open the project configuration menu (`idf.py menuconfig`).

In the `Example Configuration` menu:

* Use `WPS mode` to select the type.
* Select `PBC`, `PIN` or `disable`.

In `PBC` mode, the ESP will wait for the WPS initialization (usually by pressing WPS button on the Wi-Fi router) and then will connect to the router.

In `PIN` mode, the ESP will enter the WPS mode and you'll see a pin code on the terminal. Enter this pin code in your router and then the ESP will connect to it.

(See your router manual and configuration for WPS compatibility.)

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for all the steps to configure and use the ESP-IDF to build projects.

* [ESP-IDF Getting Started Guide on ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)
* [ESP-IDF Getting Started Guide on ESP32-S2](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/index.html)
* [ESP-IDF Getting Started Guide on ESP32-C3](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/get-started/index.html)
* [ESP-IDF Getting Started Guide on ESP32-S3](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s3/get-started/index.html)
* [ESP-IDF Getting Started Guide on ESP32-C2](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c2/get-started/index.html)


## Example Output

As you run the example, you will see the following log:

```
I (814) example_wps: start wps...
I (814) example_wps: WIFI_EVENT_STA_START
I (7304) wifi:new:<5,1>, old:<1,0>, ap:<255,255>, sta:<5,1>, prof:1
I (8084) wifi:state: init -> auth (b0)
I (8094) wifi:state: auth -> assoc (0)
I (8104) wifi:state: assoc -> run (10)
I (8904) wifi:state: run -> init (17a0)
I (8904) wifi:new:<5,0>, old:<5,1>, ap:<255,255>, sta:<5,1>, prof:1
I (8914) example_wps: WIFI_EVENT_STA_DISCONNECTED
E (8914) wifi:sta is connecting, return error
I (9904) example_wps: WIFI_EVENT_STA_WPS_ER_SUCCESS
I (9914) wifi:new:<5,1>, old:<5,0>, ap:<255,255>, sta:<5,1>, prof:1
I (10654) wifi:state: init -> auth (b0)
I (10664) wifi:state: auth -> assoc (0)
I (10664) wifi:state: assoc -> run (10)
I (10704) wifi:connected with ROUTER_SSID, aid = 3, channel 5, 40U, bssid = xx:xx:xx:xx:xx:xx
I (10704) wifi:security: WPA2-PSK, phy: bgn, rssi: -58
I (10714) wifi:pm start, type: 1

I (10734) wifi:AP's beacon interval = 102400 us, DTIM period = 1
W (11104) wifi:<ba-add>idx:0 (ifx:0, xx:xx:xx:xx:xx:xx), tid:0, ssn:0, winSize:64
I (12094) esp_netif_handlers: sta ip: 192.168.68.113, mask: 255.255.255.0, gw: 192.168.68.1
I (12094) example_wps: got ip: 192.168.68.113
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
