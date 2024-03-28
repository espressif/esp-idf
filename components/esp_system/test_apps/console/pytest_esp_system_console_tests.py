# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pexpect
import pytest
from pytest_embedded import Dut


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
    pytest.mark.esp32c6,
    pytest.mark.esp32h2,
    pytest.mark.esp32p4,
]


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        pytest.param('console_none', marks=pytest.mark.supported_targets),
    ],
    indirect=True
)
def test_esp_system_console_no_output_uart(dut: Dut) -> None:
    not_expect(dut, r'2nd stage bootloader|Hello World')
    dut.expect('This message will be printed even with CONFIG_ESP_CONSOLE_NONE')


@pytest.mark.usb_serial_jtag
@pytest.mark.parametrize(
    'port, config',
    [
        pytest.param('/dev/serial_ports/ttyACM-esp32', 'serial_jtag_only', marks=JTAG_SERIAL_MARKS),
    ],
    indirect=True,
)
def test_esp_system_console_only_serial_jtag(dut: Dut) -> None:
    dut.expect('2nd stage bootloader')
    dut.expect('Hello World')
