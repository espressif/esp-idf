<!-- SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD -->
<!-- SPDX-License-Identifier: Apache-2.0 -->

# BLE UART Bridge

BLE UART Bridge is a host-side utility for talking to ESP-IDF applications that expose a BLE UART-style GATT service. It provides a reusable Python transport layer, an interactive console for manual testing, and a daemon mode for simple local IPC request/response workflows.

## Table of contents

- [Quick Start](#quick-start) - install dependencies and run the first commands
- [CLI overview](#cli-overview) - command list and common Console/Daemon workflows
  - [Typical Console workflow](#typical-console-workflow)
  - [Typical Daemon workflow](#typical-daemon-workflow)
  - [Custom scripts and porting](#custom-scripts-and-porting)
- [What is included](#what-is-included) - directory layout and component roles
  - [Core](#core)
  - [Console](#console)
  - [Daemon](#daemon)
- [Choosing Core, Console, or Daemon](#choosing-core-console-or-daemon)
- [Profile compatibility](#profile-compatibility)
- [Dependencies](#dependencies)
- [Limitations](#limitations)
- [Further reading](#further-reading)

## Quick Start

You can reuse the ESP-IDF Python environment, or use your own Python virtual environment. If you reuse the ESP-IDF environment, export it first and then install the additional BLE UART Bridge dependencies:

```bash
cd $IDF_PATH
. ./export.sh
cd tools/ble/ble_uart_bridge
python -m pip install -r requirements.txt
```

On Windows, run `export.bat` or `export.ps1` from the ESP-IDF root directory before installing `requirements.txt`. If you use your own Python virtual environment instead, activate it before running:

```bash
cd tools/ble/ble_uart_bridge
python -m pip install -r requirements.txt
```

List nearby BLE UART devices:

```bash
python main.py list-devices
```

Use the printed device identifier as `DEVICE_ID` in later commands. On macOS, this identifier is a CoreBluetooth UUID and is different from the device MAC address.

Check whether the device can be connected:

```bash
python main.py connection-check DEVICE_ID
```

Open an interactive BLE UART Console:

```bash
python main.py console DEVICE_ID
```

For Console options such as line endings, hex mode, and write-with-response, see [Quick-Start-BLE-UART-Console.md](docs/Quick-Start-BLE-UART-Console.md). If you need firmware to test against, use the [BLE UART Service example](../../../examples/bluetooth/ble_uart_service) as an Echo Server: it advertises the default Nordic UART Service profile and echoes RX writes back through TX notifications.

Run the BLE UART Daemon:

```bash
python main.py daemon DEVICE_ID
```

In another terminal, check daemon status and send a request:

```bash
python main.py daemon-status
python main.py daemon-send --op echo "hello"
python main.py daemon-notify --op set_led --json '{"state": true}'
```

For Daemon details, the HTTP API, and the JSONL RPC protocol, see [Quick-Start-BLE-UART-Daemon.md](docs/Quick-Start-BLE-UART-Daemon.md).

## CLI overview

Run:

```bash
python main.py --help
```

Available commands:

```bash
python main.py list-devices
python main.py connection-check DEVICE_ID
python main.py console DEVICE_ID
python main.py daemon DEVICE_ID
python main.py daemon-status
python main.py daemon-send DATA
python main.py daemon-notify DATA
```

### Typical Console workflow

Use Console when you want to manually test a BLE UART device from a terminal UI. For a known-compatible target, build and flash the [BLE UART Service example](../../../examples/bluetooth/ble_uart_service), which acts as an Echo Server for Console smoke tests:

```bash
python main.py list-devices
python main.py connection-check DEVICE_ID
python main.py console DEVICE_ID
```

Common Console variants:

```bash
# Use CRLF for AT-style commands
python main.py console DEVICE_ID --terminator crlf

# Send and display raw bytes in hex
python main.py console DEVICE_ID --encoding hex

# Use BLE write-with-response
python main.py console DEVICE_ID --with-response
```

For the full Console guide, see [Quick-Start-BLE-UART-Console.md](docs/Quick-Start-BLE-UART-Console.md).

### Typical Daemon workflow

Use Daemon when a local script, editor integration, or automation tool needs request/response access to a BLE UART device.

Terminal 1 starts the daemon and owns the BLE connection:

```bash
python main.py daemon DEVICE_ID
```

Terminal 2 checks status and sends requests through the daemon:

```bash
python main.py daemon-status
python main.py daemon-send --op echo "hello"
python main.py daemon-send --op set_led --json '{"state": true}'
python main.py daemon-notify --op set_led --json '{"state": true}'
```

For the HTTP API and JSONL RPC wire protocol, see [Quick-Start-BLE-UART-Daemon.md](docs/Quick-Start-BLE-UART-Daemon.md).

### Custom scripts and porting

Use the Core API directly when you want your own Python script to own the BLE connection, implement custom framing, or integrate BLE UART into a larger automation flow.

For examples using `BLEUARTBridge`, RX handlers, byte payloads, custom `BLEUARTProfile`, and custom request/response logic, see [PORTING.md](docs/PORTING.md).

## What is included

```text
tools/ble/ble_uart_bridge/
├── main.py
├── requirements.txt
├── README.md
├── docs/
│   ├── Quick-Start-BLE-UART-Console.md
│   ├── Quick-Start-BLE-UART-Daemon.md
│   ├── Profile-Compatibility.md
│   └── PORTING.md
└── src/
    ├── core/
    ├── console/
    └── daemon/
```

### Core

The Core component is the reusable BLE transport layer.

Use it when you want to write your own Python script or tool on top of BLE UART without reimplementing scanning, connection management, notification subscription, and chunked GATT writes.

Main responsibilities:

- Scan for BLE UART devices.
- Check whether a target device can be connected.
- Connect and disconnect with a BLE UART GATT profile.
- Subscribe to device-to-host notifications.
- Send host-to-device data as `str`, `bytes`, or `bytearray`.
- Support a default NUS profile and user-defined BLE UART profiles.

Important APIs:

- `BLEUARTBridge`
- `BLEUARTProfile`
- `run_list_devices()`
- `run_connection_check()`

Additional models are available from `src.core.models`, including `DeviceInfo` and `ConnectionState`.

### Console

The Console component is an interactive terminal UI for quick BLE UART testing.

Use it when you want to manually type data into a BLE UART device and observe received data without writing code.

Main responsibilities:

- Open an interactive Textual-based UI.
- Display TX, RX, and INFO logs separately.
- Send text lines with configurable line terminators.
- Send and display raw bytes in hex mode.
- Optionally use BLE write-with-response.
- Detect disconnects and show a notice in the UI.

### Daemon

The Daemon component exposes BLE UART as a local HTTP service.

Use it when another local tool, script, editor integration, or automation process needs request/response or fire-and-forget IPC with a BLE UART device.

Main responsibilities:

- Keep one BLE UART connection open in a background server process.
- Expose local HTTP endpoints for status, request/response, and fire-and-forget calls.
- Encode requests as newline-delimited JSON messages over BLE UART.
- Correlate device responses by request ID.
- Provide a small JSONL RPC-style envelope as an example protocol.

The daemon protocol is intentionally small. It is not a full RPC framework. It demonstrates a portable pattern that users can copy into firmware or extend in their own application protocol.

By default, the daemon binds to `127.0.0.1`. Keep it on a loopback address unless you add your own network access control, because the daemon exposes unauthenticated HTTP endpoints that can send data to the BLE device.

## Choosing Core, Console, or Daemon

| Component | Best for | Interface |
| --- | --- | --- |
| Core | Custom Python tools and scripts | Python API |
| Console | Manual BLE UART smoke tests | Interactive TUI |
| Daemon | Local IPC and automation | HTTP + CLI client |

Use Core when your business logic lives in Python. Use Console when you only need to manually test a BLE UART endpoint. Use Daemon when multiple local processes need to share one BLE connection through a simple request/response or notification boundary.

## Profile compatibility

The default profile is compatible with the Nordic UART Service (NUS):

- Service UUID: `6E400001-B5A3-F393-E0A9-E50E24DCCA9E`
- RX characteristic UUID, host to device: `6E400002-B5A3-F393-E0A9-E50E24DCCA9E`
- TX characteristic UUID, device to host: `6E400003-B5A3-F393-E0A9-E50E24DCCA9E`

For ESP-IDF BLE SPP examples and custom profile mapping details, see [Profile-Compatibility.md](docs/Profile-Compatibility.md).

## Dependencies

The tool depends on:

- `bleak` for BLE host access
- `textual` and `rich` for the console UI
- `fastapi` and `uvicorn` for daemon mode
- `typer` for the CLI
- `loguru` for logging

## Limitations

- Custom GATT UUIDs require constructing `BLEUARTProfile` in Python code.
- Daemon mode is single-flight for request/response calls: it processes one `/request` at a time. `/notify` sends without waiting for a device response.
- Daemon mode does not automatically reconnect after a BLE disconnect; restart the daemon after the device starts advertising again.
- Daemon `/request` and `/notify` limit `op` to 64 characters and JSON-encoded `data` to 4096 bytes.
- The JSONL RPC protocol is a demonstration envelope, not a complete RPC framework.
- Unmatched device messages are logged as unsolicited messages and are not exposed as a streaming API.
- The daemon request framing is newline-delimited JSON; device firmware must send a newline after every JSON response.

## Further reading

- [Quick-Start-BLE-UART-Console.md](docs/Quick-Start-BLE-UART-Console.md)
- [Quick-Start-BLE-UART-Daemon.md](docs/Quick-Start-BLE-UART-Daemon.md)
- [Profile-Compatibility.md](docs/Profile-Compatibility.md)
- [PORTING.md](docs/PORTING.md)
