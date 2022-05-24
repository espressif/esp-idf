# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.generic
def test_dac_continuous_example_with_12bit_adc(dut: Dut) -> None:
    dut.expect('I \\(([0-9]+)\\) dac continuous: --------------------------------------------------', timeout=10)
    dut.expect('I \\(([0-9]+)\\) dac continuous: DAC continuous output by DMA', timeout=10)
    dut.expect('I \\(([0-9]+)\\) dac continuous: DAC channel 0 io: GPIO_NUM_25', timeout=10)
    dut.expect('I \\(([0-9]+)\\) dac continuous: DAC channel 1 io: GPIO_NUM_26', timeout=10)
    dut.expect('I \\(([0-9]+)\\) dac continuous: Waveform: SINE -> TRIANGLE -> SAWTOOTH -> SQUARE', timeout=10)
    dut.expect('I \\(([0-9]+)\\) dac continuous: DAC conversion frequency \\(Hz\\): ([0-9]+)', timeout=10)
    dut.expect('I \\(([0-9]+)\\) dac continuous: DAC wave frequency \\(Hz\\): ([0-9]+)', timeout=10)
    dut.expect('I \\(([0-9]+)\\) dac continuous: --------------------------------------------------', timeout=10)
    dut.expect(r'DAC channel 0 vaule:( +)(\d+)(.*)DAC channel 1 vaule:( +)(\d+)', timeout=10)


@pytest.mark.esp32s2
@pytest.mark.generic
def test_dac_continuous_example_with_13bit_adc(dut: Dut) -> None:
    dut.expect('I \\(([0-9]+)\\) dac continuous: --------------------------------------------------', timeout=10)
    dut.expect('I \\(([0-9]+)\\) dac continuous: DAC continuous output by DMA', timeout=10)
    dut.expect('I \\(([0-9]+)\\) dac continuous: DAC channel 0 io: GPIO_NUM_17', timeout=10)
    dut.expect('I \\(([0-9]+)\\) dac continuous: DAC channel 1 io: GPIO_NUM_18', timeout=10)
    dut.expect('I \\(([0-9]+)\\) dac continuous: Waveform: SINE -> TRIANGLE -> SAWTOOTH -> SQUARE', timeout=10)
    dut.expect('I \\(([0-9]+)\\) dac continuous: DAC conversion frequency \\(Hz\\): ([0-9]+)', timeout=10)
    dut.expect('I \\(([0-9]+)\\) dac continuous: DAC wave frequency \\(Hz\\): ([0-9]+)', timeout=10)
    dut.expect('I \\(([0-9]+)\\) dac continuous: --------------------------------------------------', timeout=10)
    dut.expect(r'DAC channel 0 vaule:( +)(\d+)(.*)DAC channel 1 vaule:( +)(\d+)', timeout=10)
