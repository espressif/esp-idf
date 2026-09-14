#
# SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0
#

import signal
import subprocess
import sys

if __name__ == '__main__':
    # Ctrl+C is handled by the monitor itself, so this wrapper must not exit before it.
    # Install a handler, not SIG_IGN, which would be inherited and disable Ctrl+C in the
    # monitor.
    signal.signal(signal.SIGINT, lambda *_: None)
    sys.exit(subprocess.run([sys.executable, '-m', 'esp_idf_monitor'] + sys.argv[1:]).returncode)
