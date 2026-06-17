| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

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
I (852) NimBLE_HTP_CENT: BLE Host Task Started
I (862) main_task: Returned from app_main()
I (902) NimBLE: GAP procedure initiated: discovery;
I (902) NimBLE: own_addr_type=0 filter_policy=0 passive=1 limited=0 filter_duplicates=1
I (902) NimBLE: duration=forever
I (912) NimBLE:

I (972) NimBLE: GAP procedure initiated: connect;
I (972) NimBLE: peer_addr_type=0 peer_addr=
I (972) NimBLE: b0:b2:1c:b4:6b:4e
I (972) NimBLE:  scan_itvl=16 scan_window=16 itvl_min=24 itvl_max=40 latency=0 supervision_timeout=256 min_ce_len=0 max_ce_len=0 own_addr_type=0
I (982) NimBLE:

I (1162) NimBLE: Connection established
I (1162) NimBLE:

I (1172) NimBLE: Connection secured

I (2762) NimBLE: encryption change event; status=0
I (2762) NimBLE: GATT procedure initiated: discover all services

I (2962) NimBLE: GATT procedure initiated: discover all characteristics;
I (2962) NimBLE: start_handle=1 end_handle=5

I (3062) NimBLE: GATT procedure initiated: discover all characteristics;
I (3062) NimBLE: start_handle=6 end_handle=13

I (3162) NimBLE: GATT procedure initiated: discover all characteristics;
I (3162) NimBLE: start_handle=14 end_handle=26

I (3362) NimBLE: GATT procedure initiated: discover all characteristics;
I (3362) NimBLE: start_handle=27 end_handle=65535

I (3562) NimBLE: GATT procedure initiated: discover all descriptors;
I (3562) NimBLE: chr_val_handle=8 end_handle=9

I (3662) NimBLE: GATT procedure initiated: discover all descriptors;
I (3662) NimBLE: chr_val_handle=16 end_handle=17

I (3762) NimBLE: GATT procedure initiated: discover all descriptors;
I (3762) NimBLE: chr_val_handle=21 end_handle=22

I (3862) NimBLE: GATT procedure initiated: discover all descriptors;
I (3862) NimBLE: chr_val_handle=24 end_handle=26

I (3962) NimBLE: GATT procedure initiated: discover all descriptors;
I (3962) NimBLE: chr_val_handle=33 end_handle=65535

I (4062) NimBLE: Service discovery complete; status=0 conn_handle=0

I (4062) NimBLE: GATT procedure initiated: read;
I (4062) NimBLE: att_handle=19

I (4162) NimBLE: Read temperature type char completed; status=0 conn_handle=0
I (4162) NimBLE:  attr_handle=19 value=
I (4162) NimBLE: 0x02
I (4162) NimBLE: :0x00
I (4162) NimBLE:

I (4172) NimBLE: GATT procedure initiated: write;
I (4172) NimBLE: att_handle=24 len=2

I (4262) NimBLE: Write to measurement interval char completed; status=0 conn_handle=0 attr_handle=24

I (4262) NimBLE: GATT procedure initiated: write;
I (4262) NimBLE: att_handle=17 len=2

I (4362) NimBLE: Subscribe to temperature measurement char completed; status=261 conn_handle=0 attr_handle=17

I (4362) NimBLE: GATT procedure initiated: write;
I (4362) NimBLE: att_handle=22 len=2

I (4462) NimBLE: Subscribe to intermediate temperature char completed; status=261 conn_handle=0 attr_handle=22

I (5462) NimBLE: received notification; conn_handle=0 attr_handle=21 attr_len=5

I (6462) NimBLE: received notification; conn_handle=0 attr_handle=21 attr_len=5

I (7462) NimBLE: received notification; conn_handle=0 attr_handle=21 attr_len=5

I (8462) NimBLE: received notification; conn_handle=0 attr_handle=21 attr_len=5

I (9462) NimBLE: received notification; conn_handle=0 attr_handle=21 attr_len=5

I (10462) NimBLE: received notification; conn_handle=0 attr_handle=21 attr_len=5
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
