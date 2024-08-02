| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# BLE HTP Cent Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example creates GATT client and performs passive scan, it then connects to peripheral device if the device advertises connectability and the device advertises support for the Health Thermometer service (0x1809) as primary service UUID.

After connection it enables bonding and link encryprion if the `Enable Link Encryption` flag is set in the example config.

It performs four GATT operations against the specified peer:

* Reads the HTP temperature type characteristic.

* After the read operation is completed, writes the HTP measurement interval characteristic.

* After the write operation is completed, subscribes to indications for the HTP temperature measurement characteristic.

* After the subscribe operation is completed, it subscribes to notifications for the HTP intermediate temperature characteristic.

If the peer does not support a required service, characteristic, or descriptor, then the peer lied when it claimed support for the Health Thermometer service! When this happens, or if a GATT procedure fails, this function immediately terminates the connection.

It uses ESP32's Bluetooth controller and NimBLE stack based BLE host.

This example aims at understanding BLE service discovery, connection, encryption and characteristic operations.

To test this demo, use any BLE GATT server app that advertises support for the Health Thermometer service (0x1809) and includes it in the GATT database.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* A development board with ESP32/ESP32-C2/ESP32-C3/ESP32-S3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
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

This is the console output on successful connection:

```
I (356) BLE_INIT: BT controller compile version [921f253]
I (356) phy_init: phy_version 950,f732b06,Feb 15 2023,18:57:12
I (396) BLE_INIT: Bluetooth MAC: 84:f7:03:05:a5:f6

I (396) NimBLE_HTP_CENT: BLE Host Task Started
I (406) NimBLE: GAP procedure initiated: stop advertising.

I (406) NimBLE: GAP procedure initiated: discovery; 
I (406) NimBLE: own_addr_type=0 filter_policy=0 passive=1 limited=0 filter_duplicates=1 
I (416) NimBLE: duration=forever
I (426) NimBLE: 

I (426) main_task: Returned from app_main()

I (616) NimBLE: GAP procedure initiated: connect; 
I (616) NimBLE: peer_addr_type=0 peer_addr=
I (616) NimBLE: 70:04:1d:13:66:fe
I (616) NimBLE:  scan_itvl=16 scan_window=16 itvl_min=24 itvl_max=40 latency=0 supervision_timeout=256 min_ce_len=0 max_ce_len=0 own_addr_type=0
I (636) NimBLE: 

I (676) NimBLE: Connection established 
I (676) NimBLE: 

I (676) NimBLE: GATT procedure initiated: discover all services

I (856) NimBLE: GATT procedure initiated: discover all characteristics; 
I (856) NimBLE: start_handle=1 end_handle=5

I (1056) NimBLE: GATT procedure initiated: discover all characteristics; 
I (1056) NimBLE: start_handle=6 end_handle=9

I (1256) NimBLE: GATT procedure initiated: discover all characteristics; 
I (1256) NimBLE: start_handle=10 end_handle=22

I (1556) NimBLE: GATT procedure initiated: discover all characteristics; 
I (1556) NimBLE: start_handle=23 end_handle=65535

I (1756) NimBLE: GATT procedure initiated: discover all descriptors; 
I (1756) NimBLE: chr_val_handle=8 end_handle=9

I (1856) NimBLE: GATT procedure initiated: discover all descriptors; 
I (1856) NimBLE: chr_val_handle=12 end_handle=13

I (1956) NimBLE: GATT procedure initiated: discover all descriptors; 
I (1956) NimBLE: chr_val_handle=17 end_handle=18

I (2056) NimBLE: GATT procedure initiated: discover all descriptors; 
I (2056) NimBLE: chr_val_handle=20 end_handle=22

I (2156) NimBLE: GATT procedure initiated: discover all descriptors; 
I (2156) NimBLE: chr_val_handle=29 end_handle=65535

I (2256) NimBLE: Service discovery complete; status=0 conn_handle=1

I (2256) NimBLE: GATT procedure initiated: read; 
I (2256) NimBLE: att_handle=15

I (283056) NimBLE: Read temperature type char completed; status=0 conn_handle=1
I (283056) NimBLE:  attr_handle=167 value=
I (283056) NimBLE: 0x02
I (283066) NimBLE: 

I (283066) NimBLE: GATT procedure initiated: write; 
I (283066) NimBLE: att_handle=172 len=2

I (283156) NimBLE: Write to measurement interval char completed; status=0 conn_handle=1 attr_handle=172

I (283156) NimBLE: GATT procedure initiated: write; 
I (283166) NimBLE: att_handle=163 len=2

I (283256) NimBLE: Subscribe to temperature measurement char completed; status=0 conn_handle=1 attr_handle=163

I (283256) NimBLE: GATT procedure initiated: write; 
I (283266) NimBLE: att_handle=170 len=2

I (283356) NimBLE: Subscribe to intermediate temperature char completed; status=0 conn_handle=1 attr_handle=170


```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
