| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 |
| ----------------- | -------- | -------- | --------- | -------- |

# BLE Power Control Central Example

This example demonstrates how to use BLE Power Control feature as a Central device using Bluedroid host API.

## What is BLE Power Control?

BLE Power Control is a BLE 5.0 feature that allows devices to monitor and control transmit power levels, and report path loss between connected devices. This helps optimize power consumption and maintain connection quality.

## How It Works

1. The Central device scans for BLE devices using extended scanning
2. When it finds the target device (Peripheral), it connects to it
3. After connection, the Central reads transmit power levels (local and remote)
4. The Central enables transmit power reporting and path loss reporting
5. The Central receives power and path loss events when conditions change

## API Usage

- `esp_ble_gap_enhanced_read_transmit_power_level()` - Read local transmit power levels
- `esp_ble_gap_read_remote_transmit_power_level()` - Read remote transmit power level
- `esp_ble_gap_set_transmit_power_reporting_enable()` - Enable transmit power reporting
- `esp_ble_gap_set_path_loss_reporting_params()` - Set path loss reporting parameters
- `esp_ble_gap_set_path_loss_reporting_enable()` - Enable path loss reporting
- `ESP_GAP_BLE_TRANS_PWR_RPTING_EVT` - Event received when transmit power changes
- `ESP_GAP_BLE_PATH_LOSS_THRESHOLD_EVT` - Event received when path loss crosses threshold

## Configuration

The example requires BLE 5.0 features to be enabled:
- `CONFIG_BT_BLE_50_FEATURES_SUPPORTED=y`
- `CONFIG_BT_BLE_50_EXTEND_SCAN_EN=y`
- `CONFIG_BT_BLE_FEAT_POWER_CONTROL=y`

## Hardware Required

- One ESP32 development board (ESP32-C5, ESP32-C6, ESP32-H2, etc.) that supports BLE 5.0

## Build and Flash

```bash
idf.py set-target esp32c5
idf.py build flash monitor
```

## Usage

1. Flash this example to one ESP32 device (Central)
2. Flash the `ble_power_control_peripheral` example to another ESP32 device (Peripheral)
3. The Central will automatically scan, connect, and monitor power levels
4. Monitor the serial output to see power and path loss events

