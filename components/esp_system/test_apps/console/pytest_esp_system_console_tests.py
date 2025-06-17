# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pexpect
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


def not_expect(dut: Dut, output_regex: str) -> None:
    try:
        dut.expect(output_regex, timeout=5)
    except pexpect.TIMEOUT:
        pass
    else:
        raise RuntimeError(f'Found not_expect output {output_regex}')


JTAG_SERIAL_MARKS = [
    pytest.mark.esp32s3,
    pytest.mark.esp32c2,
    pytest.mark.esp32c3,
    pytest.mark.esp32c5,
    pytest.mark.esp32c6,
    pytest.mark.esp32c61,
    pytest.mark.esp32h2,
]


@pytest.mark.generic
@idf_parametrize('config', ['console_none'], indirect=['config'])
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_esp_system_console_no_output_uart(dut: Dut) -> None:
    not_expect(dut, r'2nd stage bootloader|Hello World')
    dut.expect('This message will be printed even with CONFIG_ESP_CONSOLE_NONE')


@pytest.mark.usb_serial_jtag
@idf_parametrize('config', ['serial_jtag_only'], indirect=['config'])
@idf_parametrize('flash_port', ['/dev/serial_ports/ttyUSB-esp32'], indirect=['flash_port'])
@idf_parametrize('port', ['/dev/serial_ports/ttyACM-esp32'], indirect=['port'])
@idf_parametrize('target', ['esp32c2', 'esp32c3', 'esp32c6', 'esp32h2', 'esp32s3'], indirect=['target'])
def test_esp_system_console_only_serial_jtag(dut: Dut) -> None:
    dut.expect('2nd stage bootloader')
    dut.expect('Hello World')
    dut.expect('Opening /dev/console')
    dut.expect('This should be printed to stdout')
    dut.expect('Closing /dev/console')
    dut.expect('This should be printed to stdout')


@pytest.mark.usb_serial_jtag
@idf_parametrize('config', ['serial_jtag_only_no_vfs'], indirect=['config'])
@idf_parametrize('flash_port', ['/dev/serial_ports/ttyUSB-esp32'], indirect=['flash_port'])
@idf_parametrize('port', ['/dev/serial_ports/ttyACM-esp32'], indirect=['port'])
@idf_parametrize('target', ['esp32c2', 'esp32c3', 'esp32c6', 'esp32h2', 'esp32s3'], indirect=['target'])
def test_esp_system_console_only_serial_jtag_no_vfs(dut: Dut) -> None:
    dut.expect('2nd stage bootloader')
    dut.expect('Hello World')


@pytest.mark.usb_serial_jtag
@idf_parametrize('config', ['console_no_vfs'], indirect=['config'])
@idf_parametrize('flash_port', ['/dev/serial_ports/ttyUSB-esp32'], indirect=['flash_port'])
@idf_parametrize('port', ['/dev/serial_ports/ttyACM-esp32'], indirect=['port'])
@idf_parametrize('target', ['esp32c2', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32h2', 'esp32s3'], indirect=['target'])
def test_esp_system_console_secondary_serial_jtag(dut: Dut) -> None:
    dut.expect('2nd stage bootloader')
    dut.expect('Hello World')


@pytest.mark.generic
@idf_parametrize('config', ['simple'], indirect=['config'])
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_esp_system_console_correct_open_and_close(dut: Dut) -> None:
    dut.expect('2nd stage bootloader')
    dut.expect('Hello World')
    dut.expect('Opening /dev/console')
    dut.expect('This should be printed to stdout')
    dut.expect('Closing /dev/console')
    dut.expect('This should be printed to stdout')
