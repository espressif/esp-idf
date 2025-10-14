# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


def dac_example_expect(dut: Dut, mode: str) -> None:
    dut.expect('dac_audio: DAC audio example start', timeout=10)
    dut.expect('dac_audio: --------------------------------------', timeout=5)
    dut.expect('dac_audio: DAC initialized success, DAC DMA is ready', timeout=5)
    dut.expect('dac_audio: Audio size 95824 bytes, played at frequency 48000 Hz ' + mode, timeout=5)
    dut.expect('Play count: 2', timeout=10)


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'sync',
        'async',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32', 'esp32s2'], indirect=['target'])
def test_dac_audio_example(dut: Dut) -> None:
    sdkconfig = dut.app.sdkconfig
    if sdkconfig['EXAMPLE_DAC_WRITE_SYNC']:
        dac_example_expect(dut, 'synchronously')
    else:
        dac_example_expect(dut, 'asynchronously')
