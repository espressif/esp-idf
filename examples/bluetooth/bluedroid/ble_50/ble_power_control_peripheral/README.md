| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 |
| ----------------- | -------- | -------- | --------- | -------- |

# BLE Power Control Peripheral Example

This example demonstrates how to use BLE Power Control feature as a Peripheral device using Bluedroid host API.

## What is BLE Power Control?

BLE Power Control is a BLE 5.0 feature that allows devices to monitor and control transmit power levels, and report path loss between connected devices. This helps optimize power consumption and maintain connection quality.

## How It Works

1. The Peripheral device starts advertising
2. When a Central connects, the Peripheral accepts the connection
3. After connection, the Peripheral reads remote transmit power level and enables power reporting
4. The Peripheral receives power events when conditions change

## API Usage

- `esp_ble_gap_read_remote_transmit_power_level()` - Read remote transmit power level
- `esp_ble_gap_set_transmit_power_reporting_enable()` - Enable transmit power reporting
- `ESP_GAP_BLE_TRANS_PWR_RPTING_EVT` - Event received when transmit power changes

## Configuration

The example requires BLE 5.0 features to be enabled:
- `CONFIG_BT_BLE_50_FEATURES_SUPPORTED=y`
- `CONFIG_BT_BLE_FEAT_POWER_CONTROL=y`

## Hardware Required

- One ESP32 development board (ESP32-C5, ESP32-C6, ESP32-H2, etc.) that supports BLE 5.0

## Build and Flash

```bash
idf.py set-target esp32c5
idf.py build flash monitor
```

## Usage

1. Flash this example to one ESP32 device (Peripheral)
2. Flash the `ble_power_control_central` example to another ESP32 device (Central)
3. The Peripheral will start advertising and wait for connection
4. Monitor the serial output to see power events

