# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import annotations

import asyncio

from .console import BLEUARTConsole
from .console import ConsoleEncoding
from .console import ConsoleTerminator


def run_console(
    device_id: str,
    terminator: ConsoleTerminator | str = ConsoleTerminator.lf,
    encoding: ConsoleEncoding | str = ConsoleEncoding.text,
    with_response: bool = False,
) -> None:
    # Initialize BLE UART Console
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
