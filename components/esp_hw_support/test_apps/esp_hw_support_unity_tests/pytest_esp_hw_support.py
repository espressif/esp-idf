# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.temp_skip_ci(targets=['esp32c5'], reason='c5 eco2 does not support top pd')
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
    [('default', 'esp32c5')],
    indirect=['config', 'target'],
)
def test_esp_hw_support_esp32c5(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=180)


@pytest.mark.generic
@pytest.mark.esp32c5_eco3
@idf_parametrize(
    'config,target',
    [('release', 'esp32c5')],
    indirect=['config', 'target'],
)
def test_esp_hw_support_esp32c5_eco3(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=180)
