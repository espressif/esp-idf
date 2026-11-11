# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""C61/H21 share the C6/H2 production sources."""

from controller_disable_test_utils import run

if __name__ == '__main__':
    run(('esp32c2', 'esp32c6', 'esp32h2'))
