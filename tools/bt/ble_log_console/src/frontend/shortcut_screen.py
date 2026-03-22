# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""Modal screen showing available keyboard shortcuts.

Pushed by the 'h' keybinding; dismissed by Escape or 'h' again.
"""

from rich.table import Table
from textual.app import ComposeResult
from textual.binding import Binding
from textual.screen import ModalScreen
from textual.widgets import Static

_SHORTCUTS = [
    ('q', 'Quit'),
    ('c', 'Clear log'),
    ('s', 'Toggle auto-scroll'),
    ('d', 'Frame statistics'),
    ('h', 'This help screen'),
    ('r', 'Reset chip'),
]


def _build_shortcut_table() -> Table:
    """Build a Rich Table listing all keyboard shortcuts."""
    table = Table(title='Keyboard Shortcuts', expand=True)
    table.add_column('Key', style='cyan', no_wrap=True)
    table.add_column('Action')

    for key, action in _SHORTCUTS:
        table.add_row(key, action)

    return table


class ShortcutScreen(ModalScreen):
    """Modal overlay showing available keyboard shortcuts."""

    DEFAULT_CSS = """
    ShortcutScreen {
        align: center middle;
    }

    ShortcutScreen > Static {
        width: 60;
        max-height: 80%;
        background: $surface;
        padding: 1 2;
        border: thick $accent;
    }
    """

    BINDINGS = [
        Binding('escape', 'dismiss', 'Close'),
        Binding('h', 'dismiss', 'Close'),
        Binding('H', 'dismiss', show=False),
    ]

    def compose(self) -> ComposeResult:
        table = _build_shortcut_table()
        content = Static()
        content.update(table)
        yield content
        yield Static('[dim]Press Escape to return[/dim]')
