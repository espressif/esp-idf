# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@pytest.mark.parametrize('config', ['default'], indirect=True)
@idf_parametrize('target', soc_filtered_targets('SOC_RISCV_TRACE_SUPPORTED == 1'), indirect=['target'])
def test_riscv_trace(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize('config', ['psram'], indirect=True)
@idf_parametrize(
    'target', soc_filtered_targets('SOC_RISCV_TRACE_SUPPORTED == 1 and SOC_SPIRAM_SUPPORTED == 1'), indirect=['target']
)
def test_riscv_trace_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases()
