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
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_usb_cdc_vfs_default(dut: Dut) -> None:
    test_message = 'test123456789!@#%^&*'

    # test run: test_usb_cdc_select
    dut.expect_exact('test_usb_cdc_select', timeout=2)
    dut.expect_exact('select timed out', timeout=2)
    dut.write(test_message)
    dut.expect_exact(test_message, timeout=2)

    # test run: test_usb_cdc_read_non_blocking
    dut.expect_exact('test_usb_cdc_read_non_blocking', timeout=2)
    dut.expect_exact('send_bytes', timeout=2)
    dut.write('abcdefgh')

    # test run: test_usb_cdc_read_blocking
    dut.expect_exact('test_usb_cdc_read_blocking', timeout=2)
    dut.expect_exact('ready to receive', timeout=2)
    dut.write('testdata')

    # test run: test_usb_cdc_read_no_exit_on_newline_reception
    dut.expect_exact('test_usb_cdc_read_no_exit_on_newline_reception', timeout=2)
    dut.expect_exact('ready to receive', timeout=2)
    dut.write('!(@*#&(!*@&#((SDasdkjhad\nce')

    # test run: test_usb_cdc_ets_printf_cache_disabled
    dut.expect_exact('test_usb_cdc_ets_printf_cache_disabled', timeout=2)
    dut.expect_exact('test_message', timeout=2)
    dut.expect_exact('successful test', timeout=2)
