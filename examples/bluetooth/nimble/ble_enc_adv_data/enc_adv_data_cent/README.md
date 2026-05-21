| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S3 | ESP32-S31 |
| ----------------- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

# BLE Encrypted Advertising Data Central Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example creates GATT client and performs passive scan, it then connects to peripheral device if the device advertises connectability and the device advertises support for the custom service (0x2C01) as primary service UUID which has key material characteristic (0x2B88).

After connection it enables bonding and link encryprion if the `Enable Link Encryption` flag is set in the example config.

It performs the following operations against the specified peer:

* Scans the peripheral devices and saves the peer address if it supports gap service as a primary service.

* Check if session key or IV exists for the connected peer. If it's not present, read the key material characteric. The value read will contain session key and IV for decrypting advertising data.

* Disconnect from the peer once read is completed.

* While scanning for the second time, if the peer's address is saved and respestive session key and IV are present, decrypt the advertising data.

If the peer does not support a required service, characteristic, or descriptor, then the peer lied when it claimed support for the gap service! When this happens, or if a GATT procedure fails, this function immediately terminates the connection.

It uses ESP32's Bluetooth controller and NimBLE stack based BLE host.

This example aims at understanding BLE client scanning devices which advertise encrypted data.

To test this demo, use any BLE GATT server app that advertises support for the gap service (0x1800), includes it in the GATT database and has key material characteristic support.


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
In the `Component config` menu:

* Select encrypted adv data from `Component config -> Bluetooth -> NimBLE Options -> Extra Features -> Encrypted Advertising Data`

In the `Example Configuration` menu:

* Change the `Peer Address` option if needed.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

This is the console output on successful connection:

```
I (...) NimBLE: BLE Host Task Started
I (...) main_task: Returned from app_main()
I (...) NimBLE: GAP procedure initiated: discovery;
I (...) NimBLE: own_addr_type=0 filter_policy=0 passive=1 limited=0 filter_duplicates=1
I (...) NimBLE: duration=forever
I (...) NimBLE:

I (...) NimBLE: Adding peer addr : xx:xx:xx:xx:xx:xx
I (...) NimBLE: GAP procedure initiated: connect;
I (...) NimBLE: peer_addr_type=0 peer_addr=
I (...) NimBLE: xx:xx:xx:xx:xx:xx
I (...) NimBLE:  scan_itvl=16 scan_window=16 itvl_min=24 itvl_max=40 latency=0 supervision_timeout=256 min_ce_len=0 max_ce_len=0 own_addr_type=0
I (...) NimBLE:

I (...) NimBLE: Connection established
I (...) NimBLE:
I (...) NimBLE: GATT procedure initiated: exchange mtu
I (...) NimBLE: Connection secured
I (...) NimBLE: mtu update event; conn_handle=0 cid=4 mtu=256

I (...) ENC_ADV_DATA_CENT: PASSKEY_ACTION_EVENT started 2
I (...) ENC_ADV_DATA_CENT: Entering passkey 123456
I (...) ENC_ADV_DATA_CENT: ble_sm_inject_io result: 0
I (...) NimBLE: encryption change event; status=0

I (...) NimBLE: GATT procedure initiated: discover all services
I (...) NimBLE: GATT procedure initiated: discover all characteristics;
I (...) NimBLE: start_handle=...
I (...) NimBLE: GATT procedure initiated: discover all descriptors;
I (...) NimBLE: chr_val_handle=...
I (...) NimBLE: Service discovery complete; status=0 conn_handle=0

I (...) NimBLE: GATT procedure initiated: read;
I (...) NimBLE: att_handle=<key_material_attr>
I (...) NimBLE: Read complete; status=0 conn_handle=0
I (...) NimBLE:  attr_handle=<key_material_attr> value=
I (...) NimBLE: <session_key_and_iv_bytes...>
I (...) NimBLE: Writing of session key, iv, and peer addr to NVS success

I (...) NimBLE: GAP procedure initiated: terminate connection; conn_handle=0 hci_reason=19
I (...) NimBLE: disconnect; reason=...
I (...) NimBLE: GAP procedure initiated: discovery;

I (...) NimBLE: Peer was already added with addr : xx:xx:xx:xx:xx:xx
I (...) NimBLE: Read session key and iv from NVS successfully
I (...) NimBLE: Decryption of adv data done successfully
I (...) NimBLE: Data after decryption:
I (...) NimBLE: <decrypted_adv_bytes...>

I (...) NimBLE: GAP procedure initiated: connect;
I (...) NimBLE: Connection established
I (...) NimBLE: GATT procedure initiated: exchange mtu
I (...) NimBLE: Connection secured
I (...) NimBLE: encryption change event; status=0
I (...) NimBLE: GATT procedure initiated: discover all services
I (...) NimBLE: ...
I (...) NimBLE: Service discovery complete; status=0 conn_handle=0
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
