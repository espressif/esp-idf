| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | -------- |

# BLE Periodic Advertiser Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example starts periodic advertising with non resolvable private address.

It uses Bluetooth controller and NimBLE stack based BLE host.

This example aims at understanding periodic advertisement and  related NimBLE APIs.


To test this demo, any BLE Periodic Sync app can be used.


Note :

* Make sure to run `python -m pip install --user -r $IDF_PATH/requirements.txt -r $IDF_PATH/tools/ble/requirements.txt` to install the dependency packages needed.
* Currently this Python utility is only supported on Linux (BLE communication is via BLuez + DBus).

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Configure the project

Open the project configuration menu:

```bash
idf.py menuconfig
```

In the `Example Configuration` menu:

* Select I/O capabilities of device from `Example Configuration --> I/O Capability`, default is `Just_works`.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

There is this console output when periodic_adv is started:

```
I (313) BTDM_INIT: BT controller compile version [2ee0168]
I (313) phy_init: phy_version 912,d001756,Jun  2 2022,16:28:07
I (353) system_api: Base MAC address is not set
I (353) system_api: read default base MAC address from EFUSE
I (353) BTDM_INIT: Bluetooth MAC: 84:f7:03:08:14:8e

I (363) NimBLE_BLE_PERIODIC_ADV: BLE Host Task Started
I (373) NimBLE: Device Address:
I (373) NimBLE: d0:42:3a:95:84:05
I (373) NimBLE:

I (383) NimBLE: instance 1 started (periodic)
```

## Note
* Periodic sync transfer is not implemented for now.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
