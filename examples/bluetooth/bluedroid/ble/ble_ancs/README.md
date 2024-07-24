| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP-IDF BLE ANCS Example

The purpose of the Apple Notification Center Service (ANCS) is to give Bluetooth accessories (that connect to iOS devices through a Bluetooth low-energy link) a simple and convenient way to access many kinds of notifications that are generated on iOS devices.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

The Apple Notification Center Service is a primary service whose service UUID is:

`7905F431-B5CE-4E99-A40F-4B1E122D00D0`

Only one instance of the ANCS may be present on an NP. Due to the nature of iOS, the ANCS is not guaranteed to always be present. As a result, the NC should look for and subscribe to the Service Changed characteristic of the GATT service in order to monitor for the potential publishing and unpublishing of the ANCS at any time.

In its basic form, the ANCS exposes three characteristics:
Notification Source: UUID `9FBF120D-6301-42D9-8C58-25E699A21DBD` (notifiable)
Control Point: UUID `69D1D8F3-45E1-49A8-9821-9BBDFDAAD9D9` (writeable with response)
Data Source: UUID `22EAC6E9-24D6-4BB5-BE44-B36ACE7C7BFB` (notifiable)

All these characteristics require authorization for access.

### Hardware Required

* A development board with ESP32/ESP32-C3/ESP32-H2/ESP32-C2/ESP32-S3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (0) cpu_start: Starting scheduler on APP CPU.
I (558) BTDM_INIT: BT controller compile version [1342a48]
I (568) system_api: Base MAC address is not set
I (568) system_api: read default base MAC address from EFUSE
I (568) phy_init: phy_version 4670,719f9f6,Feb 18 2021,17:07:07
I (918) BLE_ANCS: app_main init bluetooth
I (1018) BLE_ANCS: REG_EVT
E (1028) BT_BTM: BTM_BleConfigLocalIcon

I (1058) BLE_ANCS: advertising start success
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
