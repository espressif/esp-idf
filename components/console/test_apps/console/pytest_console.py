# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.generic
@pytest.mark.supported_targets
def test_console(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.supported_targets
def test_console_repl(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"esp console repl test"')

    dut.expect_exact('esp>')
    dut.write('quit')

    dut.expect_exact('ByeBye')
    dut.expect_unity_test_output()
