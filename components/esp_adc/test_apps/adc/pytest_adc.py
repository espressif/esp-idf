# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.adc
@pytest.mark.parametrize('config', ['iram_safe', 'release', 'pm_enable'], indirect=True)
@idf_parametrize(
    'target',
    ['esp32', 'esp32s2', 'esp32s3', 'esp32c3', 'esp32c6', 'esp32h2', 'esp32c5', 'esp32p4', 'esp32c61'],
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


# P4 REV2 adc
@pytest.mark.adc
@pytest.mark.esp32p4_eco4
@pytest.mark.parametrize('config', ['esp32p4_eco4'], indirect=True)
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration # TODO: IDF-14357')
@idf_parametrize(
    'target',
    ['esp32p4'],
    indirect=['target'],
)
def test_adc_p4_rev2(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120, reset=True)
