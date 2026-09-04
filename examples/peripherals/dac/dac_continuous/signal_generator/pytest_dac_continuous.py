# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


def test_dac_continuous_output(dut: Dut, mode: str) -> None:
    dut.expect(f'signal_generator: DAC continuous output by {mode}', timeout=10)
    dut.expect(r'signal_generator: sine wave start', timeout=20)
    dut.expect(r'signal_generator: triangle wave start', timeout=20)
    dut.expect(r'signal_generator: sawtooth wave start', timeout=20)
    dut.expect(r'signal_generator: square wave start', timeout=20)


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'dma',
        'timer',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32', 'esp32s2'], indirect=['target'])
def test_dac_continuous_example(dut: Dut) -> None:
    sdkconfig = dut.app.sdkconfig
    if sdkconfig['EXAMPLE_DAC_CONTINUOUS_BY_DMA']:
        test_dac_continuous_output(dut, 'DMA')
    else:
        test_dac_continuous_output(dut, 'timer')
