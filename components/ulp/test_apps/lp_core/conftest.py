# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
"""Fixtures picked up by every test app under ``test_apps/lp_core``."""

import sys
from collections.abc import Callable
from pathlib import Path
from typing import Any

import pytest
from pytest_embedded_idf import CaseTester
from pytest_embedded_idf import UnittestMenuCase

_LP_CORE_TESTS = str(Path(__file__).resolve().parent)
if _LP_CORE_TESTS not in sys.path:
    sys.path.insert(0, _LP_CORE_TESTS)

from lp_unity import TwoBoardSleep  # noqa: E402
from lp_unity import UnityBoard  # noqa: E402
from lp_unity import find_menu_case  # noqa: E402


@pytest.fixture
def unity_board(case_tester: CaseTester) -> Callable[..., UnityBoard]:
    """Return ``UnityBoard`` for one DUT. Index 0 is the first device."""

    def open_board(index: int = 0, timeout: float = 120.0) -> UnityBoard:
        return UnityBoard(case_tester.dut[index], timeout=timeout)

    return open_board


@pytest.fixture
def menu_case(case_tester: CaseTester) -> Callable[..., UnittestMenuCase]:
    """Look up the single Unity case of a given type and group."""

    def lookup(case_type: str, group: str, name_fragment: str | None = None) -> UnittestMenuCase:
        return find_menu_case(case_tester, case_type, group, name_fragment=name_fragment)

    return lookup


@pytest.fixture
def two_board_sleep(case_tester: CaseTester) -> Callable[..., TwoBoardSleep]:
    """Open a sleeper/peer pair for one Unity group. See ``TwoBoardSleep``."""

    def open_group(group: str, **kwargs: Any) -> TwoBoardSleep:
        return TwoBoardSleep(case_tester, group, **kwargs)

    return open_group
