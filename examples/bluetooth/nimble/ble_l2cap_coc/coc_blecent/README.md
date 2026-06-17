| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

# BLE Central L2CAP COC Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example performs passive scan and then connects to peripheral device if the device advertises connectability and supports connection oriented channels.

It performs two operations against the specified peer:

* Connects L2CAP connection oriented channels.

* Sends data 5 times over formed L2CAP connection oriented channels.

It uses ESP32's Bluetooth controller and NimBLE stack based BLE host.

This example aims at understanding BLE service discovery, connection and data transfer over L2CAP layer through connection oriented channels.

Maximum data of 512 bytes can be transferred over L2CAP when MTU is set to 512 and MPS is 528.

Note :

* To install the dependency packages needed, please refer to the top level [README file](../../../../README.md#running-test-python-script-pytest).

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* A development board with ESP32/ESP32-C3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Configure the Project

Open the project configuration menu:

```bash
idf.py menuconfig
```

In the `Component Config` menu:

* Change Component config → Bluetooth → NimBLE Options → L2CAP → Maximum number of connection oriented channels to a value greater than 0
* Change Component config → Bluetooth → NimBLE Options → Memory Settings → MSYS_1 Block Size to 536 (For maximum transmission of data)

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

This is the console output on successful connection and 512 bytes of data sent over L2CAP connection oriented channel:

```
I (743) NimBLE_BLE_CENT_L2CAP_COC: BLE Host Task Started
I (743) main_task: Returned from app_main()
I (793) NimBLE: GAP procedure initiated: discovery;
I (793) NimBLE: own_addr_type=0 filter_policy=0 passive=1 limited=0 filter_duplicates=1
I (793) NimBLE: duration=forever
I (793) NimBLE:

I (4943) NimBLE: GAP procedure initiated: connect;
I (4943) NimBLE: peer_addr_type=0 peer_addr=
I (4943) NimBLE: b0:b2:1c:b4:74:0e
I (4943) NimBLE:  scan_itvl=16 scan_window=16 itvl_min=24 itvl_max=40 latency=0 supervision_timeout=256 min_ce_len=0 max_ce_len=0 own_addr_type=0
I (4953) NimBLE:

I (5203) NimBLE: Connection established
I (5203) NimBLE:

I (5203) NimBLE: GATT procedure initiated: discover all services

LE COC connected, conn: 0, chan: 0x3ffce3f0, psm: 0x1002, scid: 0x0040, dcid: 0x0040, our_mps: 504, our_mtu: 512, peer_mps: 504, peer_mtu: 512
I (5743) NimBLE: Data:
 0 1 2 3 4 5 6 7 8 9 10 ... 255 0 1 2 3 4 5 6 7 8 9 10 ... 255
I (5893) NimBLE: Data sent successfully

I (6393) NimBLE: Data:
 0 1 2 3 4 5 6 7 8 9 10 ... 255 0 1 2 3 4 5 6 7 8 9 10 ... 255
I (6543) NimBLE: Data sent successfully

I (7043) NimBLE: Data:
 0 1 2 3 4 5 6 7 8 9 10 ... 255 0 1 2 3 4 5 6 7 8 9 10 ... 255
I (7193) NimBLE: Data sent successfully

I (7693) NimBLE: Data:
 0 1 2 3 4 5 6 7 8 9 10 ... 255 0 1 2 3 4 5 6 7 8 9 10 ... 255
I (7843) NimBLE: Data sent successfully

I (8343) NimBLE: Data:
 0 1 2 3 4 5 6 7 8 9 10 ... 255 0 1 2 3 4 5 6 7 8 9 10 ... 255
I (8493) NimBLE: Data sent successfully
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
