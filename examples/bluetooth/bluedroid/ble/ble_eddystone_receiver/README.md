| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# ESP-IDF Eddystone Example

This example demonstrates Eddystone-compatible BLE scanning of eddystone frame, including UID and URL.

Eddystone is an open beacon protocol specification from Google aimed at improving “proximity-based experiences”
with support for both Android and iOS smart device platforms.

Learn more on [Beacons](https://developers.google.com/nearby/notifications/get-started) and [Eddystone](https://github.com/google/eddystone).

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* A development board with ESP32/ESP32-C3/ESP32-H2/ESP32-C2/ESP32-S3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (0) cpu_start: Starting scheduler on APP CPU.
I (585) BTDM_INIT: BT controller compile version [1342a48]
I (585) system_api: Base MAC address is not set
I (585) system_api: read default base MAC address from EFUSE
I (595) phy_init: phy_version 4670,719f9f6,Feb 18 2021,17:07:07
I (995) EDDYSTONE_DEMO: Register callback
I (1005) EDDYSTONE_DEMO: Start scanning...
I (18135) EDDYSTONE_DEMO: --------Eddystone Found----------
I (18145) EDDYSTONE_DEMO: Device address:: 55 5d 3b ea 11 88
I (18145) EDDYSTONE_DEMO: RSSI of packet:-46 dbm
I (18145) EDDYSTONE_DEMO: Eddystone UID inform:
I (18155) EDDYSTONE_DEMO: Measured power(RSSI at 0m distance):-65 dbm
I (18155) EDDYSTONE_DEMO: EDDYSTONE_DEMO: Namespace ID:0x
I (18165) EDDYSTONE_DEMO: 6e a8 0f c9 3b 13 8e 6d 91 78
I (18175) EDDYSTONE_DEMO: EDDYSTONE_DEMO: Instance ID:0x
I (18175) EDDYSTONE_DEMO: 01 01 01 00 00 00
I (18275) EDDYSTONE_DEMO: --------Eddystone Found----------
I (18275) EDDYSTONE_DEMO: Device address:: 55 5d 3b ea 11 88
I (18275) EDDYSTONE_DEMO: RSSI of packet:-46 dbm
I (18285) EDDYSTONE_DEMO: Eddystone UID inform:
I (18285) EDDYSTONE_DEMO: Measured power(RSSI at 0m distance):-65 dbm
I (18295) EDDYSTONE_DEMO: EDDYSTONE_DEMO: Namespace ID:0x
I (18295) EDDYSTONE_DEMO: 6e a8 0f c9 3b 13 8e 6d 91 78
I (18305) EDDYSTONE_DEMO: EDDYSTONE_DEMO: Instance ID:0x
I (18315) EDDYSTONE_DEMO: 01 01 01 00 00 00
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
