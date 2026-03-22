# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""Launch Screen — interactive setup for port, baud rate, and log directory.

Shown on startup when --port is not provided via CLI.
Dismissed with a LaunchConfig result on Connect, or None on quit.
"""

from pathlib import Path

from textual import on
from textual import work
from textual.app import ComposeResult
from textual.binding import Binding
from textual.containers import Center
from textual.containers import Vertical
from textual.screen import Screen
from textual.widgets import Button
from textual.widgets import Input
from textual.widgets import Label
from textual.widgets import Select
from textual_fspicker import SelectDirectory

from src.backend.models import LaunchConfig
from src.backend.uart_transport import list_serial_ports

BAUD_RATES: list[int] = [115200, 230400, 460800, 921600, 1500000, 2000000, 3000000]
DEFAULT_BAUD_RATE: int = 3000000


class LaunchScreen(Screen[LaunchConfig | None]):
    """Interactive setup screen for BLE Log Console."""

    DEFAULT_CSS = """
    LaunchScreen {
        align: center middle;
    }

    #launch-container {
        width: 60;
        max-height: 80%;
        background: $surface;
        padding: 1 2;
        border: thick $accent;
    }

    #launch-title {
        text-align: center;
        text-style: bold;
        margin-bottom: 1;
    }

    .field-label {
        margin-top: 1;
    }

    .field-row {
        height: auto;
        layout: horizontal;
    }

    #port-select {
        width: 1fr;
    }

    #refresh-btn {
        width: auto;
        min-width: 12;
    }

    #dir-input {
        width: 1fr;
    }

    #browse-btn {
        width: auto;
        min-width: 12;
    }

    #connect-btn {
        margin-top: 2;
    }

    #no-ports-label {
        color: $warning;
    }
    """

    BINDINGS = [
        Binding('q', 'quit', 'Quit'),
        Binding('Q', 'quit', show=False),
        Binding('ctrl+c', 'quit', show=False, priority=True),
    ]

    def __init__(self, default_log_dir: Path | None = None) -> None:
        super().__init__()
        self._default_log_dir = default_log_dir or Path.cwd()

    def compose(self) -> ComposeResult:
        ports = list_serial_ports()
        port_options = [(p, p) for p in ports]
        baud_options = [(str(b), b) for b in BAUD_RATES]

        with Vertical(id='launch-container'):
            yield Label('BLE Log Console Setup', id='launch-title')

            yield Label('Serial Port', classes='field-label')
            with Vertical(classes='field-row'):
                if port_options:
                    yield Select(port_options, value=ports[0], id='port-select')
                else:
                    yield Select([], id='port-select', prompt='No ports detected')
                    yield Label('No serial ports detected', id='no-ports-label')
                yield Button('Refresh', id='refresh-btn')

            yield Label('Baud Rate', classes='field-label')
            yield Select(baud_options, value=DEFAULT_BAUD_RATE, id='baud-select')

            yield Label('Log Directory', classes='field-label')
            with Vertical(classes='field-row'):
                yield Input(str(self._default_log_dir), id='dir-input')
                yield Button('Browse...', id='browse-btn')

            with Center():
                yield Button('Connect', variant='primary', id='connect-btn')

    @on(Button.Pressed, '#refresh-btn')
    def refresh_ports(self) -> None:
        """Re-scan serial ports and update the Select widget."""
        ports = list_serial_ports()
        port_options = [(p, p) for p in ports]
        port_select = self.query_one('#port-select', Select)
        port_select.set_options(port_options)
        if ports:
            port_select.value = ports[0]

    @on(Button.Pressed, '#browse-btn')
    @work
    async def browse_directory(self) -> None:
        """Open a directory picker dialog."""
        current = self.query_one('#dir-input', Input).value
        start = Path(current) if current else Path.cwd()
        if not start.is_dir():
            start = Path.cwd()
        chosen = await self.app.push_screen_wait(SelectDirectory(location=start))
        if chosen is not None:
            self.query_one('#dir-input', Input).value = str(chosen)

    @on(Button.Pressed, '#connect-btn')
    def connect(self) -> None:
        """Validate and return config."""
        port_select = self.query_one('#port-select', Select)
        baud_select = self.query_one('#baud-select', Select)
        dir_input = self.query_one('#dir-input', Input)

        if port_select.value is Select.BLANK:
            self.notify('Please select a serial port', severity='error')
            return

        if baud_select.value is Select.BLANK:
            self.notify('Please select a baud rate', severity='error')
            return

        log_dir = Path(dir_input.value)

        config = LaunchConfig(
            port=str(port_select.value),
            baudrate=int(baud_select.value),  # type: ignore[arg-type]  # guarded above
            log_dir=log_dir,
        )
        self.dismiss(config)

    def action_quit(self) -> None:
        self.dismiss(None)
