# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Status panel widget — docked to bottom, shows live stats.

See Spec Section 11.
"""
from rich.text import Text
from src.backend.models import format_bytes
from src.backend.models import format_throughput
from src.backend.models import FrameStats
from src.backend.models import SyncState
from src.backend.stats import UART_BITS_PER_BYTE
from textual.reactive import reactive
from textual.widget import Widget


def _format_speed(bps: float) -> str:
    return format_throughput(bps / UART_BITS_PER_BYTE)  # type: ignore[no-any-return]


_SYNC_COLORS = {
    SyncState.SEARCHING: 'yellow',
    SyncState.CONFIRMING_SYNC: 'cyan',
    SyncState.SYNCED: 'green',
    SyncState.CONFIRMING_LOSS: 'red',
}


class StatusPanel(Widget):
    DEFAULT_CSS = """
    StatusPanel {
        dock: bottom;
        height: 3;
        border-top: solid $accent;
        padding: 0 1;
    }
    """

    stats: reactive[FrameStats] = reactive(FrameStats)
    disconnected: reactive[bool] = reactive(False)

    def render(self) -> Text:
        s = self.stats
        if self.disconnected:
            line1 = '[bold red]DISCONNECTED[/bold red]'
            line2 = 'Backend stopped — serial connection closed'
            return Text.from_markup(f'{line1}\n{line2}')
        sync_color = _SYNC_COLORS.get(s.sync_state, 'white')

        if s.checksum_algorithm and s.checksum_scope:
            cksum_str = f' | Checksum: {s.checksum_algorithm.value} / {s.checksum_scope.value}'
        else:
            cksum_str = ''
        line1 = f'Sync: [{sync_color}]{s.sync_state.value}[/{sync_color}]{cksum_str} | Press [bold]h[/bold] for help'
        t = s.transport
        loss = s.loss
        loss_style = 'red' if loss.total_frames > 0 else 'yellow'
        line2 = (
            f'RX: {format_bytes(t.rx_bytes)}  '
            f'Speed: {_format_speed(t.bps)}  '
            f'Max: {_format_speed(t.max_bps)}  '
            f'Rate: {t.fps:.0f} fps  '
            f'[{loss_style}]Lost: {loss.total_frames} frames, {format_bytes(loss.total_bytes)}[/{loss_style}]'
        )

        return Text.from_markup(f'{line1}\n{line2}')
