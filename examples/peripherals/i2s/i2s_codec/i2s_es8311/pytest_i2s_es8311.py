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
    dut.expect(r'ES8311: Work in Slave mode')
    dut.expect(r'I2S_IF: channel mode [0-9]+ bits:[0-9]+/[0-9]+ channel:[0-9]+ mask:[0-9]+')
    dut.expect(r'I2S_IF: STD Mode [0-9]+ bits:[0-9]+/[0-9]+ channel:[0-9]+ sample_rate:[0-9]+ mask:[0-9]+')
    dut.expect(r'I2S_IF: channel mode [0-9]+ bits:[0-9]+/[0-9]+ channel:[0-9]+ mask:[0-9]+')
    dut.expect(r'I2S_IF: STD Mode [0-9]+ bits:[0-9]+/[0-9]+ channel:[0-9]+ sample_rate:[0-9]+ mask:[0-9]+')
    dut.expect(r'Adev_Codec: Open codec device OK')
    dut.expect(r'i2s_es8311: es8311 codec init success')
    dut.expect(r'Returned from app_main\(\)')


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'bsp',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_i2s_es8311_example_bsp(dut: Dut) -> None:
    dut.expect('i2s es8311 codec example start')
    dut.expect('-----------------------------')
    dut.expect('Using BSP for HW configuration')
    dut.expect('i2s_es8311: i2s driver init success')
