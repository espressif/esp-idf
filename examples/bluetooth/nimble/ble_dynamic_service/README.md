| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# BLE Dynamic Service Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example creates GATT server and then starts advertising, waiting to be connected to a GATT client.
In the main thread it keeps on adding and deleting one custom service in the gatt server.

It uses ESP32's Bluetooth controller and NimBLE stack based BLE host.

This example aims at understanding addition and deletion of services at runtime.
Note : Services can be added at the time of init. This example focuses on adding services after stack init. There may be active connections at the time the new service is added/deleted.

To test this demo, any BLE scanner app can be used.

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
### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

There is this console output when ble_dynamic_service is connected to ble scanner:

```
I (354) BLE_INIT: BT controller compile version [80abacd]
I (354) phy_init: phy_version 950,f732b06,Feb 15 2023,18:57:12
I (404) BLE_INIT: Bluetooth MAC: 58:cf:79:e9:c1:9e

I (404) NimBLE_BLE_DYNAMIC_SERVER: BLE Host Task Started
I (404) NimBLE: GAP procedure initiated: stop advertising.

I (414) NimBLE: Failed to restore IRKs from store; status=8

I (414) NimBLE: Device Address:
I (424) NimBLE: 58:cf:79:e9:c1:9e
I (424) NimBLE:

I (424) NimBLE: GAP procedure initiated: advertise;
I (434) NimBLE: disc_mode=2
I (434) NimBLE:  adv_channel_map=0 own_addr_type=0 adv_filter_policy=0 adv_itvl_min=0 adv_itvl_max=0
I (444) NimBLE:

I (15444) NimBLE: Adding Dynamic service
I (30444) NimBLE: Deleting service
I (45444) NimBLE: Adding Dynamic service
I (60444) NimBLE: Deleting service
I (75444) NimBLE: Adding Dynamic service
I (90444) NimBLE: Deleting service
I (101654) NimBLE: connection established; status=0
I (101654) NimBLE: handle=1 our_ota_addr_type=0 our_ota_addr=
I (101654) NimBLE: 58:cf:79:e9:c1:9e
I (101664) NimBLE:  our_id_addr_type=0 our_id_addr=
I (101664) NimBLE: 58:cf:79:e9:c1:9e
I (101674) NimBLE:  peer_ota_addr_type=1 peer_ota_addr=
I (101674) NimBLE: 41:c7:05:e9:d9:ce
I (101684) NimBLE:  peer_id_addr_type=1 peer_id_addr=
I (101684) NimBLE: 41:c7:05:e9:d9:ce
I (101694) NimBLE:  conn_itvl=39 conn_latency=0 supervision_timeout=500 encrypted=0 authenticated=0 bonded=0

I (101704) NimBLE:

I (102284) NimBLE: connection updated; status=0
I (102284) NimBLE: handle=1 our_ota_addr_type=0 our_ota_addr=
I (102284) NimBLE: 58:cf:79:e9:c1:9e
I (102294) NimBLE:  our_id_addr_type=0 our_id_addr=
I (102294) NimBLE: 58:cf:79:e9:c1:9e
I (102304) NimBLE:  peer_ota_addr_type=1 peer_ota_addr=
I (102304) NimBLE: 41:c7:05:e9:d9:ce
I (102314) NimBLE:  peer_id_addr_type=1 peer_id_addr=
I (102314) NimBLE: 41:c7:05:e9:d9:ce
I (102324) NimBLE:  conn_itvl=6 conn_latency=0 supervision_timeout=500 encrypted=0 authenticated=0 bonded=0

I (102334) NimBLE:

I (102584) NimBLE: connection updated; status=0
I (102584) NimBLE: handle=1 our_ota_addr_type=0 our_ota_addr=
I (102584) NimBLE: 58:cf:79:e9:c1:9e
I (102584) NimBLE:  our_id_addr_type=0 our_id_addr=
I (102594) NimBLE: 58:cf:79:e9:c1:9e
I (102594) NimBLE:  peer_ota_addr_type=1 peer_ota_addr=
I (102604) NimBLE: 41:c7:05:e9:d9:ce
I (102604) NimBLE:  peer_id_addr_type=1 peer_id_addr=
I (102614) NimBLE: 41:c7:05:e9:d9:ce
I (102614) NimBLE:  conn_itvl=39 conn_latency=0 supervision_timeout=500 encrypted=0 authenticated=0 bonded=0

I (102624) NimBLE:

I (104974) NimBLE: subscribe event; conn_handle=1 attr_handle=8 reason=1 prevn=0 curn=0 previ=0 curi=1

I (105444) NimBLE: Adding Dynamic service
I (105444) NimBLE: GATT procedure initiated: indicate;
I (105444) NimBLE: att_handle=8

I (105444) NimBLE: notify_tx event; conn_handle=1 attr_handle=8 status=0 is_indication=1
I (105554) NimBLE: notify_tx event; conn_handle=1 attr_handle=8 status=14 is_indication=1
I (105554) NimBLE: GATT procedure initiated: indicate;
I (105564) NimBLE: att_handle=8

I (105564) NimBLE: notify_tx event; conn_handle=1 attr_handle=8 status=0 is_indication=1
I (105654) NimBLE: notify_tx event; conn_handle=1 attr_handle=8 status=14 is_indication=1
I (105904) NimBLE: connection updated; status=0
I (105904) NimBLE: handle=1 our_ota_addr_type=0 our_ota_addr=
I (105904) NimBLE: 58:cf:79:e9:c1:9e
I (105904) NimBLE:  our_id_addr_type=0 our_id_addr=
I (105914) NimBLE: 58:cf:79:e9:c1:9e
I (105914) NimBLE:  peer_ota_addr_type=1 peer_ota_addr=
I (105924) NimBLE: 41:c7:05:e9:d9:ce
I (105924) NimBLE:  peer_id_addr_type=1 peer_id_addr=
I (105934) NimBLE: 41:c7:05:e9:d9:ce
I (105934) NimBLE:  conn_itvl=6 conn_latency=0 supervision_timeout=500 encrypted=0 authenticated=0 bonded=0

I (105944) NimBLE:

I (106334) NimBLE: connection updated; status=0
I (106334) NimBLE: handle=1 our_ota_addr_type=0 our_ota_addr=
I (106334) NimBLE: 58:cf:79:e9:c1:9e
I (106344) NimBLE:  our_id_addr_type=0 our_id_addr=
I (106344) NimBLE: 58:cf:79:e9:c1:9e
I (106354) NimBLE:  peer_ota_addr_type=1 peer_ota_addr=
I (106354) NimBLE: 41:c7:05:e9:d9:ce
I (106364) NimBLE:  peer_id_addr_type=1 peer_id_addr=
I (106364) NimBLE: 41:c7:05:e9:d9:ce
I (106374) NimBLE:  conn_itvl=39 conn_latency=0 supervision_timeout=500 encrypted=0 authenticated=0 bonded=0

I (106384) NimBLE:

I (109604) NimBLE: subscribe event; conn_handle=1 attr_handle=40 reason=1 prevn=0 curn=1 previ=0 curi=0

I (110484) NimBLE: subscribe event; conn_handle=1 attr_handle=40 reason=1 prevn=1 curn=0 previ=0 curi=0

I (112484) NimBLE: Characteristic read; conn_handle=1 attr_handle=40

I (120454) NimBLE: Deleting service
I (120454) NimBLE: GATT procedure initiated: indicate;
I (120454) NimBLE: att_handle=8

I (120454) NimBLE: notify_tx event; conn_handle=1 attr_handle=8 status=0 is_indication=1
I (120574) NimBLE: notify_tx event; conn_handle=1 attr_handle=8 status=14 is_indication=1
I (120574) NimBLE: GATT procedure initiated: indicate;
I (120574) NimBLE: att_handle=8

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
