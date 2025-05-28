| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP-IDF Periodic Sync Example

This example supports the periodic extended scan to scan the extended advertisement.

To test this demo, we can run the [periodic_adv_demo](../periodic_adv), which can start extended advertisement with supported param.

Please, check this [tutorial](tutorial/Periodic_Sync_Example_Walkthrough.md) for more information about this example.

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
I (492) BTDM_INIT: Bluetooth MAC: 7c:df:a1:40:01:c5

Output1 Without advertisement:

I (712) PERIODIC_SYNC: ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT, status 0
I (712) PERIODIC_SYNC: ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT, status 0

Output2 with advetisement:

I (712) PERIODIC_SYNC: ESP_GAP_BLE_SET_EXT_SCAN_PARAMS_COMPLETE_EVT, status 0
I (712) PERIODIC_SYNC: ESP_GAP_BLE_EXT_SCAN_START_COMPLETE_EVT, status 0
I (712) PERIODIC_SYNC: Start create sync with the peer device ESP_MULTI_ADV_80MS
I (722) PERIODIC_SYNC: ESP_GAP_BLE_PERIODIC_ADV_CREATE_SYNC_COMPLETE_EVT, status 0
I (812) PERIODIC_SYNC: ESP_GAP_BLE_PERIODIC_ADV_SYNC_ESTAB_EVT, status 0
I (812) sync addr: c0 de 52 00 00 02
I (812) PERIODIC_SYNC: sync handle 1 sid 0 perioic adv interval 64 adv phy 2
I (812) PERIODIC_SYNC: periodic adv report, sync handle 1 data status 0 data len 28 rssi -48
I (892) PERIODIC_SYNC: periodic adv report, sync handle 1 data status 0 data len 28 rssi -47
I (972) PERIODIC_SYNC: periodic adv report, sync handle 1 data status 0 data len 28 rssi -48
I (1052) PERIODIC_SYNC: periodic adv report, sync handle 1 data status 0 data len 28 rssi -47
I (1132) PERIODIC_SYNC: periodic adv report, sync handle 1 data status 0 data len 28 rssi -49
I (1212) PERIODIC_SYNC: periodic adv report, sync handle 1 data status 0 data len 28 rssi -48
I (1292) PERIODIC_SYNC: ESP_GAP_BLE_PERIODIC_ADV_SYNC_LOST_EVT, sync handle 1

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.

