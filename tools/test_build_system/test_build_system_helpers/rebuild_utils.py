# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""Shared helpers and constants for rebuild tests (test_rebuild*.py)."""

from .idf_utils import IdfPyFunc
from .snapshot import get_snapshot

# Paths excluded from mtime snapshots (volatile or external-project stamps).
BUILD_SNAPSHOT_EXCLUDE = [
    'build/log/*',
    'build/CMakeFiles/bootloader-complete',
    'build/bootloader-prefix/src/bootloader-stamp/bootloader-done',
]


def rebuild_and_check(
    idf_py: IdfPyFunc, should_be_rebuilt: str | list[str], should_not_be_rebuilt: str | list[str]
) -> None:
    """Assert that files matching *should_be_rebuilt* change and *should_not_be_rebuilt* stay the same after a build."""
    snapshot_should_be_rebuilt = get_snapshot(should_be_rebuilt)
    snapshot_should_not_be_rebuilt = get_snapshot(should_not_be_rebuilt)
    idf_py('build')
    snapshot_should_be_rebuilt.assert_different(get_snapshot(should_be_rebuilt))
    snapshot_should_not_be_rebuilt.assert_same(get_snapshot(should_not_be_rebuilt))
