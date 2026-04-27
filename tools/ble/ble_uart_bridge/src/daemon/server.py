# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import asyncio
import json
from collections.abc import AsyncIterator
from contextlib import asynccontextmanager
from uuid import uuid4

from fastapi import FastAPI
from fastapi import HTTPException
from loguru import logger

from ..core import BLEUARTBridge
from .jsonl import PROTOCOL_VERSION
from .jsonl import drain_jsonl_messages
from .jsonl import encode_jsonl_request
from .jsonl import resolve_pending_response
from .models import MAX_REQUEST_DATA_BYTES
from .models import BLEUARTRequestPayload


@asynccontextmanager
async def lifespan(app: FastAPI) -> AsyncIterator[None]:
    # Server initialization
    app.state.bridge = BLEUARTBridge(app.state.device_id)
    app.state.request_lock = asyncio.Lock()

    # Set BLE UART Bridge RX callback
    loop = asyncio.get_running_loop()
    app.state.rx_buffer = bytearray()
    app.state.pending_requests = {}

    def _handle_rx_data(data: bytes) -> None:
        for message in drain_jsonl_messages(app.state.rx_buffer, data):
            if resolve_pending_response(app.state.pending_requests, message):
                continue
            logger.debug(f'Received unsolicited BLE UART message: {message!r}')

    def _rx_handler(data: bytearray) -> None:
        try:
            loop.call_soon_threadsafe(_handle_rx_data, bytes(data))
        except RuntimeError:
            logger.warning(f'Event loop is unavailable, dropping data: {data.decode(errors="replace")}')

    app.state.bridge.add_rx_handler(_rx_handler)

    # Try to connect to the device
    if not await app.state.bridge.connect():
        logger.error('Failed to start BLE UART Daemon!')
        raise RuntimeError('Failed to start BLE UART Daemon!')

    yield

    # Disconnect from the device
    await app.state.bridge.disconnect()


app = FastAPI(title='BLE UART Daemon', lifespan=lifespan)


def _request_data_size(data: object) -> int:
    return len(json.dumps(data).encode())


@app.get('/status')
async def status() -> dict:
    bridge: BLEUARTBridge | None = getattr(app.state, 'bridge', None)
    pending_requests: dict | None = getattr(app.state, 'pending_requests', None)

    return {
        'device_id': getattr(app.state, 'device_id', None),
        'connection_state': bridge.connection_state.value if bridge else 'DISCONNECTED',
        'is_connected': bridge.is_connected if bridge else False,
        'pending_requests': len(pending_requests or {}),
        'single_flight': True,
        'max_request_data_bytes': MAX_REQUEST_DATA_BYTES,
        'protocol': f'esp-jsonl-rpc-lite-v{PROTOCOL_VERSION}',
    }


@app.post('/request')
async def request(payload: BLEUARTRequestPayload) -> dict:
    if _request_data_size(payload.data) > MAX_REQUEST_DATA_BYTES:
        raise HTTPException(status_code=413, detail=f'Request data exceeds {MAX_REQUEST_DATA_BYTES} bytes')

    # Request with coroutine lock
    async with app.state.request_lock:
        request_id = uuid4().hex
        response_future = asyncio.get_running_loop().create_future()
        app.state.pending_requests[request_id] = response_future

        try:
            # Send command to BLE device
            success = await app.state.bridge.send(
                encode_jsonl_request(request_id, payload.op, payload.data),
                with_response=True,
            )
            if not success:
                raise HTTPException(status_code=500, detail='Failed to send data to device')

            # Wait for BLE device to respond
            try:
                response = await asyncio.wait_for(response_future, timeout=payload.timeout)
            except asyncio.TimeoutError:
                raise HTTPException(status_code=504, detail='Response timeout')
            except RuntimeError as e:
                raise HTTPException(status_code=502, detail=str(e))
        finally:
            app.state.pending_requests.pop(request_id, None)

        return {'ok': True, 'data': response}
