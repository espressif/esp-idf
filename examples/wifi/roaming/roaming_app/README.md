| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- |

# Wi-Fi Station Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows how to use the Wi-Fi Roaming App  functionality of the Wi-Fi driver of ESP for efficient roaming between compatible APs.

## How to use example

### Configure the project

Open the project configuration menu (`idf.py menuconfig`).

In the `Example Configuration` menu:

* Set the Wi-Fi configuration.
    * Set `WiFi SSID`.
    * Set `WiFi Password`.

* Optional: `Restart this long after each boot's IP address (s)` restarts the board again and again, so the time from boot to IP address can be measured with fast connect's remembered AP.

* To better understand how to configure the app in Components  --> Wi-Fi --> Roaming App settings,
  please go read the README at components/esp_wifi/wifi_apps/roaming_app/src.
Optional: If you need, change the other options according to your requirements.

### Fast connect

`sdkconfig.defaults` turns on the roaming app's fast connect (`CONFIG_ESP_WIFI_ROAMING_FAST_CONNECT`). Fast connect remembers the AP each connection ends on, and the next connection, after a reboot or a lost link, tries that AP alone instead of scanning every channel first. If that connection fails, the next attempt scans every channel and connects to the strongest AP, which is then remembered. The low RSSI and periodic scan triggers still move the station off a remembered AP that has grown weak.

Fast connect's record lives in the saved station configuration, so with fast connect the example sets the station configuration only when the SSID or password changes. An application that sets it at every boot replaces the record, and scans every channel at every boot. Without fast connect, the example sets the station configuration at every boot, as before, which also replaces a record left by an earlier firmware that had fast connect on.

The example logs the time from boot to the first IP address, which is what fast connect shortens. Compare it with and without a remembered AP: the saving depends on the network and on how many channels a scan covers.

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for all the steps to configure and use the ESP-IDF to build projects.

* [ESP-IDF Getting Started Guide on ESP32](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/index.html)
* [ESP-IDF Getting Started Guide on ESP32-S2](https://docs.espressif.com/projects/esp-idf/en/latest/esp32s2/get-started/index.html)
* [ESP-IDF Getting Started Guide on ESP32-C3](https://docs.espressif.com/projects/esp-idf/en/latest/esp32c3/get-started/index.html)

## Example Output
Note that the output, in particular the order of the output, may vary depending on the environment.

Console output if station roams between APs successfully:
```
I (827) wifi roaming app: wifi_init_sta finished.
I (837) wifi:new:<1,0>, old:<1,0>, ap:<255,255>, sta:<1,0>, prof:5
I (837) wifi:state: init -> auth (b0)
I (847) wifi:state: auth -> assoc (0)
I (857) wifi:Association refused temporarily, comeback time 1000 (TUs)
I (1877) wifi:state: assoc -> assoc (0)
I (1917) wifi:state: assoc -> run (10)
I (2997) wifi:connected with roaming_setup, aid = 1, channel 1, BW20, bssid = de:bd:5b:93:55:99
I (2997) wifi:security: WPA2-PSK, phy: bgn, rssi: -58
I (3007) wifi:pm start, type: 1

I (3087) wifi:AP's beacon interval = 102400 us, DTIM period = 2
I (10467) ROAM: roaming_app_rssi_low_handler:bss rssi is=-61
I (10467) ROAM: Issued Scan
I (10857) ROAM: Could not find a better AP with the threshold set to 1
I (63017) ROAM: Issued Scan
I (63347) ROAM: Found a better AP 7c:50:79:06:18:46 at channel 9
I (63347) wifi:state: run -> init (cc0)
I (63347) wifi:pm stop, total sleep time: 54401305 us / 60339946 us

I (63347) wifi:new:<1,0>, old:<1,0>, ap:<255,255>, sta:<1,0>, prof:5
E (63357) ROAM: Neighbor report is empty
I (63357) wifi roaming app: station disconnected during roaming
I (63377) wifi roaming app: connect to the AP fail
I (63377) ROAM: Disconnecting and connecting to 7c:50:79:06:18:46 on account of better rssi
I (63497) wifi:new:<9,0>, old:<1,0>, ap:<255,255>, sta:<9,0>, prof:5
I (63837) wifi:state: init -> auth (b0)
I (63847) wifi:state: auth -> assoc (0)
I (63887) wifi:Association refused temporarily, comeback time 1000 (TUs)
I (64907) wifi:state: assoc -> assoc (0)
I (64977) wifi:state: assoc -> run (10)
I (65007) wifi:connected with roaming_setup, aid = 1, channel 9, BW20, bssid = 7c:50:79:06:18:46
I (65007) wifi:security: WPA2-PSK, phy: bgn, rssi: -32
I (65027) wifi:pm start, type: 1
```

With fast connect, the first boot on a new network, or after a firmware without fast connect, scans every channel, connects to the strongest AP and remembers it:

```
I (xxx) wifi roaming app: saving the station configuration
I (xxx) wifi roaming app: connected on channel 11, <ms> ms after boot
I (xxx) ROAM: fast connect: remembered xx:xx:xx:xx:xx:xx on channel 11
I (xxx) wifi roaming app: boot to IP: <ms> ms
```

Every boot after it connects to the remembered AP directly:

```
I (xxx) wifi roaming app: keeping the saved station configuration (AP remembered: yes)
I (xxx) wifi roaming app: connected on channel 11, <ms> ms after boot
I (xxx) ROAM: fast connect: connected to the remembered AP xx:xx:xx:xx:xx:xx on channel 11
I (xxx) wifi roaming app: boot to IP: <ms> ms
```

If the remembered AP does not take the connection, the next attempt scans every channel:

```
W (xxx) ROAM: fast connect: the remembered AP xx:xx:xx:xx:xx:xx on channel 6 failed the connection (reason 201): every channel, strongest first, from the next attempt
```

`<ms>` stands for what your board logs.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
