# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize(
    'config,target',
    [('single_core_esp32', 'esp32'), ('default', 'supported_targets'), ('release', 'supported_targets')],
    indirect=['config', 'target'],
)
def test_esp_hw_support(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=180)
