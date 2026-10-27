| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- |

# Wi-Fi Fast Connect Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example shows the roaming app's fast connect and its check after a connection:

* **Fast connect** remembers the AP each connection ends on, and the next connection, after a reboot or a lost link, tries that AP alone instead of scanning every channel first. If that connection fails, the next attempt scans every channel and connects to the strongest AP, which is then remembered.
* **The check after a connection** scans once, after the roaming backoff time and once the application is quiet, and roams only to an AP at least `ESP_WIFI_ROAMING_JOIN_CHECK_RSSI_DIFF` dB (15 by default) stronger.

The example logs the time from boot to the first IP address, which is what fast connect shortens. Compare it with and without a remembered AP: the saving depends on the network and on how many channels a scan covers.

For how the roaming app works, and how fast connect fits with its other triggers, read `components/esp_wifi/wifi_apps/roaming_app/src/README.md` and `ROAMING_FLOW.md`.

## How to use example

### Configure the project

Open the project configuration menu (`idf.py menuconfig`).

In the `Example Configuration` menu:

* Set `Wi-Fi SSID` and `Wi-Fi Password`.
* Optional: `Report the application busy for this long after each IP address (ms)` makes the example report a reply outstanding (`esp_wifi_roaming_set_app_hold()`) after each IP address, so the check waits for it.
* Optional: `Restart this long after each boot's IP address (s)` restarts the board again and again, so boot to IP can be measured with the AP remembered.

`sdkconfig.defaults` turns on the roaming app with fast connect and the check, and turns its low RSSI and periodic scan triggers off, so the station scans while connected only once per connection. Turn them on in `Component config` > `Wi-Fi` > `Configure roaming App` to use them together.

The example sets the station configuration only when the SSID or password changes. Fast connect's record lives in the saved station configuration, so an application that sets it at every boot scans every channel at every boot.

### Build and Flash

Build the project and flash it to the board, then run the monitor tool to view the serial output:

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for all the steps to configure and use the ESP-IDF to build projects.

## Example Output

The first boot on a new network scans every channel, connects to the strongest AP and remembers it:

```
I (xxx) fast connect example: saving a new network
I (xxx) fast connect example: connected on channel 11, <ms> ms after boot
I (xxx) ROAM: fast connect: remembered xx:xx:xx:xx:xx:xx on channel 11
I (xxx) fast connect example: boot to IP: <ms> ms
I (xxx) ROAM: join check: waiting for the roam backoff time (15 s after the connection)
I (xxx) ROAM: join check: scanning <ms> ms after the IP address
I (xxx) ROAM: join check: no AP 15 dB stronger than the current AP (<rssi> dBm): stay
```

Every boot after it connects to the remembered AP directly:

```
I (xxx) fast connect example: keeping the saved station configuration (AP remembered: yes)
I (xxx) fast connect example: connected on channel 11, <ms> ms after boot
I (xxx) ROAM: fast connect: connected to the remembered AP xx:xx:xx:xx:xx:xx on channel 11
I (xxx) fast connect example: boot to IP: <ms> ms
```

If the remembered AP does not take the connection, the next attempt scans every channel:

```
W (xxx) ROAM: fast connect: the remembered AP xx:xx:xx:xx:xx:xx on channel 6 failed the connection (reason 201): every channel, strongest first, from the next attempt
```

The lines are the example's and the roaming app's; `<ms>` and `<rssi>` stand for what your board logs.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
