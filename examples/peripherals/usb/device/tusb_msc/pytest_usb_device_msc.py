# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s2
@pytest.mark.usb_device
def test_usb_device_msc_example(dut: Dut) -> None:
    dut.expect('Mount storage')
    dut.expect('TinyUSB Driver installed')
    dut.expect('USB MSC initialization DONE')
    dut.write(' help')
    dut.expect('read')
    dut.expect('write')
    dut.expect('size')
    dut.expect('expose')
    dut.expect('status')
    dut.write(' status')
    dut.expect('storage exposed over USB')
