#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2017-2023 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0
#

import subprocess
import sys

if __name__ == '__main__':
    sys.exit(subprocess.run([sys.executable, '-m', 'esp_idf_size'] + sys.argv[1:]).returncode)
