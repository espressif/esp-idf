| Supported Targets | ESP32-C5 | ESP32-C61 | ESP32-H2 |
| ----------------- | -------- | --------- | -------- |

# ESP-IDF BLE Periodic Advertising with CTE Example

This example demonstrates how to use BLE Periodic Advertising with Constant Tone Extension (CTE) feature using Bluedroid host API.

## What is BLE Periodic Advertising with CTE?

BLE Periodic Advertising with CTE combines periodic advertising with Constant Tone Extension to enable Direction Finding (AoA/AoD) functionality in periodic advertising scenarios. This allows devices to determine the direction of a Bluetooth signal during periodic advertising.

## How It Works

1. The device starts extended advertising
2. The device enables periodic advertising
3. The device configures CTE parameters for periodic advertising
4. The device transmits periodic advertising packets with CTE
5. IQ samples can be received for direction finding

## API Usage

- `esp_ble_gap_ext_adv_start()` - Start extended advertising
- `esp_ble_gap_periodic_adv_start()` - Start periodic advertising
- `esp_ble_cte_set_periodic_adv_transmit_params()` - Set CTE transmit parameters for periodic advertising
- `esp_ble_cte_periodic_adv_response_enable()` - Enable CTE response for periodic advertising

## Configuration

The example requires BLE 5.0 CTE and periodic advertising features to be enabled:
- `CONFIG_BT_BLE_50_FEATURES_SUPPORTED=y`
- `CONFIG_BT_BLE_50_EXTEND_ADV_EN=y`
- `CONFIG_BT_BLE_50_PERIODIC_ADV_EN=y`
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

To test this demo, you can run the [ble_periodic_sync_with_cte](../ble_periodic_sync_with_cte) example on another device, which will synchronize with the periodic advertising and receive CTE.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

The example will output periodic advertising and CTE-related events including:
- Extended advertising start completion
- Periodic advertising start completion
- CTE parameter configuration completion
- CTE response enable completion

```
I (xxx) CTE_TEST: Extended advertising started
I (xxx) CTE_TEST: Periodic advertising started
I (xxx) CTE_TEST: CTE set periodic adv transmit params, status 0
I (xxx) CTE_TEST: CTE periodic adv response enable, status 0
...
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.
