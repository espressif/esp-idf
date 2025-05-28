| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# BLE Heart Rate Measurement Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

This example creates GATT server demonstrating standard Heart Rate measurement service. It simulates Heart rate measurement and notifies to client when the notifications are enabled.

It uses ESP32's Bluetooth controller and NimBLE stack based BLE host

This example aims at understanding notification subscriptions and sending notifications.

To test this demo, any BLE scanner app can be used.

Note :

* To install the dependency packages needed, please refer to the top level [README file](../../../README.md#running-test-python-script-pytest).

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* A development board with ESP32/ESP32-C3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

This console output can be observed when blehr is connected to client and client enables notifications:

```
I (91) BTDM_INIT: BT controller compile version [fe7ced0]
I (91) system_api: Base MAC address is not set, read default base MAC address from BLK0 of EFUSE
I (181) phy: phy_version: 4100, 6fa5e27, Jan 25 2019, 17:02:06, 0, 0
I (421) NimBLE_BLE_HeartRate: BLE Host Task Started
GAP procedure initiated: stop advertising.
Device Address: xx:xx:xx:xx:xx:xx
GAP procedure initiated: advertise; disc_mode=2 adv_channel_map=0 own_addr_type=0 adv_filter_policy=0 adv_itvl_min=0 adv_itvl_max=0
connection established; status=0
subscribe event; cur_notify=1
 value handle; val_handle=3
I (21611) BLE_GAP_SUBSCRIBE_EVENT: conn_handle from subscribe=0
GATT procedure initiated: notify; att_handle=3
GATT procedure initiated: notify; att_handle=3
GATT procedure initiated: notify; att_handle=3
GATT procedure initiated: notify; att_handle=3
GATT procedure initiated: notify; att_handle=3
GATT procedure initiated: notify; att_handle=3
GATT procedure initiated: notify; att_handle=3

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
