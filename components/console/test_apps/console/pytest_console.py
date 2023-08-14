# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


def do_test_quit(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"esp console repl test"')

    dut.expect_exact('esp>', timeout=5)
    dut.write('quit')

    dut.expect_exact('ByeBye', timeout=5)


def do_test_help_generic(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"esp console help command"')

    dut.expect_exact('esp>', timeout=5)
    dut.write('help')

    dut.expect_exact('quit', timeout=5)
    dut.expect_exact('Quit REPL environment', timeout=5)

    dut.expect(r'help\s+\[<string>\]', timeout=5)

    # Note: repl seems to do the line breaks by itself, this needs to be adjusted if repl changes its line width
    dut.expect_exact('Print the summary of all registered commands if no arguments are given,', timeout=5)
    dut.expect_exact('otherwise print summary of given command.', timeout=5)
    dut.expect(r'<string>\s+Name of command\s+esp>', timeout=5)


def do_test_help_quit(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"esp console help command"')

    dut.expect_exact('esp>', timeout=5)
    dut.write('help quit')

    dut.expect(r'quit\s+Quit REPL environment\s+esp>', timeout=5)


@pytest.mark.generic
@pytest.mark.supported_targets
def test_console(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.supported_targets
def test_console_repl(dut: Dut) -> None:
    do_test_quit(dut)


@pytest.mark.generic
@pytest.mark.supported_targets
def test_console_help_generic(dut: Dut) -> None:
    do_test_help_generic(dut)


@pytest.mark.generic
@pytest.mark.supported_targets
def test_console_help_quit(dut: Dut) -> None:
    do_test_help_quit(dut)


@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.esp32
@pytest.mark.esp32c3
def test_console_qemu(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.esp32
def test_console_repl_qemu(dut: Dut) -> None:
    do_test_quit(dut)


@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.esp32
def test_console_help_generic_qemu(dut: Dut) -> None:
    do_test_help_generic(dut)


@pytest.mark.host_test
@pytest.mark.qemu
@pytest.mark.esp32
def test_console_help_quit_qemu(dut: Dut) -> None:
    do_test_help_quit(dut)
