# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut

configurations = [
    'noexcept',
    'exceptions_rtti'
]


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.generic
@pytest.mark.parametrize('config', configurations, indirect=True)
def test_cxx_static_init_non_pod(dut: Dut) -> None:
    dut.run_all_single_board_cases(name=['can use static initializers for non-POD types'])


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.generic
@pytest.mark.parametrize('config', configurations, indirect=True)
def test_cxx_misc(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='misc')


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.generic
@pytest.mark.parametrize('config', configurations, indirect=True)
def test_cxx_stack_smash(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('\"stack smashing protection CXX\"')
    dut.expect_exact('Stack smashing protect failure!')
    dut.expect_exact('Rebooting...')
