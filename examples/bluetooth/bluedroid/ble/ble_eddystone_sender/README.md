| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP-IDF Eddystone Example

This example demonstrates Eddystone-compatible BLE sending of eddystone frame, including UID and URL and TLM.

Eddystone is an open beacon protocol specification from Google aimed at improving “proximity-based experiences”
with support for both Android and iOS smart device platforms.

Learn more on [Beacons](https://developers.google.com/nearby/notifications/get-started) and [Eddystone](https://github.com/google/eddystone).

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

Switch sending mode UID or URL or TLM:
Go to: `idf.py menuconfig --> Component config --> Example 'EDDYSTONE SEND' -->` then select the `EXMAPLE_EDDYSTONE_SEND` option.

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
I (532) EDDYSTONE_DEMO: Register callback
I (532) EDDYSTONE_DEMO: Raw advertising data set complete
I (532) EDDYSTONE_DEMO: Advertising started successfully
I (542) main_task: Returned from app_main()
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
