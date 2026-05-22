<!-- SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD -->
<!-- SPDX-License-Identifier: Apache-2.0 -->

# Porting BLE UART Bridge to Custom Scripts

This guide explains how to reuse BLE UART Bridge in your own Python scripts.

Use the Core API when the Console and Daemon are not the right abstraction for your application. For example, use Core directly when you want to implement custom framing, a test harness, a device provisioning flow, or a domain-specific automation script.

## Choose the right integration level

| Need | Recommended integration |
| --- | --- |
| Manual testing | Use `python main.py console DEVICE_ID` |
| Local process talks to a BLE device through HTTP | Use Daemon mode |
| Custom Python logic owns the BLE connection | Use `BLEUARTBridge` directly |
| Custom service UUIDs or characteristics | Use `BLEUARTProfile` with `BLEUARTBridge` |

## Install dependencies

You can reuse the ESP-IDF Python environment, or use your own Python virtual environment. If you reuse the ESP-IDF environment, export it first and then install the extra dependencies required by BLE UART Bridge:

```bash
cd $IDF_PATH
. ./export.sh
cd tools/ble/ble_uart_bridge
python -m pip install -r requirements.txt
```

On Windows, run `export.bat` or `export.ps1` from the ESP-IDF root directory before installing `requirements.txt`. If you use your own Python virtual environment instead, activate it before installing `requirements.txt`.

When importing from a script outside this directory, make sure `tools/ble/ble_uart_bridge` is on `PYTHONPATH`, or run the script from this directory.

Example:

```bash
PYTHONPATH=tools/ble/ble_uart_bridge python my_script.py
```

## Basic script

The simplest script connects, sends one line, and disconnects:

```python
import asyncio

from src.core import BLEUARTBridge


async def main() -> None:
    bridge = BLEUARTBridge("AA:BB:CC:DD:EE:FF")

    try:
        if not await bridge.connect():
            raise RuntimeError("failed to connect")

        await bridge.send("hello\n")
    finally:
        await bridge.disconnect()


asyncio.run(main())
```

## Receive data with handlers

Register one or more RX handlers before connecting:

```python
import asyncio

from src.core import BLEUARTBridge


def print_rx(data: bytearray) -> None:
    print("RX:", data.decode(errors="replace"))


async def main() -> None:
    bridge = BLEUARTBridge("AA:BB:CC:DD:EE:FF")
    bridge.add_rx_handler(print_rx)

    try:
        if not await bridge.connect():
            raise RuntimeError("failed to connect")

        await bridge.send("help\n")
        await asyncio.sleep(2)
    finally:
        await bridge.disconnect()


asyncio.run(main())
```

Handlers are synchronous callables. If your application needs async processing, push received data into an `asyncio.Queue`:

```python
import asyncio

from src.core import BLEUARTBridge


async def main() -> None:
    bridge = BLEUARTBridge("AA:BB:CC:DD:EE:FF")
    rx_queue: asyncio.Queue[bytes] = asyncio.Queue()

    def enqueue_rx(data: bytearray) -> None:
        rx_queue.put_nowait(bytes(data))

    bridge.add_rx_handler(enqueue_rx)

    try:
        if not await bridge.connect():
            raise RuntimeError("failed to connect")

        await bridge.send("status\n")
        data = await asyncio.wait_for(rx_queue.get(), timeout=5.0)
        print("RX:", data)
    finally:
        await bridge.disconnect()


asyncio.run(main())
```

## Send bytes instead of text

`BLEUARTBridge.send()` accepts `str`, `bytes`, and `bytearray`.

```python
await bridge.send(b"\x01\x02\x03\x0a")
await bridge.send(bytearray([0x01, 0x02, 0x03, 0x0A]))
```

Use `with_response=True` when the target characteristic or debugging workflow should use BLE write-with-response:

```python
await bridge.send(b"\x01\x02", with_response=True)
```

## Use a custom BLE UART profile

The default profile uses Nordic UART Service UUIDs. For custom firmware, create a `BLEUARTProfile`:

```python
from src.core import BLEUARTBridge
from src.core import BLEUARTProfile


profile = BLEUARTProfile(
    service_uuid="00000000-0000-0000-0000-000000000001",
    rx_char_uuid="00000000-0000-0000-0000-000000000002",
    tx_char_uuid="00000000-0000-0000-0000-000000000003",
)

bridge = BLEUARTBridge("AA:BB:CC:DD:EE:FF", profile=profile)
```

The naming follows BLE UART convention:

- RX characteristic: host writes to device.
- TX characteristic: device notifies host.

## Implement your own request/response protocol

If your script needs request/response semantics, use a queue or future map and correlate responses at the application layer.

The daemon uses a lightweight JSONL envelope. You can reuse the same pattern:

```python
import asyncio
import json
from uuid import uuid4

from src.core import BLEUARTBridge


async def main() -> None:
    bridge = BLEUARTBridge("AA:BB:CC:DD:EE:FF")
    rx_buffer = bytearray()
    pending: dict[str, asyncio.Future[object]] = {}

    def handle_rx(data: bytearray) -> None:
        rx_buffer.extend(data)
        while b"\n" in rx_buffer:
            index = rx_buffer.index(b"\n")
            line = bytes(rx_buffer[:index])
            del rx_buffer[: index + 1]

            message = json.loads(line.decode())
            request_id = message.get("id")
            future = pending.get(request_id)
            if future is None or future.done():
                continue

            if message.get("ok") is False:
                future.set_exception(RuntimeError(str(message.get("error"))))
            else:
                future.set_result(message.get("data"))

    bridge.add_rx_handler(handle_rx)

    try:
        if not await bridge.connect():
            raise RuntimeError("failed to connect")

        request_id = uuid4().hex
        loop = asyncio.get_running_loop()
        pending[request_id] = loop.create_future()

        request = {"v": 1, "id": request_id, "op": "echo", "data": "hello"}
        await bridge.send(json.dumps(request) + "\n", with_response=True)

        response = await asyncio.wait_for(pending[request_id], timeout=10.0)
        print(response)
    finally:
        await bridge.disconnect()


asyncio.run(main())
```

For production scripts, add validation around incoming JSON and clean up `pending` entries on timeout.

## Scan for devices from Python

Use `scan_devices()` if your script needs to discover devices first:

```python
import asyncio

from src.core.scanner import scan_devices


async def main() -> None:
    devices = await scan_devices(timeout=5.0)
    for device in devices:
        print(device.device_id, device.name, device.rssi)


asyncio.run(main())
```

For custom service UUID discovery:

```python
devices = await scan_devices(
    timeout=5.0,
    service_uuid="00000000-0000-0000-0000-000000000001",
)
```

## Error handling guidance

The current Core API returns `False` for connection or send failures and logs details through `loguru`.

Recommended script pattern:

```python
if not await bridge.connect():
    raise RuntimeError("failed to connect to BLE UART device")

if not await bridge.send("hello\n"):
    raise RuntimeError("failed to send BLE UART data")
```

Always disconnect in `finally`:

```python
try:
    ...
finally:
    await bridge.disconnect()
```

## Porting checklist

- [ ] Decide whether your use case needs Console, Daemon, or Core.
- [ ] Confirm the BLE service and characteristic UUIDs.
- [ ] Decide whether your payload is text, binary, JSON, or another framing format.
- [ ] Register RX handlers before calling `connect()`.
- [ ] Add a newline delimiter if your protocol is JSONL or line-oriented.
- [ ] Use `with_response=True` only when needed.
- [ ] Clean up pending request state on timeout.
- [ ] Call `disconnect()` in `finally`.
