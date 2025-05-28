| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP-IDF Gatt Server Service Table Example

This example shows how to create a GATT service with an attribute table defined in one place. Provided API releases the user from adding attributes one by one as implemented in BLUEDROID. A demo of the other method to create the attribute table is presented in [gatt_server_demo](../gatt_server).

Please, check this [tutorial](tutorial/Gatt_Server_Service_Table_Example_Walkthrough.md) for more information about this example.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* A development board with ESP32/ESP32-C3/ESP32-H2/ESP32-C2/ESP32-S3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (0) cpu_start: Starting scheduler on APP CPU.
I (512) BTDM_INIT: BT controller compile version [1342a48]
I (522) system_api: Base MAC address is not set
I (522) system_api: read default base MAC address from EFUSE
I (522) phy_init: phy_version 4670,719f9f6,Feb 18 2021,17:07:07
I (942) GATTS_TABLE_DEMO: create attribute table successfully, the number handle = 8

I (942) GATTS_TABLE_DEMO: SERVICE_START_EVT, status 0, service_handle 40
I (962) GATTS_TABLE_DEMO: advertising start successfully
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
