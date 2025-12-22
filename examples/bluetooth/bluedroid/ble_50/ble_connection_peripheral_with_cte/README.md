| Supported Targets | ESP32-C5 | ESP32-C61 | ESP32-H2 |
| ----------------- | -------- | --------- | -------- |

# ESP-IDF BLE Connection Peripheral with CTE Example

This example demonstrates how to use BLE Constant Tone Extension (CTE) feature as a Peripheral device using Bluedroid host API.

## What is BLE CTE?

BLE Constant Tone Extension (CTE) is a BLE 5.0 feature that enables Direction Finding (AoA/AoD) functionality. CTE allows devices to determine the direction of a Bluetooth signal by analyzing the phase of the received signal.

## How It Works

1. The Peripheral device starts extended advertising
2. When a Central device connects to it
3. After connection, the Peripheral configures CTE transmit parameters
4. The Peripheral enables CTE response to transmit CTE to the Central
5. The Peripheral receives IQ samples for direction finding

## API Usage

- `esp_ble_cte_register_callback()` - Register CTE event callback
- `esp_ble_cte_set_connection_transmit_params()` - Set CTE transmit parameters for connection
- `esp_ble_cte_connection_cte_response_enable()` - Enable CTE response on connection
- `ESP_BLE_CTE_CONN_IQ_REPORT_EVT` - Event received when IQ samples are available

## Configuration

The example requires BLE 5.0 CTE features to be enabled:
- `CONFIG_BT_BLE_50_FEATURES_SUPPORTED=y`
- `CONFIG_BT_BLE_FEAT_CTE_EN=y`
- `CONFIG_BT_LE_CTE_FEATURE_ENABLED=y`

## Hardware Required

* A development board with ESP32-C5, ESP32-C61, or ESP32-H2 SoC that supports BLE 5.0 CTE
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target esp32c5  # or esp32c61 or esp32h2
```

To test this demo, you can run the [ble_connection_central_with_cte](../ble_connection_central_with_cte) example on another device, which will scan and connect to this demo automatically.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

The example will output CTE-related events including:
- Extended advertising start completion
- Connection establishment
- CTE parameter configuration completion
- CTE response enable completion
- IQ sample reports for direction finding

```
I (xxx) CTE_TEST: Extended advertising started
I (xxx) CTE_TEST: Connection established
I (xxx) CTE_TEST: CTE set connection transmit params, status 0
I (xxx) CTE_TEST: CTE set connection response enable, status 0
I (xxx) CTE_TEST: CTE connection IQ report received
...
```
## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
