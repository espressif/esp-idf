#
# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0
#

import subprocess
import sys

if __name__ == '__main__':
    print(
        "Warning: DEPRECATED: 'espsecure.py' wrapper is deprecated. "
        "Please use 'espsecure' or 'python -m espsecure' instead.",
        file=sys.stderr,
    )
    sys.exit(subprocess.run([sys.executable, '-m', 'espsecure'] + sys.argv[1:]).returncode)
