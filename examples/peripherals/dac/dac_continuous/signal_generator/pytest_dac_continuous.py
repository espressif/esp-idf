# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


def test_dac_continuous_output(dut: Dut, mode: str, chan0_io: str, chan1_io: str) -> None:
    dut.expect('dac continuous: --------------------------------------------------', timeout=10)
    dut.expect('dac continuous: DAC continuous output by {}'.format(mode), timeout=10)
    dut.expect('dac continuous: DAC channel 0 io: GPIO_NUM_{}'.format(chan0_io), timeout=10)
    dut.expect('dac continuous: DAC channel 1 io: GPIO_NUM_{}'.format(chan1_io), timeout=10)
    dut.expect('dac continuous: Waveform: SINE -> TRIANGLE -> SAWTOOTH -> SQUARE', timeout=10)
    dut.expect('dac continuous: DAC conversion frequency \\(Hz\\): ([0-9]+)', timeout=10)
    dut.expect('dac continuous: DAC wave frequency \\(Hz\\): ([0-9]+)', timeout=10)
    dut.expect('dac continuous: --------------------------------------------------', timeout=10)
    dut.expect(r'DAC channel 0 value:( +)(\d+)(.*)DAC channel 1 value:( +)(\d+)', timeout=10)
    dut.expect(r'dac continuous\({}\): triangle wave start'.format(mode), timeout=20)
    dut.expect(r'dac continuous\({}\): sawtooth wave start'.format(mode), timeout=20)
    dut.expect(r'dac continuous\({}\): square wave start'.format(mode), timeout=20)
    dut.expect(r'dac continuous\({}\): sine wave start'.format(mode), timeout=20)


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'dma',
        'timer',
    ],
    indirect=True,
)
def test_dac_continuous_example_with_dma(dut: Dut) -> None:
    sdkconfig = dut.app.sdkconfig
    if dut.target == 'esp32':
        if sdkconfig['EXAMPLE_DAC_CONTINUOUS_BY_DMA']:
            test_dac_continuous_output(dut, 'DMA', '25', '26')
        else:
            test_dac_continuous_output(dut, 'timer', '25', '26')
    elif dut.target == 'esp32s2':
        if sdkconfig['EXAMPLE_DAC_CONTINUOUS_BY_DMA']:
            test_dac_continuous_output(dut, 'DMA', '17', '18')
        else:
            test_dac_continuous_output(dut, 'timer', '17', '18')
