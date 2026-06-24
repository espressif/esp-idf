| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S3 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

# BLE Periodic Sync Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example performs passive scan for non-connectable non-scannable extended advertisement, it then establishes the periodic sync with the advertiser and then listens to the periodic advertisements.


It uses Bluetooth controller and NimBLE stack based BLE host.

This example aims at understanding BLE periodic sync establishment and periodic advertisement reports.

To test this demo, use the `ble_periodic_adv` example or any periodic advertiser with SID 2.


Note :

* Make sure to run `python -m pip install --user -r $IDF_PATH/requirements.txt -r $IDF_PATH/tools/ble/requirements.txt` to install the dependency packages needed.
* Currently this Python utility is only supported on Linux (BLE communication is via BLuez + DBus).

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* A development board with a supported SoC (e.g., ESP32-C3, ESP32-C6, ESP32-H2, ESP32-S3, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

This is the console output on successful periodic sync:

```
I (...) NimBLE_BLE_PERIODIC_SYNC: BLE Host Task Started
I (...) NimBLE: GAP procedure initiated: extended discovery;

I (...) main_task: Returned from app_main()
I (...) NimBLE: Periodic sync event :

I (...) NimBLE: status : 0
periodic_sync_handle : 0
sid : 2

I (...) NimBLE: adv addr :
I (...) NimBLE: xx xx xx xx xx xx
I (...) NimBLE: adv_phy : 2m

I (...) NimBLE: per_adv_ival : 192
I (...) NimBLE: adv_clk_accuracy : 4

I (...) NimBLE: Periodic adv report event:
I (...) NimBLE: sync_handle : 0
I (...) NimBLE: tx_power : 127
I (...) NimBLE: rssi : -..
I (...) NimBLE: data_status : 0
I (...) NimBLE: data_length : 16
I (...) NimBLE: data :
I (...) NimBLE: 45 53 50 5f 50 45 52 49 4f 44 49 43 5f 41 44 56
I (...) NimBLE:

I (...) NimBLE: Periodic adv report event:
I (...) NimBLE: ...
```


## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
