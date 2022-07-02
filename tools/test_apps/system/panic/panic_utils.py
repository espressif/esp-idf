# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import hashlib
import logging
import re
import time
from subprocess import Popen


class NoGdbProcessError(ValueError):
    """Raise when trying to interact with gdb subprocess, but it does not exist.
    It may have been killed and removed, or failed to initialize for some reason."""

    pass


def sha256(file: str) -> str:
    res = hashlib.sha256()
    with open(file, 'rb') as fr:
        res.update(fr.read())
    return res.hexdigest()


def quote_string(string: str) -> str:
    """Return a shell-escaped version of the string *string*."""
    _find_unsafe = re.compile(r'[^\w@%+=:,./-]', re.ASCII).search
    if not string:
        return "''"
    if _find_unsafe(string) is None:
        return string

    # use single quotes, and put single quotes into double quotes
    # the string $'b is then quoted as '$'"'"'b'
    return "'" + string.replace("'", "'\"'\"'") + "'"


def verify_valid_gdb_subprocess(gdb_process: Popen) -> None:
    """Verify there is a process object, and that it is still running.
    Raise NoGdbProcessError if either of the above are not true."""
    if not gdb_process:
        raise NoGdbProcessError('gdb process is not attached')

    elif gdb_process.poll() is not None:
        raise NoGdbProcessError(
            'gdb process has already finished with return code: %s'
            % str(gdb_process.poll())
        )


def attach_logger() -> logging.Logger:
    handler = logging.StreamHandler()
    handler.setFormatter(logging.Formatter('%(message)s'))
    unique_number = time.time()
    logger = logging.getLogger(__name__ + '.' + str(unique_number))
    logger.propagate = False
    logger.setLevel(logging.ERROR)
    logger.addHandler(handler)
    return logger
