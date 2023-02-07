#
# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0
#

import subprocess
import sys

if __name__ == '__main__':
    sys.exit(subprocess.run([sys.executable, '-m', 'esp_idf_monitor'] + sys.argv[1:]).returncode)
