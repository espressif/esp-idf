# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.adc
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_dac_cosine_wave_example_with_12bit_adc(dut: Dut) -> None:
    res = []
    for _ in range(30):
        res.append(dut.expect(r'DAC channel 0 value:( +)(\d+)(.*)DAC channel 1 value:( +)(\d+)', timeout=10))

    chan0_val = []
    for val in res:
        chan0_val.append(int(val.group(2)))

    assert max(chan0_val) - min(chan0_val) > 1000


@pytest.mark.adc
@idf_parametrize('target', ['esp32s2'], indirect=['target'])
def test_dac_cosine_wave_example_with_13bit_adc(dut: Dut) -> None:
    res = []
    for _ in range(30):
        res.append(dut.expect(r'DAC channel 0 value:( +)(\d+)(.*)DAC channel 1 value:( +)(\d+)', timeout=10))

    chan0_val = []
    for val in res:
        chan0_val.append(int(val.group(2)))

    assert max(chan0_val) - min(chan0_val) > 3000
