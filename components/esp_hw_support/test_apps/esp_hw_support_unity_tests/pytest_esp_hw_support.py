# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize(
    'config,target',
    [('single_core_esp32', 'esp32'), ('default', 'supported_targets'), ('release', 'supported_targets')],
    indirect=['config', 'target'],
)
def test_esp_hw_support(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=180)


@pytest.mark.generic
@idf_parametrize(
    'config,target',
    [('pvt', target) for target in soc_filtered_targets('SOC_PMU_PVT_SUPPORTED == 1')],
    indirect=['config', 'target'],
)
def test_esp_hw_support_pvt(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='pvt', timeout=180)
