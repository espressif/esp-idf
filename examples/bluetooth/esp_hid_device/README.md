| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S3 | ESP32-S31 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | --------- | -------- | -------- | --------- |

# ESP-IDF BT/BLE HID Device Demo

This demo use APIs which esp_hid component provided to create a BT, BLE or Bluetooth dual mode hid device. Users can choose mode by setting `HID_DEV_MODE`.


This example works in companion with the [BLE HID Host Example](../esp_hid_host/README.md)

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```
The BT hid device plays as a mouse. When the connection is successfully established, users can follow the usage below to operate the 'mouse'.

```
########################################################################
BT hid mouse demo usage:
You can input these value to simulate mouse: 'q', 'w', 'e', 'a', 's', 'd', 'h'
q -- click the left key
w -- move up
e -- click the right key
a -- move left
s -- move down
d -- move right
h -- show the help
########################################################################
```

The BLE hid device plays as a remote control. When the connection is successfully established, the remote control will set volume up and down periodically.
This example implements a BLE HID device.

### Hardware Required

* A development board with ESP32 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Configure the Project

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output.

```
idf.py -p PORT flash monitor
```

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (0) cpu_start: Starting scheduler on APP CPU.
I (607) BTDM_INIT: BT controller compile version [d03a5d3]
I (607) system_api: Base MAC address is not set
I (607) system_api: read default base MAC address from EFUSE
I (617) phy_init: phy_version 4670,719f9f6,Feb 18 2021,17:07:07
W (1337) BT_BTM: BTM_BleWriteAdvData, Partial data write into ADV
I (1357) HID_DEV_DEMO: START
I (25067) HID_DEV_DEMO: CONNECT
I (25357) HID_DEV_DEMO: Send the volume
E (25437) BT_SMP: Value for numeric comparison = 125657
I (25437) ESP_HID_GAP: BLE GAP NC_REQ passkey:125657
W (25517) BT_SMP: FOR LE SC LTK IS USED INSTEAD OF STK
I (25587) ESP_HID_GAP: BLE GAP KEY type = ESP_LE_KEY_LENC
I (25587) ESP_HID_GAP: BLE GAP KEY type = ESP_LE_KEY_PENC
I (25587) ESP_HID_GAP: BLE GAP KEY type = ESP_LE_KEY_LID
I (25647) ESP_HID_GAP: BLE GAP KEY type = ESP_LE_KEY_PID
I (25757) ESP_HID_GAP: BLE GAP AUTH SUCCESS
I (27457) HID_DEV_DEMO: Send the volume
I (29557) HID_DEV_DEMO: Send the volume
I (31657) HID_DEV_DEMO: Send the volume
I (33757) HID_DEV_DEMO: Send the volume
I (35857) HID_DEV_DEMO: Send the volume
I (37957) HID_DEV_DEMO: Send the volume
I (40057) HID_DEV_DEMO: Send the volume
I (42157) HID_DEV_DEMO: Send the volume
I (44257) HID_DEV_DEMO: Send the volume
I (46357) HID_DEV_DEMO: Send the volume
I (48457) HID_DEV_DEMO: Send the volume
I (50557) HID_DEV_DEMO: Send the volume
...
```

## NimBLE Logging Scheme

The following are verbatim NimBLE log excerpts from `logs.txt` for each device role.

### Media Mode

```text
I (496) HID_DEV_DEMO: setting ble device
I (496) HID_DEV_DEMO: BLE Host Task Started
I (506) HID_DEV_BLE: START
I (506) NimBLE: GAP procedure initiated: advertise;
I (35886) ESP_HID_GAP: connection established; status=0
I (35896) HID_DEV_BLE: CONNECT
I (35946) ESP_HID_GAP: mtu update event; conn_handle=0 cid=4 mtu=256
I (36476) ESP_HID_GAP: PASSKEY_ACTION_EVENT started
I (36476) ESP_HID_GAP: Enter passkey 123456on the peer side
I (37896) ESP_HID_GAP: subscribe event; conn_handle=0 attr_handle=37 reason=1 prevn=0 curn=1 previ=0 curi=0
I (40936) NimBLE: encryption change event; status=0
I (40946) HID_DEV_DEMO: Send the volume
I (40946) NimBLE: GATT procedure initiated: notify;
I (40946) NimBLE: att_handle=37
I (40946) NimBLE: notify_tx event; conn_handle=0 attr_handle=37 status=0 is_indication=0
```

### Keyboard Mode

```text
I (436) HID_DEV_DEMO: setting ble device
I (436) HID_DEV_DEMO: BLE Host Task Started
I (436) HID_DEV_BLE: START
I (446) NimBLE: GAP procedure initiated: advertise;
I (11526) ESP_HID_GAP: connection established; status=0
I (11526) HID_DEV_BLE: CONNECT
I (11576) ESP_HID_GAP: mtu update event; conn_handle=0 cid=4 mtu=256
I (12116) ESP_HID_GAP: PASSKEY_ACTION_EVENT started
I (12116) ESP_HID_GAP: Enter passkey 123456on the peer side
I (13826) ESP_HID_GAP: subscribe event; conn_handle=0 attr_handle=37 reason=1 prevn=0 curn=1 previ=0 curi=0
I (16576) NimBLE: encryption change event; status=0
########################################################################
BT hid keyboard demo usage:
########################################################################
I (22306) NimBLE: GATT procedure initiated: notify;
I (22306) NimBLE: att_handle=37
I (22306) NimBLE: notify_tx event; conn_handle=0 attr_handle=37 status=0 is_indication=0
```

### Mouse Mode

```text
I (446) HID_DEV_DEMO: setting ble device
I (446) HID_DEV_DEMO: BLE Host Task Started
I (456) HID_DEV_BLE: START
I (456) NimBLE: GAP procedure initiated: advertise;
I (10556) ESP_HID_GAP: connection established; status=0
I (10556) HID_DEV_BLE: CONNECT
I (10606) ESP_HID_GAP: mtu update event; conn_handle=0 cid=4 mtu=256
I (11136) ESP_HID_GAP: PASSKEY_ACTION_EVENT started
I (11136) ESP_HID_GAP: Enter passkey 123456on the peer side
I (12656) ESP_HID_GAP: subscribe event; conn_handle=0 attr_handle=37 reason=1 prevn=0 curn=1 previ=0 curi=0
I (15606) NimBLE: encryption change event; status=0
########################################################################
BT hid mouse demo usage:
You can input these value to simulate mouse: 'q', 'w', 'e', 'a', 's', 'd', 'h'
q -- click the left key
w -- move up
e -- click the right key
a -- move left
s -- move down
d -- move right
h -- show the help
########################################################################
I (18166) NimBLE: GATT procedure initiated: notify;
I (18166) NimBLE: att_handle=37
I (18166) NimBLE: notify_tx event; conn_handle=0 attr_handle=37 status=0 is_indication=0
```

## Troubleshooting

1. When using NimBLE stack, some iOS devices do not show the volume pop up. To fix this, please set CONFIG_BT_NIMBLE_SM_LVL to value 2. iOS needs Authenticated Pairing with Encryption to show up the pop ups.
2. For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
