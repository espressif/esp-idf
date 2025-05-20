| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | -------- | -------- |

#ESP-IDF Multi Adv Example

This example support legacy as well as extended advertisement for all phy.

Please, check this [tutorial](tutorial/Mulit_Adv_Example_Walkthrough.md) for more information about this example.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* A development board with ESP32-C3 SoC,ESP32-S3/ESP32-H2/ESP32-C2 SoC and BLE5.0 supported chips (e.g., ESP32-C3-DevKitC, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.


## Example Output

```
I (361) BTDM_INIT: BT controller compile version [3e61eea]
I (371) coexist: coexist rom version 8459080
I (371) phy_init: phy_version 500,985899c,Apr 19 2021,16:05:08
I (491) system_api: Base MAC address is not set
I (491) system_api: read default base MAC address from EFUSE
I (491) BTDM_INIT: Bluetooth MAC: 7c:df:a1:40:01:c5

I (711) MULTI_ADV_DEMO: ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT, status 0
I (711) MULTI_ADV_DEMO: ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT, status 0
I (711) MULTI_ADV_DEMO: ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT, status 0
I (721) MULTI_ADV_DEMO: ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT, status 0
I (731) MULTI_ADV_DEMO: ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT, status 0
I (741) MULTI_ADV_DEMO: ESP_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT, status 0
I (741) MULTI_ADV_DEMO: ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT, status 0
I (751) MULTI_ADV_DEMO: ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT, status 0
I (761) MULTI_ADV_DEMO: ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT, status 0
I (771) MULTI_ADV_DEMO: ESP_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT, status 0
I (781) MULTI_ADV_DEMO: ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT, status 0
I (781) MULTI_ADV_DEMO: ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT, status 0
I (791) MULTI_ADV_DEMO: ESP_GAP_BLE_EXT_SCAN_RSP_DATA_SET_COMPLETE_EVT, status 0
I (801) MULTI_ADV_DEMO: ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT, status 0

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
