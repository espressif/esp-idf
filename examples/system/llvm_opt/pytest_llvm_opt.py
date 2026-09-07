# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_llvm_opt(dut: Dut) -> None:
    dut.expect(r'LLVM optimization for ESP32-P4 memcpy:', timeout=30)
    dut.expect_exact('This component is built with ENABLE_LLVM_OPT.', timeout=10)
