# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
        'release',
    ],
    indirect=True,
)
@idf_parametrize('target', soc_filtered_targets('SOC_PCNT_SUPPORTED == 1'), indirect=['target'])
def test_pulse_cnt(dut: Dut) -> None:
    dut.run_all_single_board_cases()
