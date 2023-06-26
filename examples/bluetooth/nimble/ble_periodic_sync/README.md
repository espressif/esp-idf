| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | -------- |

# BLE Periodic Sync Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example performs passive scan for non-connectable non-scannable extended advertisement, it then establishes the periodic sync with the advertiser and then listens to the periodic advertisements.


It uses ESP32C3's Bluetooth controller and NimBLE stack based BLE host.

This example aims at understanding BLE periodic sync establishment and periodic advertisement reports.

To test this demo, use any periodic advertiser with uses extended adv data as "ESP_PERIODIC_ADV".


Note :

* Make sure to run `python -m pip install --user -r $IDF_PATH/requirements.txt -r $IDF_PATH/tools/ble/requirements.txt` to install the dependency packages needed.
* Currently this Python utility is only supported on Linux (BLE communication is via BLuez + DBus).

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* A development board with ESP32/ESP32-C3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Configure the Project

Open the project configuration menu:

```bash
idf.py menuconfig
```

In the `Example Configuration` menu:

* Change the `Peer Address` option if needed.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

This is the console output on successful periodic sync:

```
I (311) BTDM_INIT: BT controller compile version [3a49744]
I (311) phy_init: phy_version 912,d001756,Jun  2 2022,16:28:07
I (351) system_api: Base MAC address is not set
I (351) system_api: read default base MAC address from EFUSE
I (351) BTDM_INIT: Bluetooth MAC: 84:f7:03:08:14:8e

I (361) NimBLE_BLE_PERIODIC_SYNC: BLE Host Task Started
I (941) NimBLE: Periodic sync event :

I (941) NimBLE: Periodic adv report event:

I (4241) NimBLE: Periodic adv report event:

I (7541) NimBLE: Periodic adv report event:

I (10841) NimBLE: Periodic adv report event:
```


## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
