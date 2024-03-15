# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import os
import sys
from typing import Any
from typing import Callable

import menuconfig

if os.name == 'nt':
    import curses


def main() -> None:
    if os.name == 'nt' and sys.version_info[:2] >= (3, 12):
        curses.wrapper = _wrapper  # type: ignore
    menuconfig._main()


def _wrapper(func: Callable) -> Any:
    # Using workaround to address windows-curses bug on Python 3.12
    # More info: https://github.com/zephyrproject-rtos/windows-curses/issues/50
    # This function is a copy of curses.wrapper with changes commented further in the code
    if os.name == 'nt' and sys.version_info[:2] >= (3, 12):
        stdscr = None
        try:
            import _curses

            # This crashes on Python 3.12 and needs to be commented out.
            # It is not needed for the menuconfig to work, though.
            # setupterm(term=_os.environ.get('TERM', 'unknown'),
            # fd=_sys.__stdout__.fileno())
            stdscr = _curses.initscr()
            for key, value in _curses.__dict__.items():
                if key[0:4] == 'ACS_' or key in ('LINES', 'COLS'):
                    setattr(curses, key, value)

            curses.noecho()
            curses.cbreak()

            try:
                curses.start_color()
            except:  # noqa: E722
                pass

            if stdscr is not None:
                stdscr.keypad(True)
                func(stdscr)
        finally:
            if stdscr is not None:
                stdscr.keypad(False)
            curses.echo()
            curses.nocbreak()
            return curses.endwin()
    else:
        return curses.wrapper(func)


if __name__ == '__main__':
    main()
