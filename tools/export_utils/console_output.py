# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import sys
from typing import Any
from typing import Callable

from utils import conf

try:
    # The ESP-IDF virtual environment hasn't been verified yet, so see if the rich library
    # can be imported to display error and status messages nicely.
    from rich.console import Console
except ImportError as e:
    sys.exit(f'error: Unable to import the rich module: {e}. Please execute the install script.')

CONSOLE_STDERR = Console(stderr=True, width=255)
CONSOLE_STDOUT = Console(width=255)


def status_message(msg: str, msg_result: str='', rv_on_ok: bool=False, die_on_err: bool=True) -> Callable:
    def inner(func: Callable) -> Callable:
        def wrapper(*args: Any, **kwargs: Any) -> Any:
            eprint(f'[dark_orange]*[/dark_orange] {msg} ... ', end='')

            try:
                rv = func(*args, **kwargs)
            except Exception as e:
                eprint('[red]FAILED[/red]')
                if conf.ARGS.debug:
                    raise
                if not die_on_err:
                    return None
                die(str(e))

            if rv_on_ok:
                eprint(f'[green]{rv}[/green]')
            elif msg_result:
                eprint(f'[green]{msg_result}[/green]')
            else:
                eprint('[green]OK[/green]')

            return rv
        return wrapper
    return inner


def err(*args: Any, **kwargs: Any) -> None:
    CONSOLE_STDERR.print('[red]error[/red]: ', *args, **kwargs)  # type: ignore


def warn(*args: Any, **kwargs: Any) -> None:
    CONSOLE_STDERR.print('[yellow]warning[/yellow]: ', *args, **kwargs)  # type: ignore


def debug(*args: Any, **kwargs: Any) -> None:
    if not conf.ARGS.debug:
        return
    CONSOLE_STDERR.print('[green_yellow]debug[/green_yellow]: ', *args, **kwargs)  # type: ignore


def die(*args: Any, **kwargs: Any) -> None:
    err(*args, **kwargs)
    sys.exit(1)


def eprint(*args: Any, **kwargs: Any) -> None:
    CONSOLE_STDERR.print(*args, **kwargs)  # type: ignore


def oprint(*args: Any, **kwargs: Any) -> None:
    CONSOLE_STDOUT.print(*args, **kwargs)  # type: ignore
