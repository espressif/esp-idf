| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | -------- | --------- | -------- | --------- | -------- | --------- |

# BLE Periodic Advertiser With Response (PAwR) Sync Connection Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)
 BLE PAwR Advertiser Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates PAwR connection functionality as PAwR Sync.

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
I (...) NimBLE: GAP procedure initiated: extended discovery;
I (...) main_task: Returned from app_main()

I (...) NimBLE_BLE_PAwR_CONN: [Disc advertiser] addr xx:xx:xx:xx:xx:xx, props: 0x0, rssi:...
I (...) NimBLE_BLE_PAwR_CONN: Create sync
I (...) NimBLE_BLE_PAwR_CONN: [Periodic Sync Established] sync handle:0, num_subevents:0xa
I (...) NimBLE_BLE_PAwR_CONN: subevent_interval:0x34, slot_delay:0x5,slot_spacing:0xa
I (...) NimBLE_BLE_PAwR_CONN: [Subevent Sync OK] sync handle:0, sync_subevents:10

I (...) NimBLE_BLE_PAwR_CONN: [Periodic Adv Report] handle:0, event_counter(...), subevent(0)
I (...) NimBLE_BLE_PAwR_CONN: [Periodic Adv Report] handle:0, event_counter(...), subevent(0)
W (...) NimBLE_BLE_PAwR_CONN: [RSP Data Set] sync handle: 0, subev(0), rsp_slot(2), rc(0x0)
I (...) NimBLE_BLE_PAwR_CONN: [Periodic Adv Report] handle:0, event_counter(...), subevent(1)
W (...) NimBLE_BLE_PAwR_CONN: [RSP Data Set] sync handle: 0, subev(1), rsp_slot(2), rc(0x0)
I (...) NimBLE_BLE_PAwR_CONN: [Periodic Adv Report] handle:0, event_counter(...), subevent(2)
W (...) NimBLE_BLE_PAwR_CONN: [RSP Data Set] sync handle: 0, subev(2), rsp_slot(2), rc(0x0)
I (...) NimBLE_BLE_PAwR_CONN: ...

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
