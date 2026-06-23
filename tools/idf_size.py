#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2017-2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0
#
import subprocess
import sys

from esp_pylib.excepthook import install_exception_reporting
from esp_pylib.logger import log

if __name__ == '__main__':
    install_exception_reporting()
    try:
        import esp_idf_size  # noqa: F401

    except ImportError:
        log.warn('esp-idf-size not installed, please run the install script to install it')
        sys.exit(1)

    sys.exit(subprocess.run([sys.executable, '-m', 'esp_idf_size'] + sys.argv[1:]).returncode)
