# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


def do_test_quit(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.confirm_write('"esp console repl test"', expect_str='esp>')

    dut.confirm_write('quit', expect_str='ByeBye')


def do_test_help_generic(dut: Dut, registration_order: str) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.confirm_write('"esp console help command - {} registration"'.format(registration_order), expect_str='esp>')

    dut.confirm_write('help', expect_str='aaa')

    dut.expect_exact('should appear first in help')

    dut.expect(r'help\s+\[<string>\]')

    # Note: repl seems to do the line breaks by itself, this needs to be adjusted if repl changes its line width
    dut.expect_exact('Print the summary of all registered commands if no arguments are given,')
    dut.expect_exact('otherwise print summary of given command.')
    dut.expect(r'<string>\s+Name of command')

    dut.expect_exact('quit')
    dut.expect_exact('Quit REPL environment')

    dut.expect_exact('zzz')
    dut.expect_exact('should appear last in help')
    dut.expect_exact('esp>')


def do_test_help_quit(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')

    dut.confirm_write('"esp console help command - sorted registration"', expect_str='esp>')

    dut.write('help quit')

    dut.expect(r'quit\s+Quit REPL environment\s+esp>')


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
    dut.run_all_single_board_cases(group='!ignore', timeout=120)


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
    dut.confirm_write('"esp console help command - reverse registration"', expect_str='esp>')

    dut.confirm_write('help', expect_str='zzz')

    dut.expect_exact('should appear last in help')

    dut.expect_exact('quit')
    dut.expect_exact('Quit REPL environment')

    dut.expect(r'help\s+\[<string>\]')

    # Note: repl seems to do the line breaks by itself, this needs to be adjusted if repl changes its line width
    dut.expect_exact('Print the summary of all registered commands if no arguments are given,')
    dut.expect_exact('otherwise print summary of given command.')
    dut.expect(r'<string>\s+Name of command')

    dut.expect_exact('aaa')
    dut.expect_exact('should appear first in help')
    dut.expect_exact('esp>')


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
def test_console_help_verbose_level_0(dut: Dut, test_on: str) -> None:
    help_verbose_info = 'Print the summary of all registered commands if no arguments are given,'
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.confirm_write('"esp console help command - set verbose level = 0"', expect_str='esp>')

    # verify help command
    dut.write('help')
    dut.write('help')
    dut.expect_exact('help', not_matching=help_verbose_info)


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
def test_console_help_verbose_level_1(dut: Dut, test_on: str) -> None:
    help_verbose_info = 'Print the summary of all registered commands if no arguments are given,'
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.confirm_write('"esp console help command - set verbose level = 1"', expect_str='esp>')

    # verify help command
    dut.write('help')
    dut.expect_exact(help_verbose_info)


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
def test_console_help_verbose_subcommand(dut: Dut, test_on: str) -> None:
    help_verbose_info = 'Print the summary of all registered commands if no arguments are given,'
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.confirm_write('"esp console help command - --verbose sub command"', expect_str='esp>')

    # verify help --verbose=0 subcommand
    dut.write('help --verbose=0')
    dut.write('help --verbose=0')
    dut.expect_exact('help --verbose=0',not_matching=help_verbose_info)

    # verify help --verbose=1 subcommand
    dut.write('help --verbose=1')
    dut.expect_exact(help_verbose_info)


@pytest.mark.parametrize(
    'config', [
        pytest.param('defaults'),
    ]
)
@pytest.mark.parametrize(
    'test_on', [
        pytest.param('host', marks=[pytest.mark.linux, pytest.mark.host_test]),
        pytest.param('target', marks=[pytest.mark.esp32, pytest.mark.esp32c3, pytest.mark.generic]),
        pytest.param('qemu', marks=[pytest.mark.esp32, pytest.mark.host_test, pytest.mark.qemu]),
    ]
)
def test_console_help_deregister(dut: Dut, test_on: str) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.confirm_write('"esp console deregister commands"', expect_str='esp>')

    dut.write('help')

    # in the test sequence, command a is registered before registering command z, then
    # command a is deregistered; therefore, console shall not print command a's description
    cmd_a_description, cmd_z_description = 'should appear first in help', 'should appear last in help'
    dut.expect_exact(cmd_z_description, not_matching=cmd_a_description)


@pytest.mark.parametrize(
    'config', [
        pytest.param('defaults'),
    ]
)
@pytest.mark.parametrize(
    'test_on', [
        pytest.param('host', marks=[pytest.mark.linux, pytest.mark.host_test]),
        pytest.param('target', marks=[pytest.mark.esp32, pytest.mark.esp32c3, pytest.mark.generic]),
        pytest.param('qemu', marks=[pytest.mark.esp32, pytest.mark.host_test, pytest.mark.qemu]),
    ]
)
def test_console_help_re_register(dut: Dut, test_on: str) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.confirm_write('"esp console re-register commands"', expect_str='esp>')

    dut.write('help')

    dut.expect_exact('should appear last in help')
    dut.expect_exact('should appear first in help')
