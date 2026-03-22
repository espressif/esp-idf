# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import sys

if sys.version_info < (3, 10):  # noqa: UP036 — runtime guard for users on old Python
    print(f'Error: Python 3.10 or later is required.\nCurrent version: {sys.version}')
    sys.exit(1)

try:
    import textual  # noqa: F401
except ImportError:
    print(
        "Error: 'textual' package is not installed.\n"
        "Run 'run.sh' (Linux/macOS) or 'run.bat' (Windows) "
        'to launch with auto-setup.'
    )
    sys.exit(1)
