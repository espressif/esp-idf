| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# ESP-IDF BT/BLE HID Host Demo


# ESP-IDF BLE HID Host Demo

This demo use APIs which esp_hid component provided to create a Bluetooth dual mode hid host. After the program is started, the HID host will scan the surrounding Bluetooth HID device and try to connect to the last device which has been scanned. When the connection is successfully established, the HID host will dump the HID device information and can receive the data sent by the HID device.

This example works in companion with the [BLE HID Device Example](../esp_hid_device/README.md)

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

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
I (602) BTDM_INIT: BT controller compile version [d03a5d3]
I (612) system_api: Base MAC address is not set
I (612) system_api: read default base MAC address from EFUSE
I (612) phy_init: phy_version 4670,719f9f6,Feb 18 2021,17:07:07
I (1332) ESP_HIDH_DEMO: SCAN...
I (7212) ESP_HIDH_DEMO: SCAN: 1 results
  BLE: fc:f5:c4:3c:62:92, RSSI: -29, USAGE: GENERIC, APPEARANCE: 0x03c0, ADDR_TYPE: 'PUBLIC', NAME: ESP BLE HID2 
I (7332) ESP_HID_GAP: BLE GAP SEC_REQ
E (7702) BT_SMP: Value for numeric comparison = 125657
I (7702) ESP_HID_GAP: BLE GAP NC_REQ passkey:125657
W (7742) BT_SMP: FOR LE SC LTK IS USED INSTEAD OF STK
I (7892) ESP_HID_GAP: BLE GAP KEY type = ESP_LE_KEY_PID
I (7892) ESP_HID_GAP: BLE GAP KEY type = ESP_LE_KEY_LENC
I (7892) ESP_HID_GAP: BLE GAP KEY type = ESP_LE_KEY_PENC
I (7892) ESP_HID_GAP: BLE GAP KEY type = ESP_LE_KEY_LID
I (8142) ESP_HID_GAP: BLE GAP AUTH SUCCESS
I (9332) ESP_HIDH_DEMO: fc:f5:c4:3c:62:92 OPEN: ESP BLE HID2
BDA:fc:f5:c4:3c:62:92, Appearance: 0x03c0, Connection ID: 0
Name: ESP BLE HID2, Manufacturer: Espressif, Serial Number: 1234567890
PID: 0x05df, VID: 0x16c0, VERSION: 0x0100
Battery: Handle: 42, CCC Handle: 43
Report Maps: 2
  Report Map Length: 38
      VENDOR FEATURE REPORT, ID:  1, Length:   8, Permissions: 0x0a, Handle:  71, CCC Handle:   0
      VENDOR  OUTPUT REPORT, ID:  1, Length:   8, Permissions: 0x0a, Handle:  68, CCC Handle:   0
      VENDOR   INPUT REPORT, ID:  1, Length:   8, Permissions: 0x12, Handle:  64, CCC Handle:  65
  Report Map Length: 111
    CCONTROL   INPUT REPORT, ID:  3, Length:   2, Permissions: 0x12, Handle:  85, CCC Handle:  86
I (9722) ESP_HIDH_DEMO: fc:f5:c4:3c:62:92 INPUT: CCONTROL, MAP:  1, ID:   3, Len: 2, Data:
I (9722) ESP_HIDH_DEMO: 40 00 
I (9812) ESP_HIDH_DEMO: fc:f5:c4:3c:62:92 INPUT: CCONTROL, MAP:  1, ID:   3, Len: 2, Data:
I (9812) ESP_HIDH_DEMO: 00 00 
I (11822) ESP_HIDH_DEMO: fc:f5:c4:3c:62:92 INPUT: CCONTROL, MAP:  1, ID:   3, Len: 2, Data:
I (11822) ESP_HIDH_DEMO: 80 00 
I (11912) ESP_HIDH_DEMO: fc:f5:c4:3c:62:92 INPUT: CCONTROL, MAP:  1, ID:   3, Len: 2, Data:
I (11912) ESP_HIDH_DEMO: 00 00 
I (13922) ESP_HIDH_DEMO: fc:f5:c4:3c:62:92 INPUT: CCONTROL, MAP:  1, ID:   3, Len: 2, Data:
I (13922) ESP_HIDH_DEMO: 40 00 
I (14012) ESP_HIDH_DEMO: fc:f5:c4:3c:62:92 INPUT: CCONTROL, MAP:  1, ID:   3, Len: 2, Data:
I (14012) ESP_HIDH_DEMO: 00 00 
I (16022) ESP_HIDH_DEMO: fc:f5:c4:3c:62:92 INPUT: CCONTROL, MAP:  1, ID:   3, Len: 2, Data:
I (16022) ESP_HIDH_DEMO: 80 00 
I (16112) ESP_HIDH_DEMO: fc:f5:c4:3c:62:92 INPUT: CCONTROL, MAP:  1, ID:   3, Len: 2, Data:
I (16112) ESP_HIDH_DEMO: 00 00 
I (18122) ESP_HIDH_DEMO: fc:f5:c4:3c:62:92 INPUT: CCONTROL, MAP:  1, ID:   3, Len: 2, Data:
I (18122) ESP_HIDH_DEMO: 40 00 
I (18212) ESP_HIDH_DEMO: fc:f5:c4:3c:62:92 INPUT: CCONTROL, MAP:  1, ID:   3, Len: 2, Data:
I (18212) ESP_HIDH_DEMO: 00 00 
...
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
