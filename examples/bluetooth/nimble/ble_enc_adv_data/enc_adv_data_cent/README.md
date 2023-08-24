| Supported Targets | ESP32-C2 | ESP32-C6 | ESP32-H2 |
| ----------------- | -------- | -------- | -------- |

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

* Select encrypted adv data from `Component config -> Bluetooth -> NimBLE Options -> BT_NIMBLE_ENC_ADV_DATA`

In the `Example Configuration` menu:

* Change the `Peer Address` option if needed.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

This is the console output on successful connection:

```
I (436) NimBLE: BLE Host Task Started
I (446) main_task: Returned from app_main()
I (506) NimBLE: Adding peer addr : 60:55:f9:f7:4f:42
I (546) NimBLE: Connection established
I (546) NimBLE:
I (546) NimBLE: GATT procedure initiated: exchange mtu

I (546) NimBLE: GATT procedure initiated: discover all services

I (636) NimBLE: mtu update event; conn_handle=1 cid=4 mtu=256

I (696) NimBLE: GATT procedure initiated: discover all characteristics;
I (696) NimBLE: start_handle=1 end_handle=7

I (846) NimBLE: GATT procedure initiated: discover all characteristics;
I (846) NimBLE: start_handle=8 end_handle=11

I (966) NimBLE: GATT procedure initiated: discover all characteristics;
I (966) NimBLE: start_handle=12 end_handle=65535

I (1116) NimBLE: GATT procedure initiated: discover all descriptors;
I (1116) NimBLE: chr_val_handle=10 end_handle=11

I (1176) NimBLE: GATT procedure initiated: discover all descriptors;
I (1176) NimBLE: chr_val_handle=14 end_handle=65535

I (1386) NimBLE: Service discovery complete; status=0 conn_handle=1

I (1386) NimBLE: GATT procedure initiated: read;
I (1386) NimBLE: att_handle=7

I (1476) NimBLE: Read complete; status=0 conn_handle=1
I (1476) NimBLE:  attr_handle=7 value=
I (1476) NimBLE: 0xc0
I (1486) NimBLE: :0xc1
I (1486) NimBLE: :0xc2
I (1486) NimBLE: :0xc3
I (1486) NimBLE: :0xc4
I (1496) NimBLE: :0xc5
I (1496) NimBLE: :0xc6
I (1496) NimBLE: :0xc7
I (1506) NimBLE: :0xc8
I (1506) NimBLE: :0xc9
I (1506) NimBLE: :0xca
I (1516) NimBLE: :0xcb
I (1516) NimBLE: :0xcc
I (1516) NimBLE: :0xcd
I (1526) NimBLE: :0xce
I (1526) NimBLE: :0xcf
I (1526) NimBLE: :0xfb
I (1526) NimBLE: :0x56
I (1536) NimBLE: :0xe1
I (1536) NimBLE: :0xda
I (1536) NimBLE: :0xdc
I (1546) NimBLE: :0x7e
I (1546) NimBLE: :0xad
I (1546) NimBLE: :0xf5
I (1556) NimBLE: Writing of session key, iv, and peer addr to NVS success
I (1556) NimBLE: GAP procedure initiated: terminate connection; conn_handle=1 hci_reason=19

I (1596) NimBLE: disconnect; reason=534
I (1596) NimBLE:
I (1626) NimBLE: Peer was already added with addr : 60:55:f9:f7:4f:42
I (1626) NimBLE: Read session key and iv from NVS successfully
I (1636) NimBLE: Decryption of adv data done successfully
I (1726) NimBLE: Connection established
I (1726) NimBLE:
I (1726) NimBLE: GATT procedure initiated: exchange mtu

I (1736) NimBLE: GATT procedure initiated: discover all services

I (1826) NimBLE: mtu update event; conn_handle=1 cid=4 mtu=256

I (1886) NimBLE: GATT procedure initiated: discover all characteristics;
I (1886) NimBLE: start_handle=1 end_handle=7

I (2006) NimBLE: GATT procedure initiated: discover all characteristics;
I (2006) NimBLE: start_handle=8 end_handle=11

I (2156) NimBLE: GATT procedure initiated: discover all characteristics;
I (2156) NimBLE: start_handle=12 end_handle=65535

I (2276) NimBLE: GATT procedure initiated: discover all descriptors;
I (2276) NimBLE: chr_val_handle=10 end_handle=11

I (2366) NimBLE: GATT procedure initiated: discover all descriptors;
I (2366) NimBLE: chr_val_handle=14 end_handle=65535

I (2546) NimBLE: Service discovery complete; status=0 conn_handle=1


Done

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
