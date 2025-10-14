| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-S2 | ESP32-S3 | ESP32-P4 | ESP32-H2 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- | -------- | -------- |

# Wi-Fi Examples

This directory contains a range of examples ESP-IDF projects. These are intended to demonstrate the Wi-Fi functionality, and to provide code that you can copy and adapt into your own projects.

ESP Chips with Wi-Fi can use the examples as is.

ESP Chips without Wi-Fi (e.g., ESP32-P4 and ESP32-H2) can work with a [**two-chip solution**](#wi-fi-examples-with-two-chip-solution) described below.

# Example Layout

The examples are grouped into sub-directories by category. Each category directory contains one or more example projects:

* `espnow` is the connectionless Wi-Fi communication protocol.
* `fast_scan` shows how to use fast scan while connecting to an AP.
* `ftm` is a how to use FTM (Fine Timing Measurement) feature to calculate distance between devices.
* `getting_started` contains the basic examples for using Wi-Fi in STA and AP mode.
* `iperf` shows the common performance measurements for two ESP running this example.
* `power_save` contains a how to use power save mode of Wi-Fi.
* `roaming` contains 2 examples that demonstrate roaming capabilities.
* `scan` shows how to scan for all the available APs.
* `smart_config` shows how ESP connects to a target AP with ESPTOUCH.
* `wifi_eap_fast` shows how ESP connects to AP with Wi-Fi enterprise encryption using the EAP-FAST method.
* `wifi_easy_connect` shows how to configure an ESP device as an enrollee using Device Provisioning Protocol (DPP).
* `wpa2_enterprise` shows how ESP connects to AP with WPA2 enterprise encryption.
* `wps` contains a how to use the Wi-Fi Protected Setup protocol.

## Wi-Fi Examples with Two-Chip Solution

Some ESP chips like the ESP32-P4 or ESP32-H2, without built-in Wi-Fi or Bluetooth, can still use these features through a **two-chip solution** via ESP-Hosted.

The two chip solution needs:

- Communication bus like SDIO or SPI between the host and co-processor
- Co-processor flashed with ESP-Hosted co-processor (slave) software
- Host processor flashed with additional software components: [esp-hosted](https://components.espressif.com/components/espressif/esp_hosted/) and [esp-wifi-remote](https://components.espressif.com/components/espressif/esp_wifi_remote/). This is handled by `idf_component.yml`. See the `idf_component.yml` in [iperf](iperf/main/idf_component.yml) for an example.

The ESP32-P4-Function-EV-Board is used here as a demonstration on how to run a Wi-Fi example.

### Running a Wi-Fi example on the ESP32-P4-Function-EV-Board

On this board,

* The ESP32-P4 is connected with the on-board ESP32-C6 (co-processor) using SDIO communication bus. See this ESP-Hosted ESP32-P4 EV Board [documentation](https://github.com/espressif/esp-hosted-mcu/blob/main/docs/esp32_p4_function_ev_board.md) for more information.
* The ESP32-C6 on the ESP32-P4 EV Board is pre-flashed with ESP-Hosted co-processor (slave) software.
* Only the example code needs to be built & flashed on the Host using the steps below.

#### Host Flashing

* Set ESP32-P4 as the target

```sh
idf.py set-target esp32p4
```

* Run Configure

```sh
idf.py menuconfig
```

Set `Component config` -> `Wi-Fi Remote` -> `choose slave target` to `esp32c6`.

Do any other project configuration required for the example.

* Build and Flash

```sh
idf.py -p <host serial port> flash monitor
```

### Co-processor Flashing

**(Optional, but recommended)**

The ESP32-C6 on the ESP32-P4 EV Board is already pre-flashed with ESP-Hosted co-processor software. Host software on the ESP-Hosted component is backward compatible with this co-processor software. It is recommended to upgrade to the latest software via [simplified co-processor OTA](https://github.com/espressif/esp-hosted-mcu/blob/main/docs/esp32_p4_function_ev_board.md#51-ota-updates) or by [manual flashing with ESP-Prog](https://github.com/espressif/esp-hosted-mcu/blob/main/docs/esp32_p4_function_ev_board.md#52-using-esp-prog).

### Troubleshooting

For any technical queries on the two-chip solution, please open an [ESP-Hosted issue](https://github.com/espressif/esp-hosted-mcu/issues) on GitHub.

# More

See the [README.md](../README.md) file in the upper level [examples](../) directory for more information about examples.
