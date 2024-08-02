| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP-IDF Gatt Security Server Example

This example shows how to use the APIs to connect to and encrypt with peer devices.

To test this example, you can run [gatt_security_client_demo](../gatt_security_client), which starts scanning, connects to and starts encryption with `gatt_security_server_demo` automatically.

Please, check this [tutorial](tutorial/Gatt_Security_Server_Example_Walkthrough.md) for more information about this example.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```
There are some important points for this demo:
1.`esp_ble_gap_set_security_param` should be used to set the security parameters in the initial stage;
2.`esp_ble_set_encryption` should be used to start encryption with peer device. If the peer device initiates the encryption, `esp_ble_gap_security_rsp` should be used to send security response to the peer device when `ESP_GAP_BLE_SEC_REQ_EVT` is received.
3.The `gatt_security_client_demo` will receive a `ESP_GAP_BLE_AUTH_CMPL_EVT` once the encryption procedure has completed.

### Hardware Required

* A development board with ESP32/ESP32-C3/ESP32-C2/ESP32-H2/ESP32-S3 Soc (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (0) cpu_start: Starting scheduler on APP CPU.
I (545) BTDM_INIT: BT controller compile version [d03a5d3]
I (545) system_api: Base MAC address is not set
I (545) system_api: read default base MAC address from EFUSE
I (555) phy_init: phy_version 4670,719f9f6,Feb 18 2021,17:07:07
I (905) SEC_GATTS_DEMO: app_main init bluetooth
I (1015) SEC_GATTS_DEMO: ESP_GAP_BLE_LOCAL_IR_EVT
I (1015) SEC_GATTS_DEMO: ESP_GAP_BLE_LOCAL_ER_EVT
I (1035) SEC_GATTS_DEMO: The number handle = 8
I (1065) SEC_GATTS_DEMO: advertising start success
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
