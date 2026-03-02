| Supported Targets | ESP32-C5 | ESP32-C6 | ESP32-C61 | ESP32-H2 |
| ----------------- | -------- | -------- | --------- | -------- |

# BLE Connection Subrating Central Example

This example demonstrates how to use BLE Connection Subrating feature as a Central device using Bluedroid host API.

## What is BLE Connection Subrating?

BLE Connection Subrating is a BLE 5.0 feature that allows a Central or Peripheral to request a change to the subrating factor and/or other parameters applied to an existing connection. This can help optimize power consumption and connection intervals.

## How It Works

1. The Central device scans for BLE devices using extended scanning
2. When it finds the target device (Peripheral), it connects to it
3. After connection, the Central sets default subrate parameters
4. The Central requests a subrate change to reduce power consumption
5. The Central receives subrate change events when the subrating is updated

## API Usage

- `esp_ble_gap_set_default_subrate()` - Set default subrate parameters for future connections
- `esp_ble_gap_subrate_request()` - Request subrate change on an existing connection
- `ESP_GAP_BLE_SUBRATE_CHANGE_EVT` - Event received when subrate changes

## Configuration

The example requires BLE 5.0 features to be enabled:
- `CONFIG_BT_BLE_50_FEATURES_SUPPORTED=y`
- `CONFIG_BT_BLE_50_EXTEND_SCAN_EN=y`
- `CONFIG_BT_BLE_FEAT_CONN_SUBRATING=y`

## Hardware Required

- One ESP32 development board (ESP32-C5, ESP32-C6, ESP32-H2, etc.) that supports BLE 5.0

## Build and Flash

```bash
idf.py set-target esp32c5
idf.py build flash monitor
```

## Usage

1. Flash this example to one ESP32 device (Central)
2. Flash the `ble_conn_subrating_peripheral` example to another ESP32 device (Peripheral)
3. The Central will automatically scan, connect, and request subrate changes
4. Monitor the serial output to see subrate change events

