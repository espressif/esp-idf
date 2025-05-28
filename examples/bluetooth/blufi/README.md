| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# ESP-IDF Blufi Example

This demo shows how to use the *Blufi* to configure the Wi-Fi connection to AP.

The BluFi for ESP32 is a Wi-Fi network configuration function via Bluetooth channel. It provides a secure protocol to pass Wi-Fi configuration and credentials to the ESP32.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

To test this demo, you need to prepare a mobile phone with blufi application installed. You can download the blufi application from [Android version](https://github.com/EspressifApp/EspBlufi) and [iOS version](https://itunes.apple.com/cn/app/espblufi/id1450614082?mt=8).

Blufi is completely open source, here is the download link:

* [Blufi source code](https://github.com/espressif/esp-idf/tree/master/examples/bluetooth/blufi)
* [BluFi protocol](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-guides/ble/blufi.html)
* [iOS source code](https://github.com/EspressifApp/EspBlufiForiOS)
* [Android source code](https://github.com/EspressifApp/EspBlufi)
* [Bluetooth Network User Guide CN](https://www.espressif.com/sites/default/files/documentation/esp32_bluetooth_networking_user_guide_cn.pdf)

### Hardware Required

* A development board with ESP32/ESP32-C3/ESP32-S3/ESP32-C2 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (0) cpu_start: Starting scheduler on APP CPU.
I (708) wifi:wifi driver task: 3ffbb118, prio:23, stack:6656, core=0
I (708) system_api: Base MAC address is not set
I (708) system_api: read default base MAC address from EFUSE
I (718) wifi:wifi firmware version: 44799ff
I (718) wifi:wifi certification version: v7.0
I (718) wifi:config NVS flash: enabled
I (718) wifi:config nano formatting: disabled
I (728) wifi:Init data frame dynamic rx buffer num: 32
I (728) wifi:Init management frame dynamic rx buffer num: 32
I (738) wifi:Init management short buffer num: 32
I (738) wifi:Init dynamic tx buffer num: 32
I (748) wifi:Init static rx buffer size: 1600
I (748) wifi:Init static rx buffer num: 10
I (748) wifi:Init dynamic rx buffer num: 32
I (758) wifi_init: rx ba win: 6
I (758) wifi_init: tcpip mbox: 32
I (768) wifi_init: udp mbox: 6
I (768) wifi_init: tcp mbox: 6
I (768) wifi_init: tcp tx win: 5744
I (778) wifi_init: tcp rx win: 5744
I (778) wifi_init: tcp mss: 1440
I (788) wifi_init: WiFi IRAM OP enabled
I (788) wifi_init: WiFi RX IRAM OP enabled
I (798) phy_init: phy_version 4670,719f9f6,Feb 18 2021,17:07:07
I (908) wifi:mode : sta (fc:f5:c4:3c:62:90)
I (908) wifi:enable tsf
I (908) BTDM_INIT: BT controller compile version [1342a48]
I (1198) BLUFI_EXAMPLE: BD ADDR: fc:f5:c4:3c:62:92

I (1198) BLUFI_EXAMPLE: BLUFI VERSION 0102

I (1198) BLUFI_EXAMPLE: BLUFI init finish

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
