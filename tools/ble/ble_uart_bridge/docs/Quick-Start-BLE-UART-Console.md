<!-- SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD -->
<!-- SPDX-License-Identifier: Apache-2.0 -->

# Quick Start: BLE UART Console

This guide shows how to use the BLE UART Console for quick manual testing.

The Console is useful when you want to type data into a BLE UART device and inspect the bytes or text sent back by the device.

## Prerequisites

1. A host machine with Bluetooth access.
2. Python environment prepared. You can reuse the ESP-IDF Python environment, or use your own Python virtual environment. If you reuse the ESP-IDF environment, export it first and then install the BLE UART Bridge dependencies:

   ```bash
   cd $IDF_PATH
   . ./export.sh
   cd tools/ble/ble_uart_bridge
   python -m pip install -r requirements.txt
   ```

   On Windows, run `export.bat` or `export.ps1` from the ESP-IDF root directory before installing `requirements.txt`. If you use your own Python virtual environment instead, activate it before installing `requirements.txt`.

3. A BLE device advertising the BLE UART service. By default the tool scans for Nordic UART Service UUIDs. For a known-compatible test target, build and flash the [BLE UART Service example](../../../../examples/bluetooth/ble_uart_service), which acts as an Echo Server by echoing RX writes back through TX notifications.

## Find a device

```bash
cd tools/ble/ble_uart_bridge
python main.py list-devices
```

Example output may include a device address and name:

```text
Found: AA:BB:CC:DD:EE:FF, with name esp-ble-uart, rssi=-42
```

Use the printed device identifier as `DEVICE_ID`. On macOS, this identifier is a CoreBluetooth UUID and is different from the device MAC address.

## Check the connection

```bash
python main.py connection-check AA:BB:CC:DD:EE:FF
```

This command connects to the device, discovers the BLE UART service and characteristics, then disconnects.

## Start the Console

```bash
python main.py console AA:BB:CC:DD:EE:FF
```

The console connects before opening the UI. If connection fails, the UI is not started.

Inside the UI:

- Type a line and press Enter to send it.
- Received data is shown with an `[RX]` prefix.
- Transmitted data is shown with a `[TX]` prefix.
- Connection information is shown with an `[INFO]` prefix.
- Press `Ctrl+C` or `Ctrl+D` to quit.
- Press `Ctrl+L` to clear the log.

## Text mode

Text mode is the default. It UTF-8 encodes input and appends a line terminator.

```bash
python main.py console AA:BB:CC:DD:EE:FF
```

By default, each submitted line is sent with `\n`.

### Choose a line terminator

Use `--terminator` for protocols that expect different line endings:

```bash
python main.py console AA:BB:CC:DD:EE:FF --terminator lf
python main.py console AA:BB:CC:DD:EE:FF --terminator crlf
python main.py console AA:BB:CC:DD:EE:FF --terminator none
```

Supported values:

| Value | Bytes appended |
| --- | --- |
| `lf` | `\n` |
| `crlf` | `\r\n` |
| `none` | nothing |

Use `crlf` for many AT-style command interpreters. Use `none` if the device expects the exact bytes you type.

## Hex mode

Hex mode sends raw bytes parsed from hexadecimal input and displays received bytes as hexadecimal.

```bash
python main.py console AA:BB:CC:DD:EE:FF --encoding hex
```

Inside the UI, enter bytes as hex:

```text
01 02 03 0a
```

The console sends:

```text
0x01 0x02 0x03 0x0a
```

Notes:

- Hex input is parsed with Python `bytes.fromhex()`.
- Spaces are allowed.
- In hex mode, `--terminator` is ignored because the input already represents exact bytes.

## Write-with-response

By default the console writes without response. Use `--with-response` if the target characteristic or debugging workflow should use BLE write-with-response:

```bash
python main.py console AA:BB:CC:DD:EE:FF --with-response
```

This affects BLE GATT write behavior only. It does not create an application-level request/response protocol. For application-level request/response, use Daemon mode instead.

## Common examples

### ESP-IDF BLE UART Echo Server

Use the [BLE UART Service example](../../../../examples/bluetooth/ble_uart_service) when you want a ready-made ESP-IDF Echo Server for testing BLE UART Bridge Console. After building, flashing, and pairing with the example, open Console and type any text; the example should echo the same data back as `[RX]` output.

```bash
# List nearby BLE UART devices and use the printed device ID as DEVICE_ID
python main.py list-devices
python main.py console AA:BB:CC:DD:EE:FF
```

### ESP-IDF console-style command

```bash
python main.py console AA:BB:CC:DD:EE:FF --terminator lf
```

Then type:

```text
help
```

### AT-style command

```bash
python main.py console AA:BB:CC:DD:EE:FF --terminator crlf
```

Then type:

```text
AT
```

### Binary smoke test

```bash
python main.py console AA:BB:CC:DD:EE:FF --encoding hex --with-response
```

Then type:

```text
aa 55 01 00
```

## Troubleshooting

### No devices found

- Confirm the host Bluetooth adapter is available.
- Confirm the device is advertising the BLE UART service UUID.
- Move the device closer to the host.

### Connection fails

- Make sure no other host is already connected to the BLE device.
- Restart advertising on the device.
- Run `connection-check` before opening the console.

### Text looks broken

- The console decodes RX bytes as UTF-8 in text mode.
- Use `--encoding hex` if the device sends binary data.

### Device does not react to input

- Check the required line ending. Try `--terminator crlf` or `--terminator none`.
- Check whether the device requires write-with-response. Try `--with-response`.
