# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.generic
def test_dac_cosine_wave_example_with_12bit_adc(dut: Dut) -> None:
    res = []
    for _ in range(20):
        res.append(dut.expect(r'DAC channel 0 vaule:( +)(\d+)(.*)DAC channel 1 vaule:( +)(\d+)', timeout=10))

    chan1_val = []
    for val in res:
        chan1_val.append(int(val.group(2)))

    assert min(chan1_val) < 300
    assert max(chan1_val) > 3500


@pytest.mark.esp32s2
@pytest.mark.generic
def test_dac_cosine_wave_example_with_13bit_adc(dut: Dut) -> None:
    res = []
    for _ in range(20):
        res.append(dut.expect(r'DAC channel 0 vaule:( +)(\d+)(.*)DAC channel 1 vaule:( +)(\d+)', timeout=10))

    chan1_val = []
    for val in res:
        chan1_val.append(int(val.group(2)))

    assert min(chan1_val) < 600
    assert max(chan1_val) > 7000
