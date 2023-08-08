#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0
#
# Script for checking the compatibility of the Python interpreter with ESP-IDF.
#
# There are related tools/detect_python.{sh,fish} scripts which are called earlier when the paths are not properly
# set-up and they only intend to prefer the use of Python 3 over Python 2. Why not more? All possible executables
# (python3.8, python3.9, ...) cannot be hardcoded there and at the end, the user is responsible to set-up a system
# where "python" or "python3" of compatible version is available.

import sys

try:
    # Python 2 is not supported anymore but still the old way of typing is used here in order to give a nice Python
    # version failure and not a typing exception.
    from typing import Iterable
except ImportError:
    pass

OLDEST_PYTHON_SUPPORTED = (3, 8)  # keep it as tuple for comparison with sys.version_info


def _ver_to_str(it):  # type: (Iterable) -> str
    return '.'.join(str(x) for x in it)


def is_supported():  # type: () -> bool
    return sys.version_info[:2] >= OLDEST_PYTHON_SUPPORTED[:2]


def check():  # type: () -> None
    if not is_supported():
        raise RuntimeError(
            'ESP-IDF supports Python {} or newer but you are using Python {}. Please upgrade your '
            'installation as described in the documentation.'.format(
                _ver_to_str(OLDEST_PYTHON_SUPPORTED), _ver_to_str(sys.version_info[:3])
            )
        )


if __name__ == '__main__':
    check()
