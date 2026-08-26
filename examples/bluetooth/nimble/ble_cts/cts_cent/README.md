| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

# BLE CTS Cent Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example creates GATT client and performs passive scan, it then connects to peripheral device if the device advertises connectability and the device advertises support for the Current Time Service (0x1805) as primary service UUID.

It performs following GATT operations against the specified peer:

* Reads the Current Time characteristic.

If the peer does not support a required service, characteristic, or descriptor, then the peer lied when it claimed support for the Current Time Service! When this happens, or if a GATT procedure fails, this function immediately terminates the connection.

It uses ESP32's Bluetooth controller and NimBLE stack based BLE host.

This example aims at understanding BLE service discovery, connection, encryption and characteristic operations.

To test this demo, use any BLE GATT server app that advertises support for the Current Time Service (0x1805) and includes it in the GATT database.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* A development board with ESP32/ESP32-C2/ESP32-C3/ESP32-S3/ESP32-H2/ESP32-C6 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
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
I (822) NimBLE_CTS_CENT: BLE Host Task Started
I (822) main_task: Returned from app_main()
I (872) NimBLE: GAP procedure initiated: discovery;
I (872) NimBLE: own_addr_type=0 filter_policy=0 passive=1 limited=0 filter_duplicates=1
I (872) NimBLE: duration=forever
I (872) NimBLE:

I (26022) NimBLE: GAP procedure initiated: connect;
I (26022) NimBLE: peer_addr_type=0 peer_addr=
I (26022) NimBLE: b0:b2:1c:b4:74:0e
I (26022) NimBLE:  scan_itvl=16 scan_window=16 itvl_min=24 itvl_max=40 latency=0 supervision_timeout=256 min_ce_len=0 max_ce_len=0 own_addr_type=0
I (26032) NimBLE:

I (26282) NimBLE: Connection established
I (26282) NimBLE:

I (26292) NimBLE: Connection secured

I (27832) NimBLE: encryption change event; status=0
I (27832) NimBLE: GATT procedure initiated: discover all services

I (27932) NimBLE: GATT procedure initiated: discover all characteristics;
I (27932) NimBLE: start_handle=1 end_handle=5

I (28032) NimBLE: GATT procedure initiated: discover all characteristics;
I (28032) NimBLE: start_handle=6 end_handle=13

I (28132) NimBLE: GATT procedure initiated: discover all characteristics;
I (28132) NimBLE: start_handle=14 end_handle=65535

I (28332) NimBLE: GATT procedure initiated: discover all descriptors;
I (28332) NimBLE: chr_val_handle=8 end_handle=9

I (28432) NimBLE: GATT procedure initiated: discover all descriptors;
I (28432) NimBLE: chr_val_handle=16 end_handle=17

I (28532) NimBLE: GATT procedure initiated: discover all descriptors;
I (28532) NimBLE: chr_val_handle=21 end_handle=65535

I (28632) NimBLE: Service discovery complete; status=0 conn_handle=0

I (28632) NimBLE: GATT procedure initiated: read;
I (28632) NimBLE: att_handle=16

I (28732) NimBLE: Read Current time complete; status=0 conn_handle=0

I (28732) NimBLE:  attr_handle=16 value=

I (28732) NimBLE: 0xb2
I (28732) NimBLE: :0x07
I (28732) NimBLE: :0x01
I (28732) NimBLE: :0x01
I (28732) NimBLE: :0x00
I (28742) NimBLE: :0x00
I (28742) NimBLE: :0x03
I (28742) NimBLE: :0x05
I (28742) NimBLE: :0x4b
I (28752) NimBLE: :0x00
I (28752) NimBLE:

I (28752) NimBLE_CTS_CENT: Date : 1/1/1970 Friday
I (28752) NimBLE_CTS_CENT: hours : 0 minutes : 0
I (28762) NimBLE_CTS_CENT: seconds : 3

I (28762) NimBLE_CTS_CENT: fractions : 75
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
