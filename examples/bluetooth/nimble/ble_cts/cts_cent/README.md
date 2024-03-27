| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

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
I (358) BLE_INIT: BT controller compile version [59725b5]
I (358) BLE_INIT: Bluetooth MAC: 60:55:f9:68:c4:fa
I (368) phy_init: phy_version 1110,9c20f0a,Jul 27 2023,10:42:54
I (408) NimBLE_CTS_CENT: BLE Host Task Started
I (408) NimBLE: GAP procedure initiated: stop advertising.

I (408) NimBLE: GAP procedure initiated: discovery;
I (408) NimBLE: own_addr_type=0 filter_policy=0 passive=1 limited=0 filter_duplicates=1
I (418) NimBLE: duration=forever
I (428) NimBLE:

I (428) main_task: Returned from app_main()
I (628) NimBLE: GAP procedure initiated: connect;
I (628) NimBLE: peer_addr_type=1 peer_addr=
I (628) NimBLE: 6b:93:b5:30:71:cf
I (638) NimBLE:  scan_itvl=16 scan_window=16 itvl_min=24 itvl_max=40 latency=0 supervision_timeout=256 min_ce_len=0 max_ce_len=0 own_addr_type=0
I (648) NimBLE:

I (908) NimBLE: Connection established
I (908) NimBLE:

I (918) NimBLE: Connection secured

I (1208) NimBLE: received indication; conn_handle=1 attr_handle=3 attr_len=4

I (1208) NimBLE: GAP procedure initiated:
I (1208) NimBLE: connection parameter update; conn_handle=1 itvl_min=6 itvl_max=6 latency=0 supervision_timeout=500 min_ce_len=0 max_ce_len=0
I (1228) NimBLE:

I (3568) NimBLE: encryption change event; status=0
I (3568) NimBLE: GATT procedure initiated: discover all services

I (3608) NimBLE: GATT procedure initiated: discover all characteristics;
I (3608) NimBLE: start_handle=1 end_handle=9

I (3658) NimBLE: GATT procedure initiated: discover all characteristics;
I (3658) NimBLE: start_handle=20 end_handle=26

I (3688) NimBLE: GATT procedure initiated: discover all characteristics;
I (3688) NimBLE: start_handle=134 end_handle=141

I (3718) NimBLE: GATT procedure initiated: discover all characteristics;
I (3718) NimBLE: start_handle=142 end_handle=151

I (3758) NimBLE: GATT procedure initiated: discover all characteristics;
I (3758) NimBLE: start_handle=152 end_handle=158

I (3788) NimBLE: GATT procedure initiated: discover all characteristics;
I (3788) NimBLE: start_handle=159 end_handle=65535

I (3818) NimBLE: GATT procedure initiated: discover all descriptors;
I (3818) NimBLE: chr_val_handle=136 end_handle=137

I (3838) NimBLE: GATT procedure initiated: discover all descriptors;
I (3838) NimBLE: chr_val_handle=144 end_handle=148

I (3898) NimBLE: GATT procedure initiated: discover all descriptors;
I (3898) NimBLE: chr_val_handle=150 end_handle=151

I (3908) NimBLE: GATT procedure initiated: discover all descriptors;
I (3908) NimBLE: chr_val_handle=161 end_handle=162

I (3928) NimBLE: GATT procedure initiated: discover all descriptors;
I (3928) NimBLE: chr_val_handle=164 end_handle=65535

I (3938) NimBLE: Service discovery complete; status=0 conn_handle=1

I (3938) NimBLE: GATT procedure initiated: read;
I (3938) NimBLE: att_handle=161

I (3958) NimBLE: Read Current time complete; status=0 conn_handle=1
I (3958) NimBLE:  attr_handle=161 value=
I (3958) NimBLE: 0xe7
I (3958) NimBLE: :0x07
I (3958) NimBLE: :0x08
I (3968) NimBLE: :0x1e
I (3968) NimBLE: :0x14
I (3968) NimBLE: :0x25
I (3978) NimBLE: :0x02
I (3978) NimBLE: :0x03
I (3978) NimBLE: :0xf6
I (3978) NimBLE: :0x00
I (3988) NimBLE:

I (3988) NimBLE_CTS_CENT: Date : 30/8/2023
I (3998) NimBLE_CTS_CENT: hours : 20 minutes : 37
I (3998) NimBLE_CTS_CENT: seconds : 2

I (4008) NimBLE_CTS_CENT: fractions : 0

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
