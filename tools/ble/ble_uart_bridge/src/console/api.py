# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from __future__ import annotations

import asyncio
from typing import Union

from .console import BLEUARTConsole
from .console import ConsoleEncoding
from .console import ConsoleTerminator
# Keep annotations compatible with Python 3.9.
# ruff: noqa: UP007


def run_console(
    device_id: str,
    terminator: Union[ConsoleTerminator, str] = ConsoleTerminator.lf,
    encoding: Union[ConsoleEncoding, str] = ConsoleEncoding.text,
    with_response: bool = False,
) -> None:
    # Initialize ESP-BLE-UART Console
    console = BLEUARTConsole(
        device_id,
        terminator=terminator,
        encoding=encoding,
        with_response=with_response,
    )

    try:
        asyncio.run(console.run_console())
    except KeyboardInterrupt:
        pass
