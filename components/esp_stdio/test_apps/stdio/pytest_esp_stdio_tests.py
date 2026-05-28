# SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
#
# HW runs: use --embedded-services esp,idf (not idf alone) so serial is attached.
# See README.md in this directory for QEMU vs hardware commands and common failures.
#
import pexpect
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


def _expect_app_main_banner(dut: Dut) -> None:
    """Sync to the latest app boot (avoids matching a stale 'Hello World' left in the serial buffer)."""
    dut.expect('Calling app_main()')


def not_expect(dut: Dut, output_regex: str) -> None:
    try:
        dut.expect(output_regex, timeout=5)
    except pexpect.TIMEOUT:
        pass
    else:
        raise RuntimeError(f'Found not_expect output {output_regex}')


@pytest.mark.generic
@idf_parametrize('config', ['stdio_none'], indirect=['config'])
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_esp_system_stdio_no_output_uart(dut: Dut) -> None:
    not_expect(dut, r'2nd stage bootloader|Hello World')
    dut.expect('This message will be printed even with CONFIG_ESP_CONSOLE_NONE')


@pytest.mark.usb_serial_jtag
@idf_parametrize('config', ['serial_jtag_only'], indirect=['config'])
@idf_parametrize('flash_port', ['/dev/serial_ports/ttyUSB-esp32'], indirect=['flash_port'])
@idf_parametrize('port', ['/dev/serial_ports/ttyACM-esp32'], indirect=['port'])
@idf_parametrize('target', soc_filtered_targets('SOC_USB_SERIAL_JTAG_SUPPORTED == 1'), indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-10703')
def test_esp_system_stdio_only_serial_jtag(dut: Dut) -> None:
    dut.expect('2nd stage bootloader')
    _expect_app_main_banner(dut)
    dut.expect('Hello World')
    dut.expect('STDIO_TEST:FD_INIT:IN=0,OUT=1,ERR=2')
    dut.expect('STDIO_TEST:STREAM:STDOUT')
    dut.expect('STDIO_TEST:STREAM:STDERR')
    dut.expect('Opening /dev/console')
    dut.expect('This should be printed to stdout')
    dut.expect('Closing /dev/console')
    dut.expect('This should be printed to stdout')


@pytest.mark.usb_serial_jtag
@idf_parametrize('config', ['serial_jtag_only_no_vfs'], indirect=['config'])
@idf_parametrize('flash_port', ['/dev/serial_ports/ttyUSB-esp32'], indirect=['flash_port'])
@idf_parametrize('port', ['/dev/serial_ports/ttyACM-esp32'], indirect=['port'])
@idf_parametrize('target', soc_filtered_targets('SOC_USB_SERIAL_JTAG_SUPPORTED == 1'), indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-10703')
def test_esp_system_stdio_only_serial_jtag_no_vfs(dut: Dut) -> None:
    dut.expect('2nd stage bootloader')
    _expect_app_main_banner(dut)
    dut.expect('Hello World')


@pytest.mark.usb_serial_jtag
@idf_parametrize('config', ['stdio_no_vfs'], indirect=['config'])
@idf_parametrize('flash_port', ['/dev/serial_ports/ttyUSB-esp32'], indirect=['flash_port'])
@idf_parametrize('port', ['/dev/serial_ports/ttyACM-esp32'], indirect=['port'])
@idf_parametrize('target', soc_filtered_targets('SOC_USB_SERIAL_JTAG_SUPPORTED == 1'), indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-10703')
def test_esp_system_stdio_secondary_serial_jtag(dut: Dut) -> None:
    dut.expect('2nd stage bootloader')
    _expect_app_main_banner(dut)
    dut.expect('Hello World')


@pytest.mark.generic
@idf_parametrize('config', ['simple'], indirect=['config'])
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_esp_system_stdio_correct_open_and_close(dut: Dut) -> None:
    dut.expect('2nd stage bootloader')
    _expect_app_main_banner(dut)
    dut.expect('Hello World')
    # Order matches test_app_main.c: stdio_fd_init_check() runs before console_open_close_check()
    dut.expect('STDIO_TEST:FD_INIT:IN=0,OUT=1,ERR=2')
    dut.expect('STDIO_TEST:STREAM:STDOUT')
    dut.expect('STDIO_TEST:STREAM:STDERR')
    dut.expect('STDIO_TEST:SELECT:LOGICAL_FD_MAP_OK')
    dut.expect('Opening /dev/console')
    dut.expect('This should be printed to stdout')
    dut.expect('Closing /dev/console')
    dut.expect('This should be printed to stdout')
    dut.expect('STDIO_TEST:MODE=FORWARDING')
    dut.expect('STDIO_TEST:UNIQUE_FDS_OK')
    dut.expect('STDIO_TEST:WRITE_OK')
    dut.expect('STDIO_TEST:FSYNC_OK')
    dut.expect('STDIO_TEST:CLOSE_OK')


@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.parametrize(
    'config, embedded_services',
    [('simple', 'idf,qemu')],
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_esp_stdio_non_basic_default_qemu(dut: Dut) -> None:
    dut.expect('Hello World')
    dut.expect('STDIO_TEST:FD_INIT:IN=0,OUT=1,ERR=2')
    dut.expect('STDIO_TEST:SELECT:LOGICAL_FD_MAP_OK')
    dut.expect('STDIO_TEST:MODE=FORWARDING')
    dut.expect('STDIO_TEST:UNIQUE_FDS_OK')
    dut.expect('STDIO_TEST:WRITE_OK')
    dut.expect('STDIO_TEST:FSYNC_OK')
    dut.expect('STDIO_TEST:CLOSE_OK')
