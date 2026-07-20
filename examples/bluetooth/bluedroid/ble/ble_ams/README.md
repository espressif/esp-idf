| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | --------- | -------- | -------- |

# ESP-IDF BLE AMS Example

The purpose of the Apple Media Service (AMS) is to give Bluetooth accessories (that connect to iOS devices through a Bluetooth Low Energy link) a simple and convenient way to control media playback and access metadata such as track information and playback state.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

The Apple Media Service is a primary service whose service UUID is:

89D3502B-0F36-433A-8EF4-C502AD55F8DC

Only one instance of the AMS may be present on an iOS device. The AMS exposes three characteristics:

Remote Command: UUID 9B3C81D8-57B1-4A8A-8634-EC0BBD7C7D35 (writable with response)
Entity Update: UUID 2F7CABCE-808D-411F-9A0C-BB92BA96C102 (notifiable)
Entity Attribute: UUID C6B2F38C-23AB-46D8-A6AB-A3A870BBD5D7 (writable with response)

These characteristics require proper authorization and pairing for access.

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
I (918) BLE_AMS: app_main init bluetooth
I (1018) BLE_AMS: REG_EVT
I (1058) BLE_AMS: advertising start success

I (1058) BLE_AMS: advertising start success
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
