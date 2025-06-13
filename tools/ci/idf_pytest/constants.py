# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""
Pytest Related Constants. Don't import third-party packages here.
"""

import typing as t

SUPPORTED_TARGETS = [
    'esp32',
    'esp32s2',
    'esp32c3',
    'esp32s3',
    'esp32c2',
    'esp32c6',
    'esp32h2',
    'esp32p4',
    'esp32c5',
    'esp32c61',
]
PREVIEW_TARGETS: t.List[str] = []  # this PREVIEW_TARGETS excludes 'linux' target

DEFAULT_LOGDIR = 'pytest-embedded'

# by default the timeout is 1h, for some special cases we need to extend it
TIMEOUT_4H_MARKERS = [
    'ethernet_stress',
]
