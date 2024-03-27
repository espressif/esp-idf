| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

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

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
