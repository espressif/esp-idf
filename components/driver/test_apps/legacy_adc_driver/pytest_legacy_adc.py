# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.adc
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
def test_legacy_adc(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('*')
    dut.expect_unity_test_output(timeout=240)


# All ESP32C2 ADC runners are 26m xtal
@pytest.mark.esp32c2
@pytest.mark.adc
@pytest.mark.parametrize(
    'config, baud',
    [
        ('esp32c2_xtal26m_release', '74880'),
    ],
    indirect=True,
)
def test_legacy_adc_esp32c2_xtal_26mhz(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('*')
    dut.expect_unity_test_output(timeout=240)
