# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from esp_pylib.errors import FatalError as PylibFatalError
from rich_click import Context


class FatalError(PylibFatalError):
    """
    Wrapper class for runtime errors that aren't caused by bugs in idf.py or the build process.

    Extends the shared ``esp_pylib.errors.FatalError`` (a ``RuntimeError``) while keeping the
    idf.py-specific context cleanup hook.
    """

    def __init__(self, message: str, ctx: Context = None):
        super().__init__(message)
        # if context is defined, check for the cleanup tasks
        if ctx is not None and 'cleanup' in ctx.meta:
            # cleans up the environment before failure
            ctx.meta['cleanup']()


class NoSerialPortFoundError(FatalError):
    pass
