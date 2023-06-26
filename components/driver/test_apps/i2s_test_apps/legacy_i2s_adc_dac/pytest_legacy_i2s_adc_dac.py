# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
def test_i2s_adc_dac(dut: Dut) -> None:
    #  ADC_DMA_read test takes more than 30 sec
    dut.run_all_single_board_cases(timeout=60)
