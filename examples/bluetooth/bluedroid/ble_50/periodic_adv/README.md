| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP_IDF Periodic Adv Example

This example support for the periodic advertisement which allow the scanner to sync with the advertiser so that scanner and advertiser wake up same time. It support extended adv with 2M phy in connectable mode.


To test this demo, we can run the [periodic_sync_demo](../periodic_sync), which can do periodic scan and try to sync with periodic adv.


Please, check this [tutorial](tutorial/Periodic_adv_Example_Walkthrough.md) for more information about this example.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* A development board with ESP32-C3 SoC, ESP32-S3, ESP32-C2, ESP32-H2 and  BLE5.0 supported chips (e.g., ESP32-C3-DevKitC-1, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (362) BTDM_INIT: BT controller compile version [3e61eea]
I (372) coexist: coexist rom version 8459080
I (372) phy_init: phy_version 500,985899c,Apr 19 2021,16:05:08
I (492) system_api: Base MAC address is not set
I (492) system_api: read default base MAC address from EFUSE
I (492) BTDM_INIT: Bluetooth MAC: 7c:df:a1:40:01:dd

I (712) MULTI_ADV_DEMO: ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT, status 0
I (712) MULTI_ADV_DEMO: ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT, status 0
I (712) MULTI_ADV_DEMO: ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT, status 0
I (722) MULTI_ADV_DEMO: ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT, status 0
I (732) MULTI_ADV_DEMO: ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT, status 0
I (742) MULTI_ADV_DEMO: ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT, status 0
I (742) MULTI_ADV_DEMO: ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT, status 0

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
