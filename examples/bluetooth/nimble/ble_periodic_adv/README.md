| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S3 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

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

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

There is this console output when periodic_adv is started:

```
I (...) NimBLE_BLE_PERIODIC_ADV: BLE Host Task Started
I (...) NimBLE: Device Address:
I (...) NimBLE: xx:xx:xx:xx:xx:xx
I (...) NimBLE:

I (...) NimBLE: GAP procedure initiated: extended advertise; instance=0
I (...) NimBLE: instance 0 started (periodic)

I (...) main_task: Returned from app_main()
```

## Note
* Periodic sync transfer is not implemented for now.

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
