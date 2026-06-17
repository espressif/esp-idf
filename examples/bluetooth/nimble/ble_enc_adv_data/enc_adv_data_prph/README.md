| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S3 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

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

* Select encrypted adv data from `Component config -> Bluetooth -> NimBLE Options -> Extra Features -> Encrypted Advertising Data`

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
I (...) NimBLE: BLE Host Task Started
I (...) main_task: Returned from app_main()
I (...) NimBLE: Device Address:
I (...) NimBLE: xx:xx:xx:xx:xx:xx
I (...) NimBLE:

I (...) NimBLE: Data before encryption:
I (...) NimBLE: <plain_adv_bytes...>
I (...) NimBLE: Encryption of adv data done successfully
I (...) NimBLE: Data after encryption:
I (...) NimBLE: <encrypted_adv_bytes...>

I (...) NimBLE: GAP procedure initiated: advertise;
I (...) NimBLE: disc_mode=2
I (...) NimBLE:  adv_channel_map=0 own_addr_type=0 adv_filter_policy=0 adv_itvl_min=48 adv_itvl_max=48
I (...) NimBLE:

I (...) NimBLE: connection established; status=0
I (...) NimBLE: handle=0 ... conn_itvl=40 conn_latency=0 supervision_timeout=256 encrypted=0 authenticated=0 bonded=0
I (...) NimBLE:

I (...) NimBLE: mtu update event; conn_handle=0 cid=4 mtu=256
I (...) ENC_ADV_DATA_PRPH: PASSKEY_ACTION_EVENT started
I (...) ENC_ADV_DATA_PRPH: Enter passkey 123456 on the peer side
I (...) ENC_ADV_DATA_PRPH: ble_sm_inject_io result: 0
I (...) NimBLE: encryption change event; status=0
I (...) NimBLE: handle=0 ... encrypted=1 authenticated=1 bonded=0

I (...) NimBLE: authorization event; conn_handle=0 attr_handle=7 is_read=1
I (...) NimBLE: disconnect; reason=...

I (...) NimBLE: Data before encryption:
I (...) NimBLE: <plain_adv_bytes...>
I (...) NimBLE: Encryption of adv data done successfully
I (...) NimBLE: Data after encryption:
I (...) NimBLE: <encrypted_adv_bytes...>

I (...) NimBLE: GAP procedure initiated: advertise;
I (...) NimBLE: connection established; status=0
I (...) NimBLE: mtu update event; conn_handle=0 cid=4 mtu=256
I (...) ENC_ADV_DATA_PRPH: PASSKEY_ACTION_EVENT started
I (...) ENC_ADV_DATA_PRPH: ble_sm_inject_io result: 0
I (...) NimBLE: encryption change event; status=0
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
