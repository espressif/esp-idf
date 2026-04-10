#
# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0
#

import subprocess
import sys

if __name__ == '__main__':
    print(
        "Warning: DEPRECATED: 'espefuse.py' wrapper is deprecated. "
        "Please use 'espefuse' or 'python -m espefuse' instead.",
        file=sys.stderr,
    )
    sys.exit(subprocess.run([sys.executable, '-m', 'espefuse'] + sys.argv[1:]).returncode)
