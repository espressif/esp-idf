| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | -------- | -------- |

# BLE Encrypted Advertising Data Peripheral Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example creates GATT server and then starts normal advertising and encrypted advertising data, waiting to be connected to a GATT client.

This example aims at understanding advertising encrypted data.

For RPA feature (currently Host based privacy feature is supported), use API `ble_hs_pvcy_rpa_config` to enable/disable host based privacy, `own_addr_type` needs to be set to `BLE_ADDR_RANDOM` to use this feature. Please include `ble_hs_pvcy.h` while using this API. As `ble_hs_pvcy_rpa_config` configures host privacy and sets address in controller, it is necessary to call this API after host-controller are synced (e.g. in `bleprph_on_sync` callback).

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
In the `Component config` menu:

* Select encrypted adv data from `Component config -> Bluetooth -> NimBLE Options -> BT_NIMBLE_ENC_ADV_DATA`

In the `Example Configuration` menu:

* Select I/O capabilities of device from `Example Configuration --> I/O Capability`, default is `Just_works`.
* Enable/Disable other security related parameters `Bonding, MITM option, secure connection(SM SC)`.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

There is this console output when bleprph is connected and characteristic is read:

```
I (445) NimBLE: BLE Host Task Started
I (445) NimBLE: Device Address:
I (445) NimBLE: 60:55:f9:f7:4f:42
I (445) NimBLE:

I (455) NimBLE: Encryption of adv data done successfully
I (465) main_task: Returned from app_main()
I (575) NimBLE: connection established; status=0
I (575) NimBLE: handle=1 our_ota_addr_type=0 our_ota_addr=
I (575) NimBLE: 60:55:f9:f7:4f:42
I (585) NimBLE:  our_id_addr_type=0 our_id_addr=
I (585) NimBLE: 60:55:f9:f7:4f:42
I (595) NimBLE:  peer_ota_addr_type=0 peer_ota_addr=
I (595) NimBLE: 60:55:f9:f7:51:0a
I (605) NimBLE:  peer_id_addr_type=0 peer_id_addr=
I (605) NimBLE: 60:55:f9:f7:51:0a
I (615) NimBLE:  conn_itvl=24 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0

I (625) NimBLE:

I (625) NimBLE: advertise complete; reason=0
I (635) NimBLE: Encryption of adv data done successfully

I (1635) NimBLE: disconnect; reason=531
I (1635) NimBLE: handle=1 our_ota_addr_type=0 our_ota_addr=
I (1635) NimBLE: 60:55:f9:f7:4f:42
I (1635) NimBLE:  our_id_addr_type=0 our_id_addr=
I (1635) NimBLE: 60:55:f9:f7:4f:42
I (1645) NimBLE:  peer_ota_addr_type=0 peer_ota_addr=
I (1645) NimBLE: 60:55:f9:f7:51:0a
I (1655) NimBLE:  peer_id_addr_type=0 peer_id_addr=
I (1655) NimBLE: 60:55:f9:f7:51:0a
I (1665) NimBLE:  conn_itvl=24 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0

I (1675) NimBLE:

I (1675) NimBLE: connection established; status=0
I (1685) NimBLE: handle=1 our_ota_addr_type=0 our_ota_addr=
I (1685) NimBLE: 60:55:f9:f7:4f:42
I (1695) NimBLE:  our_id_addr_type=0 our_id_addr=
I (1695) NimBLE: 60:55:f9:f7:4f:42
I (1705) NimBLE:  peer_ota_addr_type=0 peer_ota_addr=
I (1705) NimBLE: 60:55:f9:f7:51:0a
I (1715) NimBLE:  peer_id_addr_type=0 peer_id_addr=
I (1715) NimBLE: 60:55:f9:f7:51:0a
I (1725) NimBLE:  conn_itvl=24 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0

I (1735) NimBLE:

I (1735) NimBLE: advertise complete; reason=0
I (1745) NimBLE: Encryption of adv data done successfully
I (1755) NimBLE: mtu update event; conn_handle=1 cid=4 mtu=256

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
