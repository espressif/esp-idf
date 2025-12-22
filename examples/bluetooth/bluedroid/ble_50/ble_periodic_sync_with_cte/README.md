| Supported Targets | ESP32-C5 | ESP32-C61 | ESP32-H2 |
| ----------------- | -------- | --------- | -------- |

# ESP-IDF BLE Periodic Sync with CTE Example

This example demonstrates how to use BLE Periodic Synchronization with Constant Tone Extension (CTE) feature using Bluedroid host API.

## What is BLE Periodic Sync with CTE?

BLE Periodic Sync with CTE combines periodic synchronization with Constant Tone Extension to enable Direction Finding (AoA/AoD) functionality when synchronizing with periodic advertising. This allows devices to determine the direction of a Bluetooth signal during periodic advertising synchronization.

## How It Works

1. The device starts extended scanning
2. The device creates periodic sync with a periodic advertiser
3. The device configures CTE receive parameters for periodic sync
4. The device receives periodic advertising packets with CTE
5. IQ samples are received for direction finding

## API Usage

- `esp_ble_gap_ext_scan_start()` - Start extended scanning
- `esp_ble_gap_periodic_adv_create_sync()` - Create periodic sync
- `esp_ble_cte_set_periodic_adv_receive_params()` - Set CTE receive parameters for periodic sync
- `esp_ble_cte_periodic_adv_request_enable()` - Enable CTE request for periodic sync

## Configuration

The example requires BLE 5.0 CTE and periodic advertising features to be enabled:
- `CONFIG_BT_BLE_50_FEATURES_SUPPORTED=y`
- `CONFIG_BT_BLE_50_EXTEND_SCAN_EN=y`
- `CONFIG_BT_BLE_50_PERIODIC_ADV_EN=y`
- `CONFIG_BT_BLE_FEAT_CTE_EN=y`
- `CONFIG_BT_LE_CTE_FEATURE_ENABLED=y`

To test this demo, you can run the [ble_periodic_adv_with_cte](../ble_periodic_adv_with_cte) example on another device, which starts periodic advertising with CTE.

## How to Use Example

Before project configuration and build, be sure to set the correct chip target using:

```bash
idf.py set-target <chip_name>
```

### Hardware Required

* A development board with ESP32-C5, ESP32-C61, or ESP32-H2 SoC that supports BLE 5.0 CTE
* A USB cable for Power supply and programming

See [Development Boards](https://www.espressif.com/en/products/devkits) for more information about it.

### Build and Flash

Run `idf.py -p PORT flash monitor` to build, flash and monitor the project.

(To exit the serial monitor, type ``Ctrl-]``.)

See the [Getting Started Guide](https://idf.espressif.com/) for full steps to configure and use ESP-IDF to build projects.

## Example Output

The example will output periodic sync and CTE-related events including:
- Extended scan start completion
- Periodic sync creation completion
- Periodic sync establishment
- Periodic advertising reports
- CTE parameter configuration completion
- CTE request enable completion
- IQ sample reports for direction finding

```
I (xxx) CTE_TEST: Extended scan started
I (xxx) CTE_TEST: Periodic sync created
I (xxx) CTE_TEST: Periodic sync established
I (xxx) CTE_TEST: Periodic adv report received
I (xxx) CTE_TEST: CTE set periodic adv receive params, status 0
I (xxx) CTE_TEST: CTE periodic adv request enable, status 0
I (xxx) CTE_TEST: CTE periodic IQ report received
...
```

## Troubleshooting

For any technical queries, please open an [issue](https://github.com/espressif/esp-idf/issues) on GitHub. We will get back to you soon.

