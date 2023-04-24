| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | -------- |

# ESP-IDF BLE50 Security Server Example

This example shows how to use the APIs to connect in secure manner with peer device and use encryption for data exchange.

To test this example, you can run [ble50_security_client_demo](../ble50_security_client), which starts scanning, connects to and starts encryption with `ble50_sec_gattc_demo` automatically.

Please, check this [tutorial](tutorial/ble50_security_server_Example_Walkthrough.md) for more information about this example.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```
There are some important points for this demo:
1.`esp_ble_gap_set_security_param` should be used to set the security parameters in the initial stage;
2.`esp_ble_set_encryption` should be used to start encryption with peer device. If the peer device initiates the encryption,
  `esp_ble_gap_security_rsp` should be used to send security response to the peer device when `ESP_GAP_BLE_SEC_REQ_EVT` is received.
3.The `ble50_sec_gattc_demo` will receive a `ESP_GAP_BLE_AUTH_CMPL_EVT` once the encryption procedure has completed.

### Hardware Required

* A development board with ESP32-C3 SoC, ESP32-S3, ESP32-C2/ESP32-H2 SoC and BLE5.0 supoorted chips. (e.g., ESP32-C3-DevKitC-1, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (363) BTDM_INIT: BT controller compile version [3e61eea]
I (373) coexist: coexist rom version 8459080
I (373) phy_init: phy_version 500,985899c,Apr 19 2021,16:05:08
I (493) system_api: Base MAC address is not set
I (493) system_api: read default base MAC address from EFUSE
I (493) BTDM_INIT: Bluetooth MAC: 7c:df:a1:40:01:c5

I (503) SEC_GATTS_DEMO: app_main init bluetooth
I (523) SEC_GATTS_DEMO: ESP_GATTS_REG_EVT
I (523) SEC_GATTS_DEMO: The number handle = 8
I (523) SEC_GATTS_DEMO: ESP_GAP_BLE_SET_LOCAL_PRIVACY_COMPLETE_EVT, tatus = 0
I (523) SEC_GATTS_DEMO: ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT status 0
I (533) SEC_GATTS_DEMO: ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT status 0
I (543) SEC_GATTS_DEMO: ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT, status = 0

```
## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
