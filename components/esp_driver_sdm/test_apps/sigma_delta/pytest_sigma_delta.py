# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets

CONFIGS = [
    'iram_safe',
    'release',
]


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@idf_parametrize(
    'target',
    soc_filtered_targets('SOC_SDM_SUPPORTED == 1'),
    indirect=['target'],
)
def test_sdm(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='sdm')
