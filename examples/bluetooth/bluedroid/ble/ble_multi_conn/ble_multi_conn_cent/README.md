| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- |

# BLE Multiple Connection Central Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

**This example relies on the BLE controller. please use the chip modules listed under Supported Targets.**

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
I (395) BLE_INIT: ble controller commit:[d2d70d4]
I (405) BLE_INIT: Bluetooth MAC: 60:55:f9:f6:97:de
I (405) phy_init: phy_version 331,5b89037,Mar  3 2025,16:01:12
I (465) phy: libbtbb version: ec2ecba, Mar  3 2025, 16:01:27
I (475) PEER_MANAGER: peer manager init complete
I (485) MULTI_CONN_CENT: Common factor set, status 0
I (485) MULTI_CONN_CENT: Random address set, status 0, addr e6:dd:4b:da:b8:06
I (485) MULTI_CONN_CENT: GATT client register, status 0, app_id 0, gattc_if 3
I (495) MULTI_CONN_CENT: Scanning params set, status 0
I (495) MULTI_CONN_CENT: Random address set, status 0, addr ff:ed:64:bb:b0:f2
I (505) MULTI_CONN_CENT: Scanning start, status 0
I (505) MULTI_CONN_CENT: GATT server register, status 0, app_id 0, gatts_if 4
I (515) MULTI_CONN_CENT: The number handle = 3
I (515) MULTI_CONN_CENT: Advertising data set, status 0
I (535) MULTI_CONN_CENT: Create connection, remote da:20:da:dd:c8:9f
I (535) MULTI_CONN_CENT: Scanning stop
I (535) MULTI_CONN_CENT: Scheduling length set, status 0
I (1275) MULTI_CONN_CENT: Connected, conn_id 0, remote da:20:da:dd:c8:9f, total 1
I (1275) PEER_MANAGER: peer added to list
I (1275) MULTI_CONN_CENT: Open, conn_id 0, status 0
I (1275) MULTI_CONN_CENT: Random address set, status 0, addr f1:91:5d:0b:22:3a
I (1285) MULTI_CONN_CENT: Scanning start, status 0
I (1325) MULTI_CONN_CENT: Create connection, remote e5:4d:be:54:de:1a
I (1335) MULTI_CONN_CENT: Scanning stop
I (1335) MULTI_CONN_CENT: Scheduling length set, status 0
I (2015) MULTI_CONN_CENT: Connected, conn_id 1, remote e5:4d:be:54:de:1a, total 2
I (2015) PEER_MANAGER: peer added to list
I (2015) MULTI_CONN_CENT: Open, conn_id 1, status 0
I (2015) MULTI_CONN_CENT: Random address set, status 0, addr e8:8e:72:3f:6e:29
I (2025) MULTI_CONN_CENT: Scanning start, status 0
I (2075) MULTI_CONN_CENT: Create connection, remote eb:06:b7:d0:d4:48
I (2075) MULTI_CONN_CENT: Scanning stop
I (2075) MULTI_CONN_CENT: Scheduling length set, status 0
I (2755) MULTI_CONN_CENT: Connected, conn_id 2, remote eb:06:b7:d0:d4:48, total 3
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
