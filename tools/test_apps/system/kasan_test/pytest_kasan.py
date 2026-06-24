# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
"""
Pytest test cases for the KASAN Unity test application.

Two configurations:
  - no_halt: all tests run in one boot cycle (CONFIG_KASAN_NO_HALT=y).
             The Unity runner executes every test; each verifies the
             KASAN error count.
  - halt:    each error test triggers an abort.  pytest selects one test
             at a time via the Unity menu, expecting a panic.
"""

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

# ---------------------------------------------------------------------------
# no_halt configuration: all tests pass in one run
# ---------------------------------------------------------------------------


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets', 'preview_targets'], indirect=['target'])
@pytest.mark.parametrize('config', ['no_halt'], indirect=True)
def test_kasan_no_halt_all(dut: Dut) -> None:
    """Run all KASAN tests in one boot cycle with CONFIG_KASAN_NO_HALT=y."""
    dut.expect('KASAN test application starting', timeout=15)
    # Send '*' to Unity menu to run all tests
    dut.write('*')
    dut.expect(r'\d+ Tests \d+ Failures \d+ Ignored', timeout=45)


# ---------------------------------------------------------------------------
# halt configuration: each error test causes an abort
# ---------------------------------------------------------------------------


def _run_halt_test(dut: Dut, test_name: str) -> None:
    """Select a test from Unity menu and expect KASAN abort."""
    dut.expect('KASAN test application starting', timeout=15)
    dut.write('"' + test_name + '"')
    dut.expect(r'KASAN error: (WRITE|READ) of size \d+ at 0x', timeout=20)


@pytest.mark.generic
@pytest.mark.timeout(120)
@idf_parametrize('target', ['supported_targets', 'preview_targets'], indirect=['target'])
@pytest.mark.parametrize('config', ['halt'], indirect=True)
def test_kasan_halt_overflow(dut: Dut) -> None:
    _run_halt_test(dut, 'heap buffer overflow')


@pytest.mark.generic
@pytest.mark.timeout(120)
@idf_parametrize('target', ['supported_targets', 'preview_targets'], indirect=['target'])
@pytest.mark.parametrize('config', ['halt'], indirect=True)
def test_kasan_halt_uaf_write(dut: Dut) -> None:
    _run_halt_test(dut, 'use-after-free write')


@pytest.mark.generic
@pytest.mark.timeout(120)
@idf_parametrize('target', ['supported_targets', 'preview_targets'], indirect=['target'])
@pytest.mark.parametrize('config', ['halt'], indirect=True)
def test_kasan_halt_uaf_read(dut: Dut) -> None:
    _run_halt_test(dut, 'use-after-free read')


@pytest.mark.generic
@pytest.mark.timeout(120)
@idf_parametrize('target', ['supported_targets', 'preview_targets'], indirect=['target'])
@pytest.mark.parametrize('config', ['halt'], indirect=True)
def test_kasan_halt_underflow(dut: Dut) -> None:
    _run_halt_test(dut, 'heap buffer underflow')


@pytest.mark.generic
@pytest.mark.timeout(120)
@idf_parametrize('target', ['supported_targets', 'preview_targets'], indirect=['target'])
@pytest.mark.parametrize('config', ['halt'], indirect=True)
def test_kasan_halt_large_overflow(dut: Dut) -> None:
    _run_halt_test(dut, 'large heap overflow')


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets', 'preview_targets'], indirect=['target'])
@pytest.mark.parametrize('config', ['halt'], indirect=True)
def test_kasan_halt_no_false_positive(dut: Dut) -> None:
    """No-bug test should complete without KASAN error."""
    dut.expect('KASAN test application starting', timeout=15)
    dut.write('"no false positive"')
    dut.expect('no-bug test PASSED', timeout=15)


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets', 'preview_targets'], indirect=['target'])
@pytest.mark.parametrize('config', ['halt'], indirect=True)
def test_kasan_halt_asan_stubs_valid_access(dut: Dut) -> None:
    """Direct calls to every sized __asan_*_noabort stub on a valid buffer
    must link and run without raising an error."""
    dut.expect('KASAN test application starting', timeout=15)
    dut.write('"asan stubs valid access no error"')
    dut.expect('asan stubs valid-access test PASSED', timeout=15)
