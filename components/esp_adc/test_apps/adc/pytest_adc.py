# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.adc
@pytest.mark.parametrize('config', [
    'iram_safe',
    'release',
    'pm_enable'
], indirect=True)
def test_adc(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)


# No PM test, as C2 doesn't support ADC continuous mode
@pytest.mark.esp32c2
@pytest.mark.adc
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'config, baud',
    [
        ('esp32c2_xtal26m_iram_safe', '74880'),
        ('esp32c2_xtal26m_release', '74880'),
    ],
    indirect=True,
)
def test_adc_esp32c2_xtal_26mhz(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)


@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.adc
@pytest.mark.parametrize('config', [
    'gdma_iram_safe',
], indirect=True)
def test_adc_gdma_iram(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)
