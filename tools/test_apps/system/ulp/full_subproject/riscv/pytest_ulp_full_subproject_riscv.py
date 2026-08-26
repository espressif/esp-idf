# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32s2', 'esp32s3'], indirect=['target'])
def test_ulp_full_subproject_riscv(dut: Dut) -> None:
    dut.expect_exact('Not a ULP-RISC-V wakeup, initializing it!')
    dut.expect_exact('Entering deep sleep')
    dut.expect_exact('ULP-RISC-V woke up the main CPU!', timeout=5)
    count = int(dut.expect(r'ULP-RISC-V iteration count:\s+(\d+)', timeout=5).group(1), 10)
    assert count >= 5
    dut.expect_exact('Example finished')
