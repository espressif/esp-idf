# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded.dut import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.adc
@idf_parametrize(
    'target',
    ['esp32', 'esp32s2', 'esp32s3', 'esp32c3', 'esp32c6', 'esp32h2', 'esp32c5', 'esp32p4', 'esp32c61'],
    indirect=['target'],
)
def test_adc_oneshot(dut: Dut) -> None:
    dut.expect(r'EXAMPLE: ADC1 Channel\[(\d+)\] Raw Data: (\d+)', timeout=5)


@pytest.mark.adc
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'config, baud',
    [
        ('esp32c2_xtal26m', '74880'),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c2'], indirect=['target'])
def test_adc_oneshot_esp32c2_xtal_26mhz(dut: Dut) -> None:
    dut.expect(r'EXAMPLE: ADC1 Channel\[(\d+)\] Raw Data: (\d+)', timeout=5)
