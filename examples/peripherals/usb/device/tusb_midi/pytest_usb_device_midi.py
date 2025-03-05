# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.temp_skip_ci(targets=['esp32s3'], reason='lack of runners with usb_device tag')
@pytest.mark.usb_device
@idf_parametrize('target', ['esp32s2', 'esp32s3', 'esp32p4'], indirect=['target'])
def test_usb_device_midi_example(dut: Dut) -> None:
    dut.expect_exact('USB initialization DONE')
    dut.expect_exact('MIDI write task init')
    dut.expect_exact('MIDI read task init')
    dut.expect_exact('Writing MIDI data 74')
