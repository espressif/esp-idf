#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2017-2025 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0
#
import os
import subprocess
import sys

if __name__ == '__main__':
    try:
        import esp_idf_size  # noqa: F401

        # Enforce NG mode for esp-idf-size v 1.x. After v 2.x is fully incorporated, 'ESP_IDF_SIZE_NG' can be removed.
        os.environ['ESP_IDF_SIZE_NG'] = '1'
    except ImportError:
        print('WARNING: esp-idf-size not installed, please run the install script to install it', file=sys.stderr)
        raise SystemExit(1)

    sys.exit(subprocess.run([sys.executable, '-m', 'esp_idf_size'] + sys.argv[1:]).returncode)
