| Supported Targets | ESP32-C6 |
| ----------------- | -------- |

# Important Note
*This example currently requires an external Bluetooth controller supporting PAwR functionality, as the ESP chips listed above do not have native controller support for PAwR features and under development phase*

# BLE Periodic Advertiser With Response (PAwR) Sync Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)
 BLE PAwR Advertiser Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example demonstrates synchronization PAwR sync using the NimBLE stack and related APIS.

To test this demo, any BLE advertiser supporting PAwR can be used.(check /examples/ble_pawr_adv/ble_pawr_adv).

## Key Features
- Synchronization with PAwR advertising trains
- Configurable synchronization parameters (skip factor, timeout)
- Detailed reporting of PAwR advertisement data
- Handling of synchronization loss events

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
I (608) NimBLE: GAP procedure initiated: extended discovery;
 
I (618) main_task: Returned from app_main()
I (678) NimBLE_BLE_PAwR: [Disc advertiser] addr 40:4c:ca:46:1f:e2, props: 0x0, rssi:-43
I (688) NimBLE_BLE_PAwR: Create sync
I (1308) NimBLE_BLE_PAwR: [Periodic Sync Established] sync handle:0, num_subevents:0xa
I (1308) NimBLE_BLE_PAwR: subevent_interval:0x3c, slot_delay:0x6,slot_spacing:0x2
I (1308) NimBLE_BLE_PAwR: [Subevent Sync OK] sync handle:0, sync_subevents:4
I (1318) NimBLE_BLE_PAwR: [Periodic Adv Report] handle:0, event_counter(1521), subevent(0)
I (2058) NimBLE_BLE_PAwR: [Periodic Adv Report] handle:0, event_counter(1522), subevent(0)
I (2058) NimBLE_BLE_PAwR: [RSP Data Set] sync handle: 0, subev(0), rsp_slot(2), rc(0x0)
I (2208) NimBLE_BLE_PAwR: [Periodic Adv Report] handle:0, event_counter(1522), subevent(2)

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
