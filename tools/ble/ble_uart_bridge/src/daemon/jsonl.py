# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import asyncio
import json
from json import JSONDecodeError
from typing import Any

from loguru import logger

from ..core.constants import DEFAULT_READ_BUFFER_LIMIT

PROTOCOL_VERSION = 1


def encode_jsonl_request(request_id: str, op: str, data: Any) -> str:
    return json.dumps({'v': PROTOCOL_VERSION, 'id': request_id, 'op': op, 'data': data}) + '\n'


def drain_jsonl_messages(
    buffer: bytearray, data: bytes, buffer_limit: int = DEFAULT_READ_BUFFER_LIMIT
) -> list[dict[str, Any]]:
    buffer.extend(data)
    messages: list[dict[str, Any]] = []

    while True:
        try:
            newline_index = buffer.index(b'\n')
        except ValueError:
            break

        line = bytes(buffer[:newline_index])
        del buffer[: newline_index + 1]
        if not line:
            continue

        try:
            message = json.loads(line.decode())
        except (JSONDecodeError, UnicodeDecodeError):
            logger.warning(f'Invalid JSONL message from device, dropping data: {line!r}')
            continue

        if not isinstance(message, dict):
            logger.warning(f'JSONL message is not an object, dropping data: {message!r}')
            continue

        messages.append(message)

    if len(buffer) > buffer_limit:
        logger.warning(f'JSONL receive buffer exceeded {buffer_limit} bytes, dropping buffered data')
        buffer.clear()

    return messages


def resolve_pending_response(pending_requests: dict[str, asyncio.Future[Any]], message: dict[str, Any]) -> bool:
    request_id = message.get('id')
    if not isinstance(request_id, str):
        return False

    future = pending_requests.get(request_id)
    if future is None:
        return False

    pending_requests.pop(request_id)
    if future.done():
        return True

    version = message.get('v')
    if version is not None and version != PROTOCOL_VERSION:
        future.set_exception(RuntimeError(f'Invalid protocol response: unsupported version {version!r}'))
        return True

    if 'ok' in message:
        ok = message['ok']
        if not isinstance(ok, bool):
            future.set_exception(RuntimeError('Invalid protocol response: ok must be boolean'))
            return True

        if ok:
            if 'data' not in message:
                future.set_exception(RuntimeError('Invalid protocol response: missing data for successful response'))
                return True
            future.set_result(message['data'])
            return True

        error = message.get('error')
        if not isinstance(error, str) or not error:
            future.set_exception(RuntimeError('Invalid protocol response: error must be non-empty string'))
            return True
        future.set_exception(RuntimeError(error))
        return True

    if 'error' in message:
        future.set_exception(RuntimeError(str(message['error'])))
        return True

    if 'response' in message:
        future.set_result(message['response'])
        return True

    future.set_exception(RuntimeError('Invalid protocol response: missing ok/data, response, or error'))
    return True
