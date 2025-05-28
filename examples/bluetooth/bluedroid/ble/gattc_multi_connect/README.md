| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP-IDF Gatt Client Multi Connection Example

This example shows the usage of APIs to create a GATT multi-connection client. It can be used to connect to three GATT servers at the same time.

To test this example, please run [gatt_server_demo](../gatt_server) to create three GATT server devices, namely ESP_GATTS_DEMO_a, ESP_GATTS_DEMO_b and ESP_GATTS_DEMO_c, `Gatt_client_multi_connection_demo` will connect to these three gatt server demos, and then exchange data.

Please, check this [tutorial](tutorial/Gatt_Client_Multi_Connection_Example_Walkthrough.md) for more information about this example.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

The code can be modified to connect to more devices (up to 4 devices by default). If you need to connect to more devices (more than 4 devices), you need to change `BT/BLE MAX ACL CONNECTIONS` in menuconfig.

### Hardware Required

* A development board with ESP32/ESP32-C3/ESP32-C2/ESP32-H2/ESP32-S3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
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
I (932) GATTC_MULTIPLE_DEMO: REG_EVT
I (932) GATTC_MULTIPLE_DEMO: REG_EVT
I (932) GATTC_MULTIPLE_DEMO: REG_EVT
I (942) GATTC_MULTIPLE_DEMO: Scan start success
I (1072) GATTC_MULTIPLE_DEMO: 38 68 a4 69 bb 7c
I (1072) GATTC_MULTIPLE_DEMO: Searched Adv Data Len 28, Scan Response Len 0
I (1072) GATTC_MULTIPLE_DEMO: Searched Device Name Len 0
I (1082) GATTC_MULTIPLE_DEMO:

I (1102) GATTC_MULTIPLE_DEMO: 08 ef 3b a7 04 41
I (1102) GATTC_MULTIPLE_DEMO: Searched Adv Data Len 9, Scan Response Len 15
I (1102) GATTC_MULTIPLE_DEMO: Searched Device Name Len 13
I (1112) GATTC_MULTIPLE_DEMO: LG CM2760(41)
I (1112) GATTC_MULTIPLE_DEMO:

I (1222) GATTC_MULTIPLE_DEMO: 38 68 a4 69 bb 7c
I (1222) GATTC_MULTIPLE_DEMO: Searched Adv Data Len 28, Scan Response Len 0
I (1222) GATTC_MULTIPLE_DEMO: Searched Device Name Len 0
I (1232) GATTC_MULTIPLE_DEMO:

I (1372) GATTC_MULTIPLE_DEMO: 38 68 a4 69 bb 7c
I (1372) GATTC_MULTIPLE_DEMO: Searched Adv Data Len 28, Scan Response Len 0
I (1372) GATTC_MULTIPLE_DEMO: Searched Device Name Len 0
I (1382) GATTC_MULTIPLE_DEMO:

I (1412) GATTC_MULTIPLE_DEMO: 08 ef 3b a7 04 41
I (1412) GATTC_MULTIPLE_DEMO: Searched Adv Data Len 9, Scan Response Len 15
I (1422) GATTC_MULTIPLE_DEMO: Searched Device Name Len 13
I (1422) GATTC_MULTIPLE_DEMO: LG CM2760(41)
I (1432) GATTC_MULTIPLE_DEMO:

I (1522) GATTC_MULTIPLE_DEMO: 38 68 a4 69 bb 7c
I (1522) GATTC_MULTIPLE_DEMO: Searched Adv Data Len 28, Scan Response Len 0
I (1522) GATTC_MULTIPLE_DEMO: Searched Device Name Len 0
I (1532) GATTC_MULTIPLE_DEMO:
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
