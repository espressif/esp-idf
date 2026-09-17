# SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import sys
from collections.abc import Callable
from typing import Any
from typing import cast

from rich.markup import escape
from rich.text import Text
from utils import conf

try:
    # The ESP-IDF virtual environment hasn't been verified yet, so see if esp-pylib
    # can be imported to display error and status messages nicely.
    from esp_pylib.logger import EspLog
    from esp_pylib.logger import Verbosity
    from esp_pylib.logger import log
except ImportError as e:
    sys.exit(f'error: Unable to import esp-pylib: {e}. Please execute the install script.')


def configure_output(*, no_color: bool = False, quiet: bool = False, debug: bool = False) -> None:
    # log is typed as EspLogBase; console options live on the default EspLog singleton.
    # Only pass no_color=True when requested. False would override Rich's NO_COLOR detection.
    options: dict[str, Any] = {'quiet': quiet}
    if no_color:
        options['no_color'] = True
    cast(EspLog, log).set_console_options(**options)
    if quiet:
        log.set_verbosity(Verbosity.SILENT)
    elif debug:
        log.set_verbosity(Verbosity.VERBOSE)
    else:
        log.set_verbosity(Verbosity.NORMAL)


def status_message(msg: str, msg_result: str = '', rv_on_ok: bool = False, die_on_err: bool = True) -> Callable:
    def inner(func: Callable) -> Callable:
        def wrapper(*args: Any, **kwargs: Any) -> Any:
            log.print(f'[dark_orange]*[/dark_orange] {msg} ... ', file=sys.stderr, end='')

            try:
                rv = func(*args, **kwargs)
            except Exception as e:
                log.print('[red]FAILED[/red]', file=sys.stderr)
                if conf.ARGS and conf.ARGS.debug:
                    raise
                if not die_on_err:
                    return None
                log.die(escape(str(e)))

            if rv_on_ok:
                result = rv if isinstance(rv, Text) else Text(str(rv), style='green')
                log.print(result, file=sys.stderr)
            elif msg_result:
                log.print(f'[green]{msg_result}[/green]', file=sys.stderr)
            else:
                log.print('[green]OK[/green]', file=sys.stderr)

            return rv

        return wrapper

    return inner
