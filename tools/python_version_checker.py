#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0
#
# Script for checking the compatibility of the Python interpreter with ESP-IDF.
#
# There are related tools/detect_python.{sh,fish} scripts which are called earlier when the paths are not properly
# set-up and they only intend to prefer the use of Python 3 over Python 2. Why not more? All possible executables
# (python3.10, python3.11, ...) cannot be hardcoded there and at the end, the user is responsible to set-up a system
# where "python" or "python3" of compatible version is available.
import sys
from collections.abc import Iterable

OLDEST_PYTHON_SUPPORTED = (3, 10)  # keep it as tuple for comparison with sys.version_info


def _ver_to_str(it: Iterable) -> str:
    return '.'.join(str(x) for x in it)


def is_supported() -> bool:
    return sys.version_info[:2] >= OLDEST_PYTHON_SUPPORTED[:2]


def check() -> None:
    if not is_supported():
        raise RuntimeError(
            f'ESP-IDF supports Python {_ver_to_str(OLDEST_PYTHON_SUPPORTED)} or newer but you are using Python '
            f'{_ver_to_str(sys.version_info[:3])}. Please upgrade your installation as described in the documentation.'
        )


if __name__ == '__main__':
    check()
