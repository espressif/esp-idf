| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 | ESP32-H21 | ESP32-H4 | ESP32-S31 |
| ----------------- | -------- | -------- | --------- | -------- | --------- | -------- | --------- |

# BLE Multiple Connection Peripheral Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

**This example relies on the BLE controller. Currently, the ESP32/ESP32-C3/ESP32-S3/ESP32-C2 does not support the relevant vendor HCI, so please use the chip modules listed under Supported Targets.**

Please check the [tutorial](tutorial/Ble_Multiple_Connections_Peripheral_Example_Walkthrough.md) for more information about this example.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* At least two development board with ESP32-C6/ESP32-H2 SoC (e.g., ESP32-C6-DevKitC, ESP32-H2-DevKitC, etc.)
* USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

This is the console output on successful connection:

```
I (...) ESP_MULTI_CONN_PRPH: BLE Host Task Started
I (...) ESP_MULTI_CONN_PRPH: Started adv, Device Address xx:xx:xx:xx:xx:xx
I (...) ESP_MULTI_CONN_PRPH: advertisement completed. Reason=0.
I (...) ESP_MULTI_CONN_PRPH: Connection established. Handle:0. Total:1

I (...) ESP_MULTI_CONN_PRPH: Started adv, Device Address xx:xx:xx:xx:xx:xx
I (...) ESP_MULTI_CONN_PRPH: advertisement completed. Reason=0.
I (...) ESP_MULTI_CONN_PRPH: Connection established. Handle:1. Total:2

I (...) ESP_MULTI_CONN_PRPH: Started adv, Device Address xx:xx:xx:xx:xx:xx
I (...) ESP_MULTI_CONN_PRPH: advertisement completed. Reason=0.
I (...) ESP_MULTI_CONN_PRPH: Connection established. Handle:2. Total:3
...
I (...) ESP_MULTI_CONN_PRPH: Connection established. Handle:63. Total:64
I (...) ESP_MULTI_CONN_PRPH: Started adv, Device Address xx:xx:xx:xx:xx:xx
I (...) ESP_MULTI_CONN_PRPH: advertisement completed. Reason=0.
I (...) ESP_MULTI_CONN_PRPH: Connection established. Handle:64. Total:65
I (...) ESP_MULTI_CONN_PRPH: Started adv, Device Address xx:xx:xx:xx:xx:xx
I (...) ESP_MULTI_CONN_PRPH: advertisement completed. Reason=0.
I (...) ESP_MULTI_CONN_PRPH: Connection established. Handle:65. Total:66
I (...) ESP_MULTI_CONN_PRPH: Started adv, Device Address xx:xx:xx:xx:xx:xx
I (...) ESP_MULTI_CONN_PRPH: advertisement completed. Reason=0.
I (...) ESP_MULTI_CONN_PRPH: Connection established. Handle:66. Total:67
I (...) ESP_MULTI_CONN_PRPH: Started adv, Device Address xx:xx:xx:xx:xx:xx
I (...) ESP_MULTI_CONN_PRPH: advertisement completed. Reason=0.
I (...) ESP_MULTI_CONN_PRPH: Connection established. Handle:67. Total:68
I (...) ESP_MULTI_CONN_PRPH: Started adv, Device Address xx:xx:xx:xx:xx:xx
I (...) ESP_MULTI_CONN_PRPH: advertisement completed. Reason=0.
I (...) ESP_MULTI_CONN_PRPH: Connection established. Handle:68. Total:69
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
