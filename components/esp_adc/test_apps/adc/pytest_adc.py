# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.adc
@pytest.mark.parametrize('config', ['iram_safe', 'release', 'pm_enable'], indirect=True)
@idf_parametrize(
    'target',
    ['esp32', 'esp32s2', 'esp32s3', 'esp32c3', 'esp32c6', 'esp32h2', 'esp32c5', 'esp32p4'],
    indirect=['target'],
)
def test_adc(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120, reset=True)


# No PM test, as C2 doesn't support ADC continuous mode
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
@idf_parametrize('target', ['esp32c2'], indirect=['target'])
def test_adc_esp32c2_xtal_26mhz(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120, reset=True)
