# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_llvm_memcpy_opt(dut: Dut) -> None:
    """Board smoke: correctness PASS. Speedup needs Clang + menuconfig option."""
    dut.expect(r'LLVM memcpy optimization: (enabled|disabled)', timeout=60)
    dut.expect(r'Baseline cycles:\s+\d+', timeout=30)
    dut.expect(r'Optimized cycles:\s+\d+', timeout=30)
    dut.expect_exact('Result: PASS', timeout=30)
