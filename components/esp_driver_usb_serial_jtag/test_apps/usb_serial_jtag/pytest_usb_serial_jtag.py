# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.usb_serial_jtag
@pytest.mark.parametrize(
    'port, flash_port, config',
    [
        pytest.param('/dev/serial_ports/ttyACM-esp32', '/dev/serial_ports/ttyUSB-esp32', 'release'),
    ],
    indirect=True,
)
def test_usb_serial_jtag_dev(dut: Dut) -> None:                # type: ignore
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('\"test print via usb_serial_jtag driver multiple times in different tasks\"')
    for i in range(300 * 2):
        dut.expect(r'Oh, hello world (\d), this test is for testing message and parse in python, time (\d+)', timeout=10)
    dut.expect('PASS')
    dut.expect_exact('Enter next test, or \'enter\' to see menu')
    dut.write('\"see if fsync appears to work\"')
    dut.expect('PASS')


@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.usb_serial_jtag
@pytest.mark.parametrize(
    'port, flash_port, config',
    [
        pytest.param('/dev/serial_ports/ttyACM-esp32', '/dev/serial_ports/ttyUSB-esp32', 'release'),
    ],
    indirect=True,
)
def test_usb_serial_jtag_rom_dev(dut: Dut) -> None:                # type: ignore
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('\"test rom printf work after driver installed\"')
    dut.expect(r'hi, espressif1', timeout=10)
    dut.expect(r'hi, espressif2', timeout=10)
