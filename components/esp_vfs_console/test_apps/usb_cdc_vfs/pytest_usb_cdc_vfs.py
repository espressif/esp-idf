# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s3
@pytest.mark.usb_device
@pytest.mark.parametrize(
    'port, flash_port, config',
    [
        pytest.param('/dev/serial_ports/ttyACM-esp32', '/dev/serial_ports/ttyUSB-esp32', 'release'),
    ],
    indirect=True,)
@pytest.mark.parametrize('test_message', ['test123456789!@#%^&*'])
def test_usb_cdc_vfs_default(dut: Dut, test_message: str) -> None:
    # test run: test_usb_cdc_read_non_blocking
    dut.expect_exact('test_usb_cdc_read_non_blocking', timeout=2)
    dut.expect_exact('send_bytes', timeout=2)
    dut.write('abcdefgh')
