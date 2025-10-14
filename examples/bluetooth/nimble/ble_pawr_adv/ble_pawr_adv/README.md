| Supported Targets | ESP32-C6 |
| ----------------- | -------- |


# Important Note
*This example currently requires an external Bluetooth controller supporting PAwR functionality, as the ESP chips listed above do not have native controller support for PAwR features and under development phase*

# BLE Periodic Advertiser With Response (PAwR) Advertiser Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)
 BLE PAwR Advertiser Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example starts PAwR  advertising with configurable subevents and response slots.

It uses external Bluetooth controller and NimBLE stack based BLE host.

This example aims at understanding PAwR advertisement and related NimBLE APIs.

To test this demo, any BLE scanner supporting PAwR can be used.(check /examples/ble_pawr_adv/ble_pawr_sync)

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
I (504) NimBLE_BLE_PAwR: Device Address 40:4c:ca:46:1f:e2
I (504) NimBLE: GAP procedure initiated: extended advertise; instance=0
 
I (514) NimBLE_BLE_PAwR: instance 0 started (periodic)
 
I (514) NimBLE_BLE_PAwR: [Request] data: 0, subevt start:0, subevt count:5
I (534) main_task: Returned from app_main()
I (664) NimBLE_BLE_PAwR: [Request] data: 5, subevt start:5, subevt count:2
I (814) NimBLE_BLE_PAwR: [Request] data: 7, subevt start:7, subevt count:2
I (964) NimBLE_BLE_PAwR: [Request] data: 9, subevt start:9, subevt count:2
I (1114) NimBLE_BLE_PAwR: [Request] data: b, subevt start:1, subevt count:2

I (1163054) NimBLE_BLE_PAwR: [Response] subevent:0, response_slot:114, data_length:1
I (1163054) NimBLE_BLE_PAwR: data: 0x8c, 0x0
I (1163164) NimBLE_BLE_PAwR: [Request] data: 91, subevt start:5, subevt count:2

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
