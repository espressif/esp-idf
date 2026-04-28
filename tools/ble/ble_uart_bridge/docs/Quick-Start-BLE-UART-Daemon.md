<!-- SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD -->
<!-- SPDX-License-Identifier: Apache-2.0 -->

# Quick Start: BLE UART Daemon

This guide shows how to use BLE UART Daemon mode and the lightweight JSONL RPC protocol used between the host and the BLE device.

Daemon mode is useful when another local process needs to communicate with a BLE UART device without owning the BLE connection itself.

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

3. A BLE UART device that understands the daemon JSONL request/response protocol, or a device implementation you can adapt.

## Start the daemon

First, scan for a device:

```bash
cd tools/ble/ble_uart_bridge
python main.py list-devices
```

Then start the daemon:

```bash
python main.py daemon AA:BB:CC:DD:EE:FF
```

By default, the daemon listens on `127.0.0.1:8888`.

To choose another host or port:

```bash
python main.py daemon AA:BB:CC:DD:EE:FF --host 127.0.0.1 --port 8899
```

The daemon keeps one BLE connection open until it is stopped.

Security note: the daemon HTTP API does not implement authentication or authorization. Keep `--host` on `127.0.0.1` for local-only access unless you place the daemon behind your own access control.

## Check daemon status

In another terminal:

```bash
python main.py daemon-status
```

Example response:

```json
{
  "device_id": "AA:BB:CC:DD:EE:FF",
  "connection_state": "CONNECTED",
  "is_connected": true,
  "pending_requests": 0,
  "single_flight": true,
  "max_request_data_bytes": 4096,
  "protocol": "esp-jsonl-rpc-lite-v1"
}
```

If the daemon uses a non-default address:

```bash
python main.py daemon-status --host 127.0.0.1 --port 8899
```

## Send a request from the CLI

Send a raw string payload with the default operation name `raw`:

```bash
python main.py daemon-send "hello"
```

Send a request with an explicit operation name:

```bash
python main.py daemon-send --op echo "hello"
```

Send a JSON payload:

```bash
python main.py daemon-send --op set_led --json '{"state": true}'
```

Set the request timeout:

```bash
python main.py daemon-send --op echo --timeout 5.0 "hello"
```

Use a non-default daemon address:

```bash
python main.py daemon-send --host 127.0.0.1 --port 8899 --op echo "hello"
```

Do not send requests to a daemon bound to a shared network interface unless that network path is trusted or protected by your own access control.

The CLI prints only the response payload. If the device returns a JSON object, the CLI prints it as JSON.

## Send a notification from the CLI

Use `daemon-notify` for fire-and-forget operations where the caller only needs the daemon to write to the BLE device and does not need a protocol response:

```bash
python main.py daemon-notify --op set_led --json '{"state": true}'
```

Send a raw string notification with the default operation name `raw`:

```bash
python main.py daemon-notify "hello"
```

Use a non-default daemon address:

```bash
python main.py daemon-notify --host 127.0.0.1 --port 8899 --op set_led --json '{"state": true}'
```

`daemon-notify` returns after the local BLE write completes. It does not wait for the device to send a JSONL response.

## HTTP API

Daemon mode exposes a local HTTP API.

### `GET /status`

Returns daemon and BLE connection state:

```bash
curl http://127.0.0.1:8888/status
```

Response fields:

| Field | Meaning |
| --- | --- |
| `device_id` | BLE device ID used by the daemon |
| `connection_state` | Bridge connection state |
| `is_connected` | Whether the BLE client is currently connected |
| `pending_requests` | Number of pending request futures |
| `single_flight` | Whether the daemon serializes requests |
| `max_request_data_bytes` | Maximum JSON-encoded `data` size accepted by `/request` and `/notify` |
| `protocol` | Wire protocol name and version |

### `POST /request`

Sends one request to the BLE device and waits for the response:

```bash
curl -X POST http://127.0.0.1:8888/request \
  -H 'Content-Type: application/json' \
  -d '{"op":"echo","data":"hello","timeout":10}'
```

Request body:

```json
{
  "op": "echo",
  "data": "hello",
  "timeout": 10.0
}
```

Fields:

| Field | Required | Meaning |
| --- | --- | --- |
| `op` | No | Operation name. Defaults to `raw`. |
| `data` | Yes | Request payload. Can be a string, number, boolean, array, object, or null. |
| `timeout` | No | Response timeout in seconds. Defaults to `10.0`. |

Limits:

- `op` must be 1 to 64 characters.
- JSON-encoded `data` must not exceed 4096 bytes.

Successful response:

```json
{
  "ok": true,
  "data": "hello"
}
```

HTTP error behavior:

| HTTP status | Meaning |
| --- | --- |
| `413` | Request data exceeds the daemon payload limit |
| `500` | Failed to send data to the BLE device |
| `502` | Device returned a protocol error or invalid response |
| `504` | Timed out waiting for the device response |

### `POST /notify`

Sends one notification to the BLE device and returns without waiting for a protocol response:

```bash
curl -X POST http://127.0.0.1:8888/notify \
  -H 'Content-Type: application/json' \
  -d '{"op":"set_led","data":{"state":true}}'
```

Request body:

```json
{
  "op": "set_led",
  "data": {
    "state": true
  }
}
```

Fields:

| Field | Required | Meaning |
| --- | --- | --- |
| `op` | No | Operation name. Defaults to `raw`. |
| `data` | Yes | Notification payload. Can be a string, number, boolean, array, object, or null. |

Limits:

- `op` must be 1 to 64 characters.
- JSON-encoded `data` must not exceed 4096 bytes.

Successful response:

```json
{
  "ok": true
}
```

HTTP error behavior:

| HTTP status | Meaning |
| --- | --- |
| `413` | Request data exceeds the daemon payload limit |
| `500` | Failed to send data to the BLE device |

## BLE JSONL RPC protocol

The daemon communicates with the BLE device using newline-delimited JSON. Every message is one JSON object followed by `\n`.

The protocol is named:

```text
esp-jsonl-rpc-lite-v1
```

It is intentionally small:

- Human-readable during debugging.
- Easy to generate and parse on ESP-IDF firmware with `cJSON`.
- No schema registry or capability negotiation.
- No built-in routing framework.
- One request at a time in the current daemon implementation.

### Host to device request

The daemon sends this JSONL message to the BLE device:

```json
{"v":1,"id":"6f8f...","op":"echo","data":"hello"}
```

Actual wire bytes include a final newline:

```text
{"v":1,"id":"6f8f...","op":"echo","data":"hello"}\n
```

Fields:

| Field | Meaning |
| --- | --- |
| `v` | Protocol version. Current value is `1`. |
| `id` | Request ID generated by the daemon. The device must echo this in `/request` responses. `/notify` uses an empty string because no response is expected. |
| `op` | Operation name selected by the client. |
| `data` | Request payload. |

### Device to host success response

```json
{"v":1,"id":"6f8f...","ok":true,"data":"hello"}
```

Fields:

| Field | Meaning |
| --- | --- |
| `v` | Protocol version. Recommended value is `1`. |
| `id` | The request ID from the host message. |
| `ok` | `true` for success. |
| `data` | Response payload. |

The daemon requires `data` to be present when `ok` is `true`.

### Device to host error response

```json
{"v":1,"id":"6f8f...","ok":false,"error":"unsupported op"}
```

Fields:

| Field | Meaning |
| --- | --- |
| `id` | The request ID from the host message. |
| `ok` | `false` for error. |
| `error` | Human-readable error message. |

The daemon requires `error` to be a non-empty string when `ok` is `false`.

### Response validation rules

For the preferred `ok/data/error` format, the daemon validates these rules:

- `id` must be a string and must match a pending request.
- If present, `v` must be `1`.
- `ok` must be a boolean.
- `ok: true` requires a `data` field.
- `ok: false` requires a non-empty string `error` field.

Messages without a matching pending `id` are treated as unsolicited messages and are logged only.

### Legacy response compatibility

The daemon also accepts older response shapes:

```json
{"id":"6f8f...","response":"hello"}
{"id":"6f8f...","error":"failed"}
```

New device firmware should prefer the `ok/data/error` format.

## Minimal firmware-side behavior

On the BLE device, implement this loop conceptually:

1. Accumulate bytes received on the BLE UART RX characteristic.
2. Split input on `\n`.
3. Parse each line as JSON.
4. Read `id`, `op`, and `data`.
5. Execute the requested operation.
6. If `id` is non-empty, send a JSON response with the same `id` and a final `\n`.
7. If `id` is empty, treat the message as fire-and-forget and normally do not send a response.

For example, an `echo` operation can return the same data:

```json
{"v":1,"id":"6f8f...","ok":true,"data":"hello"}
```

For notifications sent through `/notify`, the daemon uses an empty `id`:

```json
{"v":1,"id":"","op":"set_led","data":{"state":true}}
```

Firmware can execute the operation without responding. If it does respond with `id: ""`, the daemon will log the message as unsolicited because no pending request is waiting for that ID.

## Single-flight behavior

The daemon currently processes one `/request` at a time. This is exposed as:

```json
"single_flight": true
```

This keeps the firmware-side example simple because the device only needs to handle one active request at a time. The request `id` is still included so the protocol can be extended later if concurrent requests are needed.

## Disconnect behavior

The daemon does not automatically reconnect after the BLE link is disconnected. If the device disconnects, stop and restart the daemon after the device starts advertising again.

## Troubleshooting

### `daemon-send` times out

- Confirm the device sends a newline after the JSON response.
- Confirm the device response contains the same `id` as the request.
- Confirm the firmware handles the requested `op`.
- Increase `--timeout` if the operation is slow.
- Restart the daemon if the BLE link was disconnected.

### Daemon returns HTTP 502

- The device returned an error response, or the response was missing required fields.
- Check daemon logs for the exact error.

### Device receives data but daemon never resolves the request

- Check that the response is valid JSON.
- Check that the response is an object, not a JSON array or string.
- Check that the response is newline terminated.
- Check that the response `id` matches the request `id` exactly.
