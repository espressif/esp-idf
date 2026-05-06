# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import annotations

import asyncio

from bleak import BleakScanner
from bleak.backends.device import BLEDevice
from bleak.backends.scanner import AdvertisementData
from loguru import logger

from .constants import DEFAULT_SCAN_TIMEOUT
from .constants import NUS_SERVICE_UUID
from .models import DeviceInfo
from .probe import has_accessible_bluetooth


async def scan_devices(timeout: float = DEFAULT_SCAN_TIMEOUT, service_uuid: str = NUS_SERVICE_UUID) -> list[DeviceInfo]:
    """List available devices discovered by Bleak."""

    # Check if there's any available bluetooth device before scanning (Bleak limitation)
    if not await has_accessible_bluetooth():
        return []

    # Scan with callback
    devices_seen: dict[str, DeviceInfo] = {}

    def on_detect(device: BLEDevice, advertisement_data: AdvertisementData) -> None:
        # Gather device info
        device_id = device.address
        device_info = DeviceInfo(
            device_id=device_id,
            name=device.name,
            rssi=getattr(advertisement_data, 'rssi', getattr(device, 'rssi', None)),
        )

        # Update info if seen, add to seen if first seen
        if device_id not in devices_seen:
            logger.success(f'Found: {device_id}, with name {device_info.name}, rssi={device_info.rssi}')
        devices_seen[device_id] = device_info

    logger.info(f'Scanning for devices with BLE UART service in {timeout}s...')
    async with BleakScanner(detection_callback=on_detect, service_uuids=[service_uuid]):
        await asyncio.sleep(timeout)

    # Synthesis
    if devices_seen:
        logger.success(f'Found {len(devices_seen)} devices!')
        return [info for info in devices_seen.values()]
    else:
        logger.info('No device found!')
        return []
