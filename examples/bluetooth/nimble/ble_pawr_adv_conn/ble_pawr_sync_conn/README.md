| Supported Targets | ESP32-C6 |
| ----------------- | -------- |

## Important Note
*This example currently requires an external Bluetooth controller supporting PAwR functionality, as the ESP chips listed above do not have native controller support for PAwR features and under development phase*

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
I (535) NimBLE: GAP procedure initiated: extended discovery;
 
I (545) main_task: Returned from app_main()
I (6595) NimBLE_BLE_PAwR: [Disc advertiser] addr 40:4c:ca:46:1f:e2, props: 0x0, rssi:-45
I (6595) NimBLE_BLE_PAwR: Create sync
I (6975) NimBLE_BLE_PAwR: [Periodic Sync Established] sync handle:0, num_subevents:0xa
I (6975) NimBLE_BLE_PAwR: subevent_interval:0x34, slot_delay:0x5,slot_spacing:0xa
I (6985) NimBLE_BLE_PAwR: [Subevent Sync OK] sync handle:0, sync_subevents:10
I (7885) NimBLE_BLE_PAwR: [Periodic Adv Report] handle:0, event_counter(2), subevent(4)
W (7885) NimBLE_BLE_PAwR: [RSP Data Set] sync handle: 0, subev(4), rsp_slot(2), rc(0x0)
W (7955) NimBLE_BLE_PAwR: [Connection established], conn_handle = 0x00, sync handle = 0x0, status = 0x0
 
I (7955) NimBLE_BLE_PAwR: handle=0 our_ota_addr_type=0 our_ota_addr=60:55:f9:f6:02:5e 
I (7965) NimBLE_BLE_PAwR: our_id_addr_type=0 our_id_addr=60:55:f9:f6:02:5e 
I (7965) NimBLE_BLE_PAwR: peer_ota_addr_type=0 peer_ota_addr=40:4c:ca:46:1f:e2 
I (7975) NimBLE_BLE_PAwR: peer_id_addr_type=0 peer_id_addr=40:4c:ca:46:1f:e2 
I (7985) NimBLE_BLE_PAwR: conn_itvl=40 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
