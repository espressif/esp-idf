| Supported Targets | ESP32-C6 |
| ----------------- | -------- |

# Important Note
*This example currently requires an external Bluetooth controller supporting PAwR functionality, as the ESP chips listed above do not have native controller support for PAwR features and under development phase*

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
I (562) NimBLE_BLE_PAwR: instance 0 started (periodic)

I (572) NimBLE_BLE_PAwR: [Request] data: 0, subevt start:0, subevt count:5
I (1842) NimBLE_BLE_PAwR: [Request] data: 17, subevt start:3, subevt count:2
I (1872) NimBLE_BLE_PAwR: [Response] subevent:0, response_slot:2, data_length:10
I (1872) NimBLE_BLE_PAwR: data: 0x00, 0x02, 0x5e, 0x02, 0xf6, 0xf9, 0x55, 0x60, 0x00, 0x00
I (1882) NimBLE: GAP procedure initiated: extended connect;

I (1892) NimBLE_BLE_PAwR: Connection create sent, adv handle = 0, subevent = 5
W (2192) NimBLE_BLE_PAwR: [Connection established], conn_handle = 0x00, Adv handle = 0x0, status = 0x0

I (2192) NimBLE_BLE_PAwR: handle=0 our_ota_addr_type=0 our_ota_addr=40:4c:ca:46:1f:e2 
I (2202) NimBLE_BLE_PAwR: our_id_addr_type=0 our_id_addr=40:4c:ca:46:1f:e2 
I (2212) NimBLE_BLE_PAwR: peer_ota_addr_type=0 peer_ota_addr=60:55:f9:f6:02:5e 
I (2212) NimBLE_BLE_PAwR: peer_id_addr_type=0 peer_id_addr=60:55:f9:f6:02:5e 
I (2222) NimBLE_BLE_PAwR: conn_itvl=40 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0
 
I (2232) NimBLE_BLE_PAwR: [Request] data: 1d, subevt start:9, subevt count:2

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
