# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.generic
def test_dac_audio_exampl(dut: Dut) -> None:

    dut.expect('I \\(([0-9]+)\\) dac audio: DAC audio example start', timeout=10)
    dut.expect('I \\(([0-9]+)\\) dac audio: --------------------------------------', timeout=5)
    dut.expect('I \\(([0-9]+)\\) dac audio: DAC initialized success, DAC DMA is ready', timeout=5)
    dut.expect('I \\(([0-9]+)\\) dac audio: Audio size ([0-9]+) bytes, played at frequency ([0-9]+) Hz', timeout=5)
