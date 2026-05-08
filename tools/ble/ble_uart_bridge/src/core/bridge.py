# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import annotations

import asyncio
from collections.abc import Callable

from bleak import BleakClient
from bleak.backends.characteristic import BleakGATTCharacteristic
from bleak.backends.service import BleakGATTService
from bleak.backends.service import BleakGATTServiceCollection
from loguru import logger

from .constants import BLE_WRITE_WITH_RESPONSE_MAX_SIZE
from .constants import DEFAULT_CONNECTION_TIMEOUT
from .constants import DEFAULT_DISCONNECTION_TIMEOUT
from .constants import DEFAULT_WRITE_CHUNK_TIMEOUT
from .errors import ConnectionTimeout
from .errors import ServiceDiscoveryError
from .models import BLEUARTProfile
from .models import ConnectionState


class BLEUARTBridge:
    def __init__(
        self,
        device_id: str,
        connection_timeout: float = DEFAULT_CONNECTION_TIMEOUT,
        profile: BLEUARTProfile | None = None,
    ) -> None:
        # Bridge initialization
        self._device_id = device_id
        self._profile = profile or BLEUARTProfile()
        self._conn_state = ConnectionState.DISCONNECTED
        self._connection_timeout = connection_timeout
        self._client = BleakClient(
            self._device_id,
            disconnected_callback=self._handle_disconnect,
            services=[self._profile.service_uuid],
            timeout=connection_timeout,
        )
        self._tx_char: BleakGATTCharacteristic | None = None
        self._rx_char: BleakGATTCharacteristic | None = None
        self._disconnected_event = asyncio.Event()
        self._state_lock = asyncio.Lock()
        self._send_lock = asyncio.Lock()
        self._rx_handlers: list[Callable[[bytearray], None]] = []

    # Connection
    @property
    def connection_state(self) -> ConnectionState:
        return self._conn_state

    @property
    def profile(self) -> BLEUARTProfile:
        return self._profile

    @property
    def is_connected(self) -> bool:
        return self._conn_state is ConnectionState.CONNECTED and self._client.is_connected

    def reset(self) -> None:
        self._conn_state = ConnectionState.DISCONNECTED
        self._tx_char = None
        self._rx_char = None

    async def disconnect(self) -> None:
        async with self._state_lock:
            await self._disconnect_locked()

    # Should be called while holding _state_lock.
    async def _disconnect_locked(self) -> None:
        # Early return if device is already disconnected
        if not self._client.is_connected:
            logger.warning(f'{self._device_id} is already disconnected')
            self.reset()
            return

        # Disconnect from device and reset bridge
        logger.info(f'Disconnecting from {self._device_id}...')
        self._disconnected_event.clear()
        try:
            await asyncio.wait_for(self._client.disconnect(), timeout=DEFAULT_DISCONNECTION_TIMEOUT)
        except asyncio.TimeoutError:
            logger.warning(f'Failed to request disconnect from {self._device_id} in {DEFAULT_DISCONNECTION_TIMEOUT}s!')
            self.reset()
            return
        except Exception as e:
            logger.exception(e)
            self.reset()
            return

        # Wait for _handle_disconnect callback to execute
        try:
            await asyncio.wait_for(self._disconnected_event.wait(), timeout=DEFAULT_DISCONNECTION_TIMEOUT)
        except asyncio.TimeoutError:
            logger.warning(f'Failed to disconnect from {self._device_id} in {DEFAULT_DISCONNECTION_TIMEOUT}s!')
            self.reset()

    async def _cleanup_connection_locked(self) -> None:
        if self._client.is_connected:
            await self._disconnect_locked()
        else:
            self.reset()

    async def _cleanup_connection(self) -> None:
        async with self._state_lock:
            await self._cleanup_connection_locked()

    def _handle_disconnect(self, _: BleakClient) -> None:
        logger.info(f'Disconnected from {self._device_id}')
        self.reset()
        self._disconnected_event.set()

    async def connect(self) -> bool:
        async with self._state_lock:
            # Early return for connected state
            if self._conn_state is ConnectionState.CONNECTED:
                if self._client.is_connected:
                    logger.warning(f'Connection state: {self._conn_state.value}, connect attempt ignored!')
                    return True
                self.reset()
            if self._conn_state is not ConnectionState.DISCONNECTED:
                logger.warning(f'Connection state: {self._conn_state.value}, connect attempt ignored!')
                return False

            # Try to connect to the device
            try:
                # Update connection state
                logger.info(f'Connecting to {self._device_id}...')
                self._conn_state = ConnectionState.CONNECTING

                await self._client.connect()
                if not self._client.is_connected:
                    raise ConnectionTimeout(
                        f'Failed to connect to device {self._device_id} in {self._connection_timeout}s.'
                    )
            except asyncio.CancelledError:
                await self._cleanup_connection_locked()
                raise
            except Exception as e:
                logger.exception(e)
                await self._cleanup_connection_locked()
                return False

            # Try to discover service
            try:
                services: BleakGATTServiceCollection = self._client.services
                nus_service: BleakGATTService | None = services.get_service(self._profile.service_uuid)
                rx_char: BleakGATTCharacteristic | None = services.get_characteristic(self._profile.rx_char_uuid)
                tx_char: BleakGATTCharacteristic | None = services.get_characteristic(self._profile.tx_char_uuid)

                if not nus_service or not rx_char or not tx_char:
                    raise ServiceDiscoveryError(
                        f'Device {self._device_id} is missing the required service or characteristics.'
                    )
            except asyncio.CancelledError:
                await self._cleanup_connection_locked()
                raise
            except Exception as e:
                logger.exception(e)
                await self._cleanup_connection_locked()
                return False

            # Save characteristic reference
            # Note: Tx char -> Device to Host, rx char -> Host to Device
            self._rx_char = rx_char
            self._tx_char = tx_char

            # Try to start device notification
            try:
                await self._client.start_notify(tx_char, self._handle_notify)
                if not self._client.is_connected:
                    self.reset()
                    return False
            except asyncio.CancelledError:
                await self._cleanup_connection_locked()
                raise
            except Exception as e:
                logger.exception(e)
                await self._cleanup_connection_locked()
                return False

            # Connection not considered to be established until notification start succeeded
            self._conn_state = ConnectionState.CONNECTED
            logger.success(f'Succeeded to connect to {self._device_id}!')
            return True

    # Receive
    def add_rx_handler(self, handler: Callable[[bytearray], None]) -> None:
        self._rx_handlers.append(handler)

    def remove_rx_handler(self, handler: Callable[[bytearray], None]) -> None:
        self._rx_handlers.remove(handler)

    def _handle_notify(self, _: BleakGATTCharacteristic, data: bytearray) -> None:
        # Fallback to debug logging if user has not registered receive handler
        if not self._rx_handlers:
            logger.debug(data.decode(errors='replace'))
            return
        for handler in self._rx_handlers:
            handler(bytearray(data))

    @staticmethod
    def _get_write_chunk_size(rx_char: BleakGATTCharacteristic, with_response: bool) -> int:
        if with_response:
            return int(BLE_WRITE_WITH_RESPONSE_MAX_SIZE)
        return max(1, int(rx_char.max_write_without_response_size))

    # Transmit
    async def send(self, data: str | bytes | bytearray, with_response: bool = False) -> bool:
        async with self._send_lock:
            # Unwritable status check
            if not self.is_connected:
                if self._conn_state is ConnectionState.CONNECTED:
                    self.reset()
                logger.error(f'Not connected to {self._device_id} yet!')
                return False
            if self._rx_char is None:
                logger.error(f'Rx characteristic not available on {self._device_id}')
                return False
            rx_char = self._rx_char
            chunk_size = self._get_write_chunk_size(rx_char, with_response)
            payload = data.encode() if isinstance(data, str) else bytes(data)

            # Write in try-except to catch write failure
            try:
                for offset in range(0, len(payload), chunk_size):
                    chunk = payload[offset : offset + chunk_size]
                    await asyncio.wait_for(
                        self._client.write_gatt_char(rx_char, chunk, response=with_response),
                        timeout=DEFAULT_WRITE_CHUNK_TIMEOUT,
                    )
            except asyncio.CancelledError:
                await self._cleanup_connection()
                raise
            except asyncio.TimeoutError:
                logger.error(f'Failed to write chunk to {self._device_id} in {DEFAULT_WRITE_CHUNK_TIMEOUT}s!')
                await self._cleanup_connection()
                return False
            except Exception as e:
                logger.exception(e)
                await self._cleanup_connection()
                return False
            return True
