# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import json
from typing import Any
from urllib.error import HTTPError
from urllib.error import URLError
from urllib.request import Request
from urllib.request import urlopen

import uvicorn

from .server import app as daemon_app


def _daemon_url(host: str, port: int, path: str) -> str:
    return f'http://{host}:{port}{path}'


def _request_json(
    method: str,
    url: str,
    payload: dict[str, Any] | None = None,
    timeout: float = 10.0,
) -> dict[str, Any]:
    data = json.dumps(payload).encode() if payload is not None else None
    headers = {'Content-Type': 'application/json'} if payload is not None else {}
    request = Request(url, data=data, headers=headers, method=method)

    try:
        with urlopen(request, timeout=timeout) as response:
            body = response.read().decode()
    except HTTPError as e:
        detail = e.read().decode(errors='replace')
        raise RuntimeError(f'Daemon request failed with HTTP {e.code}: {detail}') from e
    except TimeoutError as e:
        raise RuntimeError(f'Timed out waiting for BLE UART Daemon: {url}') from e
    except URLError as e:
        raise RuntimeError(f'Failed to connect to BLE UART Daemon: {e.reason}') from e

    if not body:
        return {}

    try:
        result = json.loads(body)
    except json.JSONDecodeError as e:
        raise RuntimeError(f'Invalid daemon response: {body!r}') from e
    if not isinstance(result, dict):
        raise RuntimeError(f'Invalid daemon response: {result!r}')
    return result


def run_daemon(device_id: str, host: str, port: int) -> None:
    daemon_app.state.device_id = device_id
    uvicorn.run(daemon_app, host=host, port=port)


def run_daemon_status(host: str = '127.0.0.1', port: int = 8888) -> None:
    try:
        status = _request_json('GET', _daemon_url(host, port, '/status'))
    except RuntimeError as e:
        print(e)
        raise SystemExit(1) from e
    print(json.dumps(status, indent=2))


def run_daemon_send(
    data: str,
    op: str = 'raw',
    json_payload: bool = False,
    timeout: float = 10.0,
    host: str = '127.0.0.1',
    port: int = 8888,
) -> None:
    try:
        payload_data: Any = json.loads(data) if json_payload else data
    except json.JSONDecodeError as e:
        print(f'Invalid JSON payload: {e}')
        raise SystemExit(1) from e

    try:
        response = _request_json(
            'POST',
            _daemon_url(host, port, '/request'),
            payload={'op': op, 'data': payload_data, 'timeout': timeout},
            timeout=timeout + 1.0,
        )
    except RuntimeError as e:
        print(e)
        raise SystemExit(1) from e
    result = response.get('data', response.get('response', ''))
    print(result if isinstance(result, str) else json.dumps(result))
