# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
#
# SPDX-License-Identifier: Apache-2.0
"""Policy for which phony CMake targets appear under ``CMake Custom Targets`` in ``idf.py --help``.

Exact-name suppression is in ``IDF_PY_HELP_SKIP_TARGETS``; CI asserts that list plus the shared shape policy
(``help_phony_name_passes_shape_policy``: path chars, prefixes, suffixes, substrings) against real help output in
``tools/test_build_system/test_common.py``. Adjust those rules when new classes of internal phony targets appear.

Note: ``PHONY_BUILD_LINE_RE`` is intentionally permissive; callers must still enforce the two-step contract of
matching the line then checking the extracted rule equals ``phony`` (case-insensitive).
"""

from __future__ import annotations

import re
from collections.abc import Set

# ``cmake --build … --target help`` / ``ninja -t targets`` lines: ``name: phony`` (last ``:`` before rule).
PHONY_BUILD_LINE_RE = re.compile(r'^(.+):\s*(\S+)\s*$')
# ``build.ninja`` lines: ``build <name>: phony``.
NINJA_BUILD_PHONY_RE = re.compile(r'^build\s+(.+?)\s*:\s*phony(?:\s|$)')

# ``cmake --build`` may still run generator checks; cap wall time for help-only invocations.
CMAKE_BUILD_HELP_TIMEOUT_SEC = 15
# ``ninja -t targets`` is a local metadata query; keep tight so ``idf.py --help`` stays responsive.
NINJA_TARGETS_HELP_TIMEOUT_SEC = 5

# Targets whose names look like filesystem paths are never user-facing phony labels in this section.
# Internal / generated prefixes from CMake, Ninja, ESP-IDF component graphs (not project-level custom commands).
# Note: this also suppresses user-defined custom targets beginning with ``lib``.
HELP_PHONY_NAME_PREFIXES: tuple[str, ...] = ('_', 'cmake_', 'gen_', 'lib', 'esp-idf')

# File-like / glue targets (``*_args``, ``.cmake``, etc.) that ``help`` lists but are not interactive commands.
HELP_PHONY_NAME_SUFFIXES: tuple[str, ...] = (
    '_args',
    '_bin',
    '_preprocess',
    '_table',
    '.cmake',
    '.txt',
    '.json',
    '.ld',
    '.a',
    '.in',
    '.ninja',
)

# Doc and partition-table helper phonys often register as top-level names; hide by substring, not exact match only.
HELP_PHONY_NAME_SUBSTRINGS: tuple[str, ...] = ('apidoc', 'partition_table')

IDF_PY_HELP_SKIP_TARGETS = frozenset(
    {
        # Generic clean/rule placeholder from Make/Ninja help output, not a project custom target.
        'C',
        'app_check_size',
        'builtin',
        'custom_bundle',
        'edit_cache',
        'encrypted-bootloader-flash',
        'encrypted-partition-table-flash',
        'everest',
        'help',
        'install',
        'lib',
        'list_install_components',
        'mbedcrypto',
        'mbedtls',
        'mbedx509',
        'p256-m',
        'rebuild_cache',
        'tfpsacrypto',
        'extras',
        'platform',
        'pqcp',
        'utilities',
    }
)


def help_phony_name_passes_shape_policy(name: str) -> bool:
    """True if ``name`` passes path / prefix / suffix / substring filters (no ``idf.py`` action or exact-skip check)."""
    if not name or '/' in name or '\\' in name:
        return False
    # Defensive: Ninja/CMake help text can contain tokens that are not real target names (e.g. separators like "|").
    # Target names in this section are expected to be single, whitespace-free identifiers.
    if any(c.isspace() for c in name) or '|' in name:
        return False
    if name.startswith(HELP_PHONY_NAME_PREFIXES):
        return False
    if name.endswith(HELP_PHONY_NAME_SUFFIXES):
        return False
    if any(s in name for s in HELP_PHONY_NAME_SUBSTRINGS):
        return False
    return True


def strip_cmake_help_listing_quotes(line: str) -> str:
    """CMake/Make ``--target help`` sometimes wraps a target in matching quotes; unwrap one layer."""
    if len(line) >= 2 and line[0] in '\'"' and line[-1] == line[0]:
        return line[1:-1]
    return line


def split_ninja_build_outputs(outputs: str) -> list[str]:
    """Split a Ninja `build` output list (may be multi-output) into individual target names."""
    return [n for n in outputs.split() if n]


def should_list_custom_target(name: str, defined: Set[str]) -> bool:
    """Return True if ``name`` should be shown as a CMake custom target in ``idf.py --help``."""
    if not help_phony_name_passes_shape_policy(name):
        return False
    if name in defined or name in IDF_PY_HELP_SKIP_TARGETS:
        return False
    return True
