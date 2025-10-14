| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- |

# BLE Multiple Connection Peripheral Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

**This example relies on the BLE controller. Please use the chip modules listed under Supported Targets.**

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* At least two development board with ESP32-C6/ESP32-H2 SoC (e.g., ESP32-C6-DevKitC, ESP32-H2-DevKitC, etc.)
* USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

This is the console output on successful connection:

```
I (402) BLE_INIT: ble controller commit:[d2d70d4]
I (402) BLE_INIT: Bluetooth MAC: 60:55:f9:f6:97:92
I (402) phy_init: phy_version 331,5b89037,Mar  3 2025,16:01:12
I (462) phy: libbtbb version: ec2ecba, Mar  3 2025, 16:01:27
I (462) MULTI_CONN_PRPH: app_main init bluetooth
I (482) MULTI_CONN_PRPH: GATT server register, status 0, app_id 0, gatts_if 3
I (482) MULTI_CONN_PRPH: The number handle = 3
I (482) MULTI_CONN_PRPH: Advertising data set, status 0
I (482) MULTI_CONN_PRPH: Random address set, status 0, addr da:20:da:dd:c8:9f
I (492) MULTI_CONN_PRPH: Advertising start, status 0
I (8212) MULTI_CONN_PRPH: Connected, conn_id 0, remote ff:ed:64:bb:b0:f2, total 1
I (8212) MULTI_CONN_PRPH: Advertising stop, status 0
I (8212) MULTI_CONN_PRPH: Random address set, status 0, addr e5:4d:be:54:de:1a
I (8222) MULTI_CONN_PRPH: Advertising start, status 0
I (8952) MULTI_CONN_PRPH: Connected, conn_id 1, remote f1:91:5d:0b:22:3a, total 2
I (8952) MULTI_CONN_PRPH: Advertising stop, status 0
I (8952) MULTI_CONN_PRPH: Random address set, status 0, addr eb:06:b7:d0:d4:48
I (8962) MULTI_CONN_PRPH: Advertising start, status 0
I (9692) MULTI_CONN_PRPH: Connected, conn_id 2, remote e8:8e:72:3f:6e:29, total 3
I (9692) MULTI_CONN_PRPH: Advertising stop, status 0
I (9692) MULTI_CONN_PRPH: Random address set, status 0, addr cc:fe:e6:7a:44:f2
I (9702) MULTI_CONN_PRPH: Advertising start, status 0
I (10432) MULTI_CONN_PRPH: Connected, conn_id 3, remote e3:49:fc:15:62:ca, total 4
I (10432) MULTI_CONN_PRPH: Advertising stop, status 0
I (10432) MULTI_CONN_PRPH: Random address set, status 0, addr f3:5d:a8:3a:d8:59
I (10442) MULTI_CONN_PRPH: Advertising start, status 0
I (11172) MULTI_CONN_PRPH: Connected, conn_id 4, remote e2:64:1a:3a:59:63, total 5
I (11172) MULTI_CONN_PRPH: Advertising stop, status 0
I (11172) MULTI_CONN_PRPH: Random address set, status 0, addr e1:8e:fa:0d:b5:04
I (11182) MULTI_CONN_PRPH: Advertising start, status 0
I (11912) MULTI_CONN_PRPH: Connected, conn_id 5, remote e6:5b:c1:9b:02:b1, total 6
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
