# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_i2s_adc_dac(dut: Dut) -> None:
    #  ADC_DMA_read test takes more than 30 sec
    dut.run_all_single_board_cases(timeout=60)
