| Supported Targets | ESP32-C2 | ESP32-C3 | ESP32-C5 | ESP32-C6 | ESP32-H2 | ESP32-S3 |
| ----------------- | -------- | -------- | -------- | -------- | -------- | -------- |

# ESP-IDF Gatt Security Client Example

This example shows how to use the ESP BLE5.0 security APIs to connect in secure manner with peer device and use encryption for data exchange.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

To test this demo, you can run the [ble50_security_server_demo](../ble50_security_server), which starts advertising and can be connected to
this demo automatically.

There are some important points for this demo:
1. `esp_ble_gap_set_security_param` should be used to set the security parameters in the initial stage;
2. `esp_ble_set_encryption` should be used to start encryption with peer device. If the peer device initiates the encryption,
   `esp_ble_gap_security_rsp` should be used to send security response to the peer device when `ESP_GAP_BLE_SEC_REQ_EVT` is received.
3. The `gatt_security_client_demo` will receive a `ESP_GAP_BLE_AUTH_CMPL_EVT` once the encryption procedure has completed.

Please, check this [tutorial](tutorial/ble50_security_client_Example_Walkthrough.md) for more information about this example.

### Hardware Required

* A development board with ESP32-C3 SoC, ESP32-S3 SoC, ESP32-C2/ESP32-H2 SoC and BT5.0 supported chip (e.g., ESP32-C3-DevKitC-1 etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (364) BTDM_INIT: BT controller compile version [3e61eea]
I (374) coexist: coexist rom version 8459080
I (374) phy_init: phy_version 500,985899c,Apr 19 2021,16:05:08
I (494) system_api: Base MAC address is not set
I (494) system_api: read default base MAC address from EFUSE
I (494) BTDM_INIT: Bluetooth MAC: 7c:df:a1:40:01:dd

I (514) SEC_GATTC_DEMO: EVT 0, gattc if 3
I (514) SEC_GATTC_DEMO: REG_EVT
I (524) SEC_GATTC_DEMO: Scan start success
I (2654) SEC_GATTC_DEMO: legacy adv, adv type 0x13 data len 31
I (2654) SEC_GATTC_DEMO: legacy adv, adv type 0x1b data len 21
I (3624) SEC_GATTC_DEMO: legacy adv, adv type 0x13 data len 31
I (3624) SEC_GATTC_DEMO: legacy adv, adv type 0x1b data len 21
I (4594) SEC_GATTC_DEMO: legacy adv, adv type 0x13 data len 31
I (4594) SEC_GATTC_DEMO: legacy adv, adv type 0x1b data len 21
I (5514) SEC_GATTC_DEMO: legacy adv, adv type 0x13 data len 31
I (5524) SEC_GATTC_DEMO: legacy adv, adv type 0x1b data len 0
I (7494) SEC_GATTC_DEMO: legacy adv, adv type 0x13 data len 31
I (7494) SEC_GATTC_DEMO: legacy adv, adv type 0x1b data len 21
I (8614) SEC_GATTC_DEMO: legacy adv, adv type 0x13 data len 31
I (8614) SEC_GATTC_DEMO: legacy adv, adv type 0x1b data len 0

```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
