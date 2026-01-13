# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize(
    'config,target',
    [
        ('single_core_esp32', 'esp32'),
        *(('default', target) for target in soc_filtered_targets('IDF_TARGET not in ["esp32c5"]')),
        *(('release', target) for target in soc_filtered_targets('IDF_TARGET not in ["esp32c5"]')),
    ],
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
