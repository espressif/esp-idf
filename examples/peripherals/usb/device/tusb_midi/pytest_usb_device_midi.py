# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s2
@pytest.mark.usb_device
def test_usb_device_midi_example(dut: Dut) -> None:
    dut.expect_exact('USB initialization DONE')
    dut.expect_exact('MIDI write task init')
    dut.expect_exact('MIDI read task init')
    dut.expect_exact('Writing MIDI data 74')
