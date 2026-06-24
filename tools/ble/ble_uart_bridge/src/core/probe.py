# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import asyncio

from bleak import BleakError
from bleak import BleakScanner
from loguru import logger

try:
    from bleak.exec import BleakBluetoothNotAvailableError
except ImportError:
    BleakBluetoothNotAvailableError = BleakError


async def has_accessible_bluetooth(timeout: float = 1.0) -> bool:
    scanner = BleakScanner()
    started = False
    try:
        await asyncio.wait_for(scanner.start(), timeout=timeout)
        started = True
        return True
    except (BleakBluetoothNotAvailableError, BleakError, asyncio.TimeoutError):
        logger.error('No available bluetooth host found on current machine!')
        return False
    finally:
        if started:
            try:
                await asyncio.wait_for(scanner.stop(), timeout=timeout)
            except Exception:
                pass
