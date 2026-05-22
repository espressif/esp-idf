# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

from __future__ import annotations

import asyncio
import codecs
from enum import Enum

from loguru import logger
from rich.highlighter import Highlighter
from rich.text import Text
from textual import on
from textual.app import App
from textual.app import ComposeResult
from textual.binding import Binding
from textual.binding import BindingType
from textual.message import Message
from textual.widgets import Footer
from textual.widgets import Header
from textual.widgets import Input
from textual.widgets import Log

from ..core import BLEUARTBridge

CONSOLE_TEXT_ENCODING = 'utf-8'


class ConsoleTerminator(str, Enum):
    lf = 'lf'
    crlf = 'crlf'
    none = 'none'


class ConsoleEncoding(str, Enum):
    text = 'text'
    hex = 'hex'


TERMINATORS: dict[ConsoleTerminator, bytes] = {
    ConsoleTerminator.lf: b'\n',
    ConsoleTerminator.crlf: b'\r\n',
    ConsoleTerminator.none: b'',
}

CONSOLE_CSS = """
Screen {
    layout: vertical;
}

#log {
    height: 1fr;
}
"""

CONSOLE_BINDINGS: list[BindingType] = [
    Binding('ctrl+c', 'quit', 'Quit', priority=True),
    Binding('ctrl+d', 'quit', show=False),
    Binding('ctrl+l', 'clear_log', 'Clear'),
]


class BLEUARTLogHighlighter(Highlighter):
    def highlight(self, text: Text) -> None:
        if text.plain.startswith('[RX] '):
            text.stylize('green', 0, 5)
        elif text.plain.startswith('[TX] '):
            text.stylize('yellow', 0, 5)
        elif text.plain.startswith('[INFO] '):
            text.stylize('cyan', 0, 7)


class BLEUARTLog(Log):
    can_focus = False
    FOCUS_ON_CLICK = False

    def __init__(self) -> None:
        super().__init__(id='log', highlight=True, auto_scroll=True)
        self.highlighter = BLEUARTLogHighlighter()


class BLEUARTDataReceived(Message):
    def __init__(self, data: bytes) -> None:
        super().__init__()
        self.data = data


class BLEUARTConsole(App):
    CSS = CONSOLE_CSS
    BINDINGS = CONSOLE_BINDINGS
    ENABLE_COMMAND_PALETTE = False

    def __init__(
        self,
        device_id: str,
        terminator: ConsoleTerminator | str = ConsoleTerminator.lf,
        encoding: ConsoleEncoding | str = ConsoleEncoding.text,
        with_response: bool = False,
    ) -> None:
        super().__init__()
        self._device_id = device_id
        self._bridge = BLEUARTBridge(device_id)
        self._terminator = self._parse_terminator(terminator)
        self._encoding = self._parse_encoding(encoding)
        self._with_response = with_response
        self._rx_decoder = codecs.getincrementaldecoder(CONSOLE_TEXT_ENCODING)(errors='replace')
        self._rx_line_open = False
        self._rx_pending: list[str] = []
        self._ui_ready = False

    @staticmethod
    def _parse_terminator(terminator: ConsoleTerminator | str) -> bytes:
        try:
            return TERMINATORS[ConsoleTerminator(terminator)]
        except ValueError:
            choices = ', '.join(item.value for item in ConsoleTerminator)
            raise ValueError(f'Unsupported terminator: {terminator}. Expected one of: {choices}') from None

    @staticmethod
    def _parse_encoding(encoding: ConsoleEncoding | str) -> ConsoleEncoding:
        try:
            return ConsoleEncoding(encoding)
        except ValueError:
            choices = ', '.join(item.value for item in ConsoleEncoding)
            raise ValueError(f'Unsupported encoding: {encoding}. Expected one of: {choices}')

    async def run_console(self) -> None:
        # Connect before starting Textual so failed connections do not flash an empty UI.
        self._bridge.add_rx_handler(self._on_ble_notif)
        try:
            # Should try connection to catch KeyInterrupt during connection establishment
            if not await self._bridge.connect():
                logger.error(f'Failed to open BLE UART Console for {self._device_id}')
                return

            # Run UI event loop
            await self.run_async()
        finally:
            # Disconnect from device
            logger.info(f'Closing BLE UART Console for {self._device_id}...')
            await self._bridge.disconnect()

    # Textual lifecycle hook: build the widget tree before the app is mounted.
    def compose(self) -> ComposeResult:
        yield Header()
        yield BLEUARTLog()
        yield Input(placeholder='Send BLE UART data...', id='input')
        yield Footer()

    # Device notification callback
    def _on_ble_notif(self, data: bytearray) -> None:
        self.post_message(BLEUARTDataReceived(bytes(data)))

    @on(BLEUARTDataReceived)
    def on_ble_uart_data_received(self, message: BLEUARTDataReceived) -> None:
        if self._encoding == 'hex':
            self._write_rx(f'{message.data.hex(" ")}\n')
            return

        text = self._rx_decoder.decode(message.data)
        if not text:
            return

        self._write_rx(text)

    # Textual lifecycle hook: widgets are ready, so BLE can be connected and UI updated.
    async def on_mount(self) -> None:
        self._ui_ready = True
        self.title = f'BLE UART — {self._device_id}'
        self.query_one('#input', Input).focus()
        self._write_info(f'Connected to {self._device_id}')
        self._drain_rx_pending()
        self.run_worker(self._monitor_connection)

    async def on_unmount(self) -> None:
        self._ui_ready = False

    # Textual event handler: called when the Input widget is submitted with Enter.
    @on(Input.Submitted)
    async def on_input_submitted(self, event: Input.Submitted) -> None:
        line = event.value
        event.input.clear()

        if not line:
            return

        try:
            if not self._bridge.is_connected:
                self._write_tx(f'failed, not connected: {line}')
                return

            payload = self._encode_tx(line)
            if await self._bridge.send(payload, with_response=self._with_response):
                self._write_tx(line)
            else:
                self._write_tx(f'failed: {line}')
        except ValueError as e:
            self._write_tx(f'failed: {e}')
        except Exception as e:
            logger.exception(e)
            self._write_tx(f'failed: {line}')

    def _encode_tx(self, line: str) -> bytes:
        if self._encoding == 'hex':
            try:
                return bytes.fromhex(line)
            except ValueError as e:
                raise ValueError('invalid hex input') from e

        try:
            payload = line.encode(CONSOLE_TEXT_ENCODING)
        except UnicodeEncodeError as e:
            raise ValueError(f'failed to encode input: {e}') from e
        return payload + self._terminator

    async def _monitor_connection(self) -> None:
        while self._bridge.is_connected:
            await asyncio.sleep(0.5)

        # DOM may be uninstalled already
        if self._ui_ready:
            self._write_info('Disconnected. Press Ctrl+C to quit.')

    def _write_rx(self, text: str) -> None:
        if not self._ui_ready:
            self._rx_pending.append(text)
            return

        log = self.query_one('#log', Log)

        for chunk in text.splitlines(keepends=True):
            if not self._rx_line_open:
                log.write('[RX] ')
                self._rx_line_open = True

            log.write(chunk)
            if chunk.endswith(('\r', '\n')):
                self._rx_line_open = False

    def _drain_rx_pending(self) -> None:
        for text in self._rx_pending:
            self._write_rx(text)
        self._rx_pending.clear()

    def _write_tx(self, text: str) -> None:
        self._write_log('[TX] ', text)

    def _write_info(self, text: str) -> None:
        self._write_log('[INFO] ', text)

    def _write_log(self, prefix: str, text: str) -> None:
        log = self.query_one('#log', Log)
        if self._rx_line_open:
            log.write('\n')
            self._rx_line_open = False

        log.write(f'{prefix}{text}\n')

    # Textual action: invoked by the ``clear_log`` key binding.
    def action_clear_log(self) -> None:
        self.query_one('#log', Log).clear()
        self._rx_line_open = False
        self._rx_pending.clear()
