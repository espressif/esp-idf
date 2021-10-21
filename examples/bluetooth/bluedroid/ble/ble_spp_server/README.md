| Supported Targets | ESP32 | ESP32-C3 |
| ----------------- | ----- | -------- |

## ESP-IDF GATT SERVER SPP Example

For description of this application please refer to [ESP-IDF GATT CLIENT SPP Example](../ble_spp_client/README.md)

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* A development board with ESP32/ESP32-C3 SoC (e.g., ESP32-DevKitC, ESP-WROVER-KIT, etc.)
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

```
I (4452) GATTS_SPP_DEMO: EVT 14, gatts if 3
I (4452) GATTS_SPP_DEMO: event = e
I (5022) GATTS_SPP_DEMO: EVT 4, gatts if 3
I (5022) GATTS_SPP_DEMO: event = 4
I (5152) GATTS_SPP_DEMO: EVT 2, gatts if 3
I (5152) GATTS_SPP_DEMO: event = 2
I (5152) GATTS_SPP_DEMO: ESP_GATTS_WRITE_EVT : handle = 5
I (5242) GATTS_SPP_DEMO: EVT 2, gatts if 3
I (5242) GATTS_SPP_DEMO: event = 2
I (5242) GATTS_SPP_DEMO: ESP_GATTS_WRITE_EVT : handle = 10
I (18462) GATTS_SPP_DEMO: EVT 5, gatts if 3
I (18462) GATTS_SPP_DEMO: event = 5
I (27652) GATTS_SPP_DEMO: EVT 2, gatts if 3
I (27652) GATTS_SPP_DEMO: event = 2
I (27652) GATTS_SPP_DEMO: ESP_GATTS_WRITE_EVT : handle = 2
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
