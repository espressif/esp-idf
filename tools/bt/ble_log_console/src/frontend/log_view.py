# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

"""Scrollable log view widget.

See Spec Section 11.
"""

from rich.text import Text
from textual.widgets import RichLog


class LogView(RichLog):
    DEFAULT_CSS = """
    LogView {
        height: 1fr;
    }
    """

    def __init__(self) -> None:
        super().__init__(highlight=False, markup=True, wrap=True, auto_scroll=True)

    def _write_tagged(self, tag: str, color: str, text: str) -> None:
        t = Text.from_markup(f'[dim][{color}]\\[{tag}][/{color}] [/dim]')
        t.append(text)
        self.write(t)

    def write_info(self, text: str) -> None:
        self._write_tagged('INFO', 'green', text)

    def write_warning(self, text: str) -> None:
        self._write_tagged('WARN', 'yellow', text)

    def write_error(self, text: str) -> None:
        self._write_tagged('ERROR', 'red', text)

    def write_sync(self, text: str) -> None:
        self._write_tagged('SYNC', 'cyan', text)

    def write_enh_stat(self, text: str) -> None:
        self._write_tagged('ENH_STAT', 'cyan', text)

    def write_traffic(self, text: str) -> None:
        self._write_tagged('TRAFFIC', 'magenta', text)

    def write_ascii(self, text: str) -> None:
        self.write(Text(text))
