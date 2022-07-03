# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from click.core import Context


class FatalError(RuntimeError):
    """
    Wrapper class for runtime errors that aren't caused by bugs in idf.py or the build process.
    """

    def __init__(self, message: str, ctx: Context=None):
        super(RuntimeError, self).__init__(message)
        # if context is defined, check for the cleanup tasks
        if ctx is not None and 'cleanup' in ctx.meta:
            # cleans up the environment before failure
            ctx.meta['cleanup']()


class NoSerialPortFoundError(FatalError):
    pass
