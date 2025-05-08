# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.usb_device
@pytest.mark.parametrize(
    'port, flash_port, config',
    [
        pytest.param('/dev/serial_ports/ttyACM-esp32', '/dev/serial_ports/ttyUSB-esp32', 'release'),
    ],
    indirect=True,
)
@pytest.mark.parametrize('test_message', ['test123456789!@#%^&*'])
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_usb_cdc_vfs_default(dut: Dut, test_message: str) -> None:
    # test run: test_usb_cdc_select
    dut.expect_exact('test_usb_cdc_select', timeout=2)
    dut.expect_exact('select timed out', timeout=2)
    dut.write(test_message)
    dut.expect_exact(test_message, timeout=2)

    # test run: test_usb_cdc_read_non_blocking
    dut.expect_exact('test_usb_cdc_read_non_blocking', timeout=2)
    dut.expect_exact('send_bytes', timeout=2)
    dut.write('abcdefgh')
