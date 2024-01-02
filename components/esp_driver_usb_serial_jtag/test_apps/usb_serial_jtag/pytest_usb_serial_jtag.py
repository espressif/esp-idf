# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.usb_serial_jtag
@pytest.mark.parametrize(
    'port, config',
    [
        ('/dev/ttyACM0', 'release'),
    ],
    indirect=True,
)
def test_usb_serial_jtag_dev(dut: Dut) -> None:                # type: ignore
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('\"test print via usb_serial_jtag driver multiple times in different tasks\"')
    for i in range(300 * 2):
        dut.expect(r'Oh, hello world (\d), this test is for testing message and parse in python, time (\d+)', timeout=10)
