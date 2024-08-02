| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP-IDF Gatt Security Client Example

This example shows how to use the ESP BLE security APIs to secure connect to and encrypt with peer devices.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

To test this demo, you can run the [gatt_security_server_demo](../gatt_security_server), which starts advertising and can be connected to this demo automatically.

There are some important points for this demo:
1. `esp_ble_gap_set_security_param` should be used to set the security parameters in the initial stage;
2. `esp_ble_set_encryption` should be used to start encryption with peer device. If the peer device initiates the encryption, `esp_ble_gap_security_rsp` should be used to send security response to the peer device when `ESP_GAP_BLE_SEC_REQ_EVT` is received.
3. The `gatt_security_client_demo` will receive a `ESP_GAP_BLE_AUTH_CMPL_EVT` once the encryption procedure has completed.

Please, check this [tutorial](tutorial/Gatt_Security_Client_Example_Walkthrough.md) for more information about this example.

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
I (572) BTDM_INIT: BT controller compile version [1342a48]
I (572) system_api: Base MAC address is not set
I (572) system_api: read default base MAC address from EFUSE
I (582) phy_init: phy_version 4670,719f9f6,Feb 18 2021,17:07:07
I (1002) SEC_GATTC_DEMO: EVT 0, gattc if 1
I (1002) SEC_GATTC_DEMO: REG_EVT
I (1032) SEC_GATTC_DEMO: Scan start success
I (1242) SEC_GATTC_DEMO: 38 68 a4 69 bb 7c
I (1242) SEC_GATTC_DEMO: Searched Adv Data Len 31, Scan Response Len 14
I (1242) SEC_GATTC_DEMO: Searched Device Name Len 0
I (1242) SEC_GATTC_DEMO:

I (1262) SEC_GATTC_DEMO: 38 68 a4 69 bb 7c
I (1262) SEC_GATTC_DEMO: Searched Adv Data Len 31, Scan Response Len 14
I (1262) SEC_GATTC_DEMO: Searched Device Name Len 0
I (1272) SEC_GATTC_DEMO:

I (1592) SEC_GATTC_DEMO: 38 68 a4 69 bb 7c
I (1592) SEC_GATTC_DEMO: Searched Adv Data Len 31, Scan Response Len 14
I (1592) SEC_GATTC_DEMO: Searched Device Name Len 0
I (1602) SEC_GATTC_DEMO:

I (1912) SEC_GATTC_DEMO: 38 68 a4 69 bb 7c
I (1912) SEC_GATTC_DEMO: Searched Adv Data Len 31, Scan Response Len 14
I (1912) SEC_GATTC_DEMO: Searched Device Name Len 0
I (1922) SEC_GATTC_DEMO:

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
