# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


def do_test_quit(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"esp console repl test"')

    dut.expect_exact('esp>', timeout=5)
    dut.write('quit')

    dut.expect_exact('ByeBye', timeout=5)


def do_test_help_generic(dut: Dut, registration_order: str) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"esp console help command - {} registration"'.format(registration_order))

    dut.expect_exact('esp>', timeout=5)
    dut.write('help')

    dut.expect_exact('aaa', timeout=5)
    dut.expect_exact('should appear first in help', timeout=5)

    dut.expect(r'help\s+\[<string>\]', timeout=5)

    # Note: repl seems to do the line breaks by itself, this needs to be adjusted if repl changes its line width
    dut.expect_exact('Print the summary of all registered commands if no arguments are given,', timeout=5)
    dut.expect_exact('otherwise print summary of given command.', timeout=5)
    dut.expect(r'<string>\s+Name of command', timeout=5)

    dut.expect_exact('quit', timeout=5)
    dut.expect_exact('Quit REPL environment', timeout=5)

    dut.expect_exact('zzz', timeout=5)
    dut.expect_exact('should appear last in help', timeout=5)
    dut.expect_exact('esp>', timeout=5)


def do_test_help_quit(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"esp console help command - sorted registration"')

    dut.expect_exact('esp>', timeout=5)
    dut.write('help quit')

    dut.expect(r'quit\s+Quit REPL environment\s+esp>', timeout=5)


@pytest.mark.parametrize(
    'config', [
        pytest.param('defaults'),
    ]
)
@pytest.mark.parametrize(
    'test_on', [
        pytest.param('host', marks=[pytest.mark.linux, pytest.mark.host_test]),
        pytest.param('target', marks=[pytest.mark.esp32, pytest.mark.generic]),
        pytest.param('target', marks=[pytest.mark.esp32c3, pytest.mark.generic]),
        pytest.param('qemu', marks=[pytest.mark.esp32, pytest.mark.host_test, pytest.mark.qemu]),
    ]
)
def test_console(dut: Dut, test_on: str) -> None:
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('![ignore]')
    dut.expect(r'\d{1} Tests 0 Failures 0 Ignored', timeout=120)


@pytest.mark.parametrize(
    'config', [
        pytest.param('defaults'),
    ]
)
@pytest.mark.parametrize(
    'test_on', [
        pytest.param('host', marks=[pytest.mark.linux, pytest.mark.host_test]),
        pytest.param('target', marks=[pytest.mark.esp32, pytest.mark.generic]),
        pytest.param('target', marks=[pytest.mark.esp32c3, pytest.mark.generic]),
        pytest.param('qemu', marks=[pytest.mark.esp32, pytest.mark.host_test, pytest.mark.qemu]),
    ]
)
def test_console_repl(dut: Dut, test_on: str) -> None:
    do_test_quit(dut)


@pytest.mark.parametrize(
    'config', [
        pytest.param('defaults'),
    ]
)
@pytest.mark.parametrize(
    'test_on', [
        pytest.param('host', marks=[pytest.mark.linux, pytest.mark.host_test]),
        pytest.param('target', marks=[pytest.mark.esp32, pytest.mark.generic]),
        pytest.param('target', marks=[pytest.mark.esp32c3, pytest.mark.generic]),
        pytest.param('qemu', marks=[pytest.mark.esp32, pytest.mark.host_test, pytest.mark.qemu]),
    ]
)
def test_console_help_sorted_registration(dut: Dut, test_on: str) -> None:
    do_test_help_generic(dut, 'sorted')


@pytest.mark.parametrize(
    'config', [
        pytest.param('defaults'),
    ]
)
@pytest.mark.parametrize(
    'test_on', [
        pytest.param('host', marks=[pytest.mark.linux, pytest.mark.host_test]),
        pytest.param('target', marks=[pytest.mark.esp32, pytest.mark.generic]),
        pytest.param('target', marks=[pytest.mark.esp32c3, pytest.mark.generic]),
        pytest.param('qemu', marks=[pytest.mark.esp32, pytest.mark.host_test, pytest.mark.qemu]),
    ]
)
def test_console_help_reverse_registration(dut: Dut, test_on: str) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"esp console help command - reverse registration"')

    dut.expect_exact('esp>', timeout=5)
    dut.write('help')

    dut.expect_exact('zzz', timeout=5)
    dut.expect_exact('should appear last in help', timeout=5)

    dut.expect_exact('quit', timeout=5)
    dut.expect_exact('Quit REPL environment', timeout=5)

    dut.expect(r'help\s+\[<string>\]', timeout=5)

    # Note: repl seems to do the line breaks by itself, this needs to be adjusted if repl changes its line width
    dut.expect_exact('Print the summary of all registered commands if no arguments are given,', timeout=5)
    dut.expect_exact('otherwise print summary of given command.', timeout=5)
    dut.expect(r'<string>\s+Name of command', timeout=5)

    dut.expect_exact('aaa', timeout=5)
    dut.expect_exact('should appear first in help', timeout=5)
    dut.expect_exact('esp>', timeout=5)


@pytest.mark.parametrize(
    'config', [
        pytest.param('sorted'),
    ]
)
@pytest.mark.parametrize(
    'test_on', [
        pytest.param('host', marks=[pytest.mark.linux, pytest.mark.host_test]),
    ]
)
def test_console_sorted_help_sorted_registration(dut: Dut, test_on: str) -> None:
    do_test_help_generic(dut, 'sorted')


@pytest.mark.parametrize(
    'config', [
        pytest.param('sorted', marks=[pytest.mark.linux, pytest.mark.host_test]),
    ]
)
@pytest.mark.parametrize(
    'test_on', [
        pytest.param('host', marks=[pytest.mark.linux, pytest.mark.host_test]),
    ]
)
def test_console_sorted_help_reverse_registration(dut: Dut, test_on: str) -> None:
    do_test_help_generic(dut, 'reverse')


@pytest.mark.parametrize(
    'config', [
        pytest.param('defaults'),
    ]
)
@pytest.mark.parametrize(
    'test_on', [
        pytest.param('host', marks=[pytest.mark.linux, pytest.mark.host_test]),
        pytest.param('target', marks=[pytest.mark.esp32, pytest.mark.generic]),
        pytest.param('target', marks=[pytest.mark.esp32c3, pytest.mark.generic]),
        pytest.param('qemu', marks=[pytest.mark.esp32, pytest.mark.host_test, pytest.mark.qemu]),
    ]
)
def test_console_help_quit(dut: Dut, test_on: str) -> None:
    do_test_help_quit(dut)
