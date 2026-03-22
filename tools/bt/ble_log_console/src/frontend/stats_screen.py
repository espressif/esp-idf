# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""Modal screen for per-source frame statistics display.

Pushed by the 'd' keybinding; dismissed by Escape or 'd' again.
Refreshes every second to show live throughput data.
"""

from __future__ import annotations

from typing import TYPE_CHECKING

from rich.table import Table
from rich.text import Text
from textual.app import ComposeResult
from textual.binding import Binding
from textual.containers import Vertical
from textual.screen import ModalScreen
from textual.widgets import Static

from src.backend.models import FunnelSnapshot
from src.backend.models import format_bytes
from src.backend.models import format_throughput
from src.backend.models import resolve_source_name

if TYPE_CHECKING:
    from src.app import BLELogApp

REFRESH_INTERVAL_SEC = 1.0


def _fmt_frames(n: int) -> str:
    return str(n) if n > 0 else '-'


def _fmt_loss_frames(n: int) -> Text:
    if n == 0:
        return Text('-')
    return Text(str(n), style='red')


def _fmt_loss_bytes(n: int) -> Text:
    if n == 0:
        return Text('-')
    return Text(format_bytes(n), style='red')


def _build_firmware_table(snapshots: list[FunnelSnapshot]) -> Table:
    table = Table(title='Firmware Counters (since chip init)', expand=True)
    table.add_column('Source', style='cyan', no_wrap=True, min_width=12, max_width=16)
    table.add_column('Written\nFrames', justify='right', min_width=10, max_width=12)
    table.add_column('Written\nBytes', justify='right', min_width=10, max_width=12)
    table.add_column('Buffer Loss\nFrames', justify='right', min_width=12, max_width=14)
    table.add_column('Buffer Loss\nBytes', justify='right', min_width=12, max_width=14)

    for snap in snapshots:
        table.add_row(
            resolve_source_name(snap.source),
            _fmt_frames(snap.written.frames),
            format_bytes(snap.written.bytes) if snap.written.bytes > 0 else '-',
            _fmt_loss_frames(snap.buffer_loss.frames),
            _fmt_loss_bytes(snap.buffer_loss.bytes),
        )

    return table


def _build_console_table(snapshots: list[FunnelSnapshot]) -> Table:
    table = Table(title='Console Measurements (since console start)', expand=True)
    table.add_column('Source', style='cyan', no_wrap=True, min_width=12, max_width=16)
    table.add_column('Received\nFrames', justify='right', min_width=10, max_width=12)
    table.add_column('Received\nBytes', justify='right', min_width=10, max_width=12)
    table.add_column('Average\nFrames/s', justify='right', style='magenta', min_width=10, max_width=12)
    table.add_column('Average\nBytes/s', justify='right', style='magenta', min_width=10, max_width=12)
    table.add_column('Peak\nFrames/10ms', justify='right', style='magenta', min_width=12, max_width=14)
    table.add_column('Peak\nBytes/s', justify='right', style='magenta', min_width=12, max_width=14)

    for snap in snapshots:
        tp_fps = snap.throughput.throughput_fps
        tp_bps = snap.throughput.throughput_bps
        pf = snap.throughput.peak_write_frames
        pb = snap.throughput.peak_write_bytes
        wms = snap.throughput.peak_window_ms

        table.add_row(
            resolve_source_name(snap.source),
            _fmt_frames(snap.received.frames),
            format_bytes(snap.received.bytes) if snap.received.bytes > 0 else '-',
            f'{tp_fps:.0f}' if tp_fps > 0 else '-',
            format_throughput(tp_bps) if tp_bps > 0 else '-',
            f'{pf}' if pf > 0 else '-',
            format_throughput(pb * 1000 / wms) if pf > 0 and wms > 0 else '-',
        )

    return table


class StatsScreen(ModalScreen):
    """Modal overlay showing per-source frame statistics with live refresh."""

    DEFAULT_CSS = """
    StatsScreen {
        align: center middle;
    }

    #stats-container {
        width: 90%;
        max-width: 140;
        height: auto;
        max-height: 80%;
        overflow-y: auto;
        background: $surface;
        padding: 1 2;
        border: thick $accent;
    }

    #stats-container > Static {
        height: auto;
    }
    """

    BINDINGS = [
        Binding('escape', 'dismiss', 'Close'),
        Binding('d', 'dismiss', 'Close'),
    ]

    def __init__(self, start_time: float) -> None:
        super().__init__()
        self._start_time = start_time

    def _get_app(self) -> BLELogApp:
        return self.app  # type: ignore[return-value]

    def compose(self) -> ComposeResult:
        with Vertical(id='stats-container'):
            yield Static(id='firmware-table')
            yield Static(id='console-table')
            yield Static('[dim]Press Escape to return — refreshes every 1s[/dim]')

    def on_mount(self) -> None:
        self._refresh_table()
        self.set_interval(REFRESH_INTERVAL_SEC, self._refresh_table)

    def _refresh_table(self) -> None:
        app = self._get_app()
        snapshots = app.funnel_snapshots

        fw = self.query_one('#firmware-table', Static)
        cs = self.query_one('#console-table', Static)
        if not snapshots:
            fw.update('No data received yet.\n\nPress Escape to return.')
            cs.update('')
            return

        fw.update(_build_firmware_table(snapshots))
        cs.update(_build_console_table(snapshots))
