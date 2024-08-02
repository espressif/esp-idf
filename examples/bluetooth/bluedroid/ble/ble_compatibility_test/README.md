| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP-IDF BLE Compatibility Test Example

This example is to test the Bluetooth compatibility and mobile phones.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Test Scenario

* ESP32 Module:	ESP32-WROOM-32x
* IDF version: 7c29a39d6f9f2dfbefc49d34d34e9267afc7200d
* [Test case](https://github.com/espressif/esp-idf/blob/master/examples/bluetooth/bluedroid/ble/ble_compatibility_test/ble_compatibility_test_case.md)
* Test APK: LightBlue V1.1.3
* [Test report](https://github.com/espressif/esp-idf/blob/master/examples/bluetooth/bluedroid/ble/ble_compatibility_test/esp_ble_compatibility_test_report.md)

### Hardware Required

* A development board with ESP32/ESP32-C3/ESP32-C2/ESP32-H2/ESP32-S3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
