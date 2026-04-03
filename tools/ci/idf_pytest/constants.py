# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""
Pytest Related Constants. Don't import third-party packages here.
"""

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
PREVIEW_TARGETS: list[str] = []  # this PREVIEW_TARGETS excludes 'linux' target

DEFAULT_LOGDIR = 'pytest-embedded'

# by default the timeout is 1h, for some special cases we need to extend it
TIMEOUT_4H_MARKERS = [
    'ethernet_stress',
]

REV_MARKERS = [
    'esp32_rev3',
    'esp32c2_rev2',
    'esp32c3_rev1',
    'esp32h2_rev1_2',
    'esp32p4_rev1',
    'esp32c5_rev1',
]
