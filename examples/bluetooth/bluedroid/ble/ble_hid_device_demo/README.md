| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# ESP-IDF BLE HID Example

This example implement a BLE HID device profile related functions, in which the HID device has 4 Reports:

1. Mouse
2. Keyboard and LED
3. Consumer Devices
4. Vendor devices

Users can choose different reports according to their own application scenarios.
BLE HID profile inheritance and USB HID class.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* A development board with ESP32/ESP32-C3/ESP32-H2/ESP32-C2/ESP32-S3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Configure the Project

* `ble_hidd_demo_main.c`
This file is the demo to show how to used the HID(you can used it to connected to the smart phone act as the consumer device then can used the button to
volume++ or volume-- etc., or connected to the Windows 10 PC act as a keyboard or mouse)

* `hidd_le_prf_int.h`
This header file includes some HID profile related definitions.

* `esp_hidd_prf_api.h` & `esp_hidd_prf_api.c`
These files contains the the api of the HID profile
When you used the HID profile, you just need to added the esp_hidd_prf_api.h includes file and send the HID data used the function defined in the esp_hidd_prf_api.c file.

* `hid_dev.h & hid_dev.c`
These file define the HID spec related definitions

* `hid_device_le_prf.c`
This file is the HID profile definition file, it include the main function of the HID profile.
It mainly includes how to create HID service. If you send and receive HID data and convert the data to keyboard keys,
the mouse and consumer values are forwarded to the application.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (0) cpu_start: Starting scheduler on APP CPU.
I (584) BTDM_INIT: BT controller compile version [1342a48]
I (584) system_api: Base MAC address is not set
I (584) system_api: read default base MAC address from EFUSE
I (594) phy_init: phy_version 4670,719f9f6,Feb 18 2021,17:07:07
I (1024) HID_LE_PRF: esp_hidd_prf_cb_hdl(), start added the hid service to the stack database. incl_handle = 40
I (1034) HID_LE_PRF: hid svc handle = 2d
I (5964) HID_LE_PRF: HID connection establish, conn_id = 0
I (5964) HID_DEMO: ESP_HIDD_EVENT_BLE_CONNECT
I (6744) HID_DEMO: remote BD_ADDR: 7767f4abe386
I (6744) HID_DEMO: address type = 1
I (6744) HID_DEMO: pair status = success
I (8024) HID_DEMO: Send the volume
I (16024) HID_DEMO: Send the volume
I (24024) HID_DEMO: Send the volume
I (27784) HID_DEMO: ESP_HIDD_EVENT_BLE_DISCONNECT
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
