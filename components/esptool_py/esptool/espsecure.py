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
    import espsecure
except ImportError:
    raise ModuleNotFoundError('No module named "espsecure" please install espsecure.py by running '
                              'the install and export scripts.')

if __name__ == '__main__':
    espsecure._main()  # type: ignore
