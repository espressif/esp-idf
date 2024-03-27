| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# BLE Central Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example creates GATT client and performs passive scan, it then connects to peripheral device if the device advertises connectability and the device advertises support for the Alert Notification service (0x1811) as primary service UUID.

After connection it enables bonding and link encryprion if the `Enable Link Encryption` flag is set in the example config.

It performs six GATT operations against the specified peer:

* Reads the ANS Supported New Alert Category characteristic.

* After the read operation is completed, writes the ANS Alert Notification Control Point characteristic.

* After the write operation is completed, subscribes to notifications for the ANS Unread Alert Status characteristic.

* After the subscribe operation is completed, it subscribes to notifications for a user defined characteristic.

* After this subscribe operation is completed, it writes to the user defined characteristic.

* After the write operation is completed, it reads from the user defined characteristic.

If the peer does not support a required service, characteristic, or descriptor, then the peer lied when it claimed support for the alert notification service! When this happens, or if a GATT procedure fails, this function immediately terminates the connection.

It uses ESP32's Bluetooth controller and NimBLE stack based BLE host.

This example aims at understanding BLE service discovery, connection, encryption and characteristic operations.

To test this demo, use any BLE GATT server app that advertises support for the Alert Notification service (0x1811) and includes it in the GATT database.

Note :

* To install the dependency packages needed, please refer to the top level [README file](../../../README.md#running-test-python-script-pytest).
* Currently this Python utility is only supported on Linux (BLE communication is via BLuez + DBus).

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
I (202) BTDM_INIT: BT controller compile version [0b60040]
I (202) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
W (212) phy_init: failed to load RF calibration data (0xffffffff), falling back to full calibration
I (422) phy: phy_version: 4007, 9c6b43b, Jan 11 2019, 16:45:07, 0, 2
I (722) NimBLE_BLE_CENT: BLE Host Task Started
GAP procedure initiated: stop advertising.
GAP procedure initiated: discovery; own_addr_type=0 filter_policy=0 passive=1 limited=0 filter_duplicates=1 duration=forever
GAP procedure initiated: connect; peer_addr_type=1 peer_addr=xx:xx:xx:xx:xx:xx scan_itvl=16 scan_window=16 itvl_min=24 itvl_max=40 latency=0 supervision_timeout=256 min_ce_len=16 max_ce_len=768 own_addr_type=0
Connection established
Connection secured
encryption change event; status=0
GATT procedure initiated: discover all services
GATT procedure initiated: discover all characteristics; start_handle=1 end_handle=3
GATT procedure initiated: discover all characteristics; start_handle=20 end_handle=26
GATT procedure initiated: discover all characteristics; start_handle=40 end_handle=65535
GATT procedure initiated: discover all descriptors; chr_val_handle=42 end_handle=43
GATT procedure initiated: discover all descriptors; chr_val_handle=49 end_handle=65535
Service discovery complete; status=0 conn_handle=0
GATT procedure initiated: read; att_handle=45
GATT procedure initiated: write; att_handle=47 len=2
GATT procedure initiated: write; att_handle=43 len=2
Read complete; status=0 conn_handle=0 attr_handle=45 value=0x02
Write complete; status=0 conn_handle=0 attr_handle=47
Subscribe complete; status=0 conn_handle=0 attr_handle=43
GATT procedure initiated: write; att_handle=26 len=2
GATT procedure initiated: write; att_handle=25 len=1
GATT procedure initiated: read; att_handle=25
Subscribe to the custom subscribable characteristic complete; status=0 conn_handle=1 attr_handle=26 value=
Write to the custom subscribable characteristic complete; status=0 conn_handle=1 attr_handle=25
received notification; conn_handle=1 attr_handle=25 attr_len=4
Read complete for the subscribable characteristic; status=0 conn_handle=1 attr_handle=25 value=0x19
```

This is the console output on failure (or peripheral does not support New Alert Service category):

```
I (180) BTDM_INIT: BT controller compile version [8e87ec7]
I (180) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (250) phy: phy_version: 4000, b6198fa, Sep  3 2018, 15:11:06, 0, 0
I (480) NimBLE_BLE_CENT: BLE Host Task Started
GAP procedure initiated: stop advertising.
GAP procedure initiated: discovery; own_addr_type=0 filter_policy=0 passive=1 limited=0 filter_duplicates=1 duration=forever
GAP procedure initiated: connect; peer_addr_type=1 peer_addr=xx:xx:xx:xx:xx:xx scan_itvl=16 scan_window=16 itvl_min=24 itvl_max=40 latency=0 supervision_timeout=256 min_ce_len=16 max_ce_len=768 own_addr_type=0
Connection established
GATT procedure initiated: discover all services
GATT procedure initiated: discover all characteristics; start_handle=1 end_handle=3
GATT procedure initiated: discover all characteristics; start_handle=20 end_handle=26
GATT procedure initiated: discover all characteristics; start_handle=40 end_handle=65535
GATT procedure initiated: discover all descriptors; chr_val_handle=42 end_handle=43
GATT procedure initiated: discover all descriptors; chr_val_handle=47 end_handle=65535
Service discovery complete; status=0 conn_handle=0
Error: Peer doesn't support the Supported New Alert Category characteristic
GAP procedure initiated: terminate connection; conn_handle=0 hci_reason=19
disconnect; reason=534
```


## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
