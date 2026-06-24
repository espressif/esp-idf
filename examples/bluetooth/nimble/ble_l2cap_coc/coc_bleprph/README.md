| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

# BLE Peripheral L2CAP COC Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example advertises data, connects to central and supports connection oriented channels.

It performs two operations against the specified peer:

* Connects L2CAP connection oriented channels.

* Receives data over formed L2CAP connection oriented channels.

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
I (859) NimBLE_BLE_PRPH_L2CAP_COC: BLE Host Task Started
I (869) uart: queue free spaces: 8
I (869) main_task: Returned from app_main()
I (909) NimBLE: Device Address:
I (909) NimBLE: b0:b2:1c:b4:74:0e
I (909) NimBLE:

I (909) NimBLE: GAP procedure initiated: advertise;
I (909) NimBLE: disc_mode=2
I (909) NimBLE:  adv_channel_map=0 own_addr_type=0 adv_filter_policy=0 adv_itvl_min=0 adv_itvl_max=0
I (919) NimBLE:

I (1149) NimBLE: connection established; status=0
I (1149) NimBLE: handle=0 our_ota_addr_type=0 our_ota_addr=
I (1149) NimBLE: b0:b2:1c:b4:74:0e
I (1149) NimBLE:  our_id_addr_type=0 our_id_addr=
I (1159) NimBLE: b0:b2:1c:b4:74:0e
I (1159) NimBLE:  peer_ota_addr_type=0 peer_ota_addr=
I (1159) NimBLE: b0:b2:1c:b4:6b:4e
I (1169) NimBLE:  peer_id_addr_type=0 peer_id_addr=
I (1169) NimBLE: b0:b2:1c:b4:6b:4e
I (1169) NimBLE:  conn_itvl=40 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0

I (1179) NimBLE:

I (1189) NimBLE: handle=0 our_ota_addr_type=0 our_ota_addr=
I (1189) NimBLE: b0:b2:1c:b4:74:0e
I (1189) NimBLE:  our_id_addr_type=0 our_id_addr=
I (1199) NimBLE: b0:b2:1c:b4:74:0e
I (1199) NimBLE:  peer_ota_addr_type=0 peer_ota_addr=
I (1209) NimBLE: b0:b2:1c:b4:6b:4e
I (1209) NimBLE:  peer_id_addr_type=0 peer_id_addr=
I (1209) NimBLE: b0:b2:1c:b4:6b:4e
I (1219) NimBLE:  conn_itvl=40 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0

LE CoC accepting, chan: 0x3ffcffd0, peer_mtu 512
LE COC connected, conn: 0, chan: 0x3ffcffd0, psm: 0x1002, scid: 0x0040, dcid: 0x0040, our_mps: 504, our_mtu: 512, peer_mps: 504, peer_mtu: 512
LE CoC accepting, chan: 0x3ffcffd0, peer_mtu 512
LE CoC accepting, chan: 0x3ffcffd0, peer_mtu 512
LE CoC accepting, chan: 0x3ffcffd0, peer_mtu 512
LE CoC accepting, chan: 0x3ffcffd0, peer_mtu 512
LE CoC accepting, chan: 0x3ffcffd0, peer_mtu 512

Done
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
