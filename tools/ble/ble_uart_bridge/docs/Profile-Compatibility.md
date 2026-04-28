<!-- SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD -->
<!-- SPDX-License-Identifier: Apache-2.0 -->

# BLE UART Profile Compatibility

BLE UART Bridge works with BLE GATT profiles that provide a UART-like data path:

- one characteristic that the host writes to
- one characteristic that the device uses to notify data back to the host

The default profile is compatible with the Nordic UART Service (NUS), but NUS is not the only possible BLE UART-style profile.

## Default NUS-compatible profile

The built-in default profile uses these UUIDs:

| Role | UUID |
| --- | --- |
| Service | `6E400001-B5A3-F393-E0A9-E50E24DCCA9E` |
| RX, host to device | `6E400002-B5A3-F393-E0A9-E50E24DCCA9E` |
| TX, device to host | `6E400003-B5A3-F393-E0A9-E50E24DCCA9E` |

Use the default profile when the device advertises a NUS-compatible service.

## ESP-IDF BLE SPP examples

ESP-IDF includes BLE SPP examples that implement Espressif BLE UART-like vendor-specific GATT profiles:

- `examples/bluetooth/nimble/ble_spp/spp_server`
- `examples/bluetooth/nimble/ble_spp/spp_client`
- `examples/bluetooth/bluedroid/ble/ble_spp_server`
- `examples/bluetooth/bluedroid/ble/ble_spp_client`

BLE SPP over BLE is not a Bluetooth SIG standard profile. It is a vendor-specific GATT design that emulates a serial link, similar in purpose to NUS.

ESP-IDF BLE SPP examples may define more characteristics than BLE UART Bridge needs, such as data, command, and status characteristics. To use BLE UART Bridge with such a profile, map only the UART-like data path into `BLEUARTProfile`.

## Mapping an ESP-IDF BLE SPP profile

Map the profile fields as follows:

| `BLEUARTProfile` field | Map to |
| --- | --- |
| `service_uuid` | BLE SPP service UUID |
| `rx_char_uuid` | Characteristic that the host writes to, such as the SPP data receive characteristic |
| `tx_char_uuid` | Characteristic that the device notifies from, such as the SPP data notify characteristic |

Example:

```python
from src.core import BLEUARTBridge
from src.core import BLEUARTProfile


profile = BLEUARTProfile(
    service_uuid="00000000-0000-0000-0000-00000000ABF0",
    rx_char_uuid="00000000-0000-0000-0000-00000000ABF1",
    tx_char_uuid="00000000-0000-0000-0000-00000000ABF2",
)

bridge = BLEUARTBridge("AA:BB:CC:DD:EE:FF", profile=profile)
```

Replace the UUIDs with the actual UUIDs used by the device firmware.

## What BLE UART Bridge does not map

BLE UART Bridge is intentionally focused on the data path. It does not automatically map extra control-plane characteristics that a profile may expose, such as:

- command characteristics
- status characteristics
- custom configuration characteristics
- profile-specific flow-control semantics

If an application needs those characteristics, implement that logic in a custom script on top of `bleak`, or extend BLE UART Bridge for that specific profile.

## Classic Bluetooth SPP is different

Classic Bluetooth SPP examples, such as `examples/bluetooth/bluedroid/classic_bt/bt_spp_*`, are not BLE GATT profiles.

They use Classic Bluetooth SPP rather than BLE GATT characteristics, so they are not compatible with BLE UART Bridge.

## Related docs

- [README.md](../README.md)
- [PORTING.md](PORTING.md)
