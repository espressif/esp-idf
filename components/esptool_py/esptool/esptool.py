#
# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0
#

import os
import sys

try:
    sys.path.remove(os.path.dirname(os.path.realpath(__file__)))  # do not import this script
except ValueError:
    pass

try:
    import esptool
except ImportError:
    raise ModuleNotFoundError('No module named "esptool" please install esptool.py by running '
                              'the install and export scripts.')

if __name__ == '__main__':
    esptool._main()  # type: ignore
