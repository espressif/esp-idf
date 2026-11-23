# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""C61 is covered by the C6 test because it shares the same implementation."""

from controller_disable_test_utils import run

if __name__ == '__main__':
    run(('esp32c5',))
