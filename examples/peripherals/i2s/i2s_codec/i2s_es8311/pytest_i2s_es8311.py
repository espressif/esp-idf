# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize(
    'target',
    ['esp32', 'esp32s2', 'esp32s3', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32h2', 'esp32p4', 'esp32c61'],
    indirect=['target'],
)
def test_i2s_es8311_example_generic(dut: Dut) -> None:
    dut.expect('i2s es8311 codec example start')
    dut.expect('-----------------------------')
    dut.expect('i2s_es8311: i2s driver init success')


@pytest.mark.es8311
@pytest.mark.parametrize(
    'config',
    [
        's3_korvo2_v3',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_i2s_es8311_example_korvo2_v3(dut: Dut) -> None:
    dut.expect(r'i2s es8311 codec example start')
    dut.expect(r'-----------------------------')
    dut.expect(r'i2s_es8311: i2s driver init success')
    dut.expect(r'i2s_es8311: es8311 codec init success')
    dut.expect(r'Returned from app_main\(\)')
