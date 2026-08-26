| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | -------- | --------- | -------- | --------- | -------- | --------- |

# BLE Periodic Advertiser With Response (PAwR) Advertiser Connection Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)
 BLE PAwR Advertiser Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates PAwR connection functionality as PAwR advertiser.

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
```
I (...) NimBLE_BLE_PAwR_CONN: BLE Host Task Started
I (...) NimBLE_BLE_PAwR_CONN: Device Address xx:xx:xx:xx:xx:xx
I (...) NimBLE: GAP procedure initiated: extended advertise; instance=0

I (...) NimBLE_BLE_PAwR_CONN: instance 0 started (periodic)
I (...) NimBLE_BLE_PAwR_CONN: [Request] data: 0, subevt start:0, subevt count:5
I (...) main_task: Returned from app_main()

I (...) NimBLE_BLE_PAwR_CONN: [Request] data: 5, subevt start:5, subevt count:2
I (...) NimBLE_BLE_PAwR_CONN: [Request] data: 7, subevt start:7, subevt count:2
I (...) NimBLE_BLE_PAwR_CONN: [Request] data: 9, subevt start:9, subevt count:2
I (...) NimBLE_BLE_PAwR_CONN: [Request] data: b, subevt start:1, subevt count:2
I (...) NimBLE_BLE_PAwR_CONN: [Request] data: d, subevt start:3, subevt count:2
I (...) NimBLE_BLE_PAwR_CONN: ...

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
