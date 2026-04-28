# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import asyncio

from .bridge import BLEUARTBridge
from .scanner import scan_devices


def run_list_devices() -> None:
    asyncio.run(scan_devices())


def run_connection_check(device_id: str) -> None:
    # Initialize BLE UART Bridge
    bridge = BLEUARTBridge(device_id)

    # Connection check
    async def connection_check() -> bool:
        try:
            return bool(await bridge.connect())
        finally:
            await bridge.disconnect()

    if not asyncio.run(connection_check()):
        raise SystemExit(1)
