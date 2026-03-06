# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

configurations = ['noexcept', 'exceptions_rtti']


@pytest.mark.generic
@pytest.mark.parametrize('config', configurations, indirect=True)
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_cxx_static_init_non_pod(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"can use static initializers for non-POD types"')
    dut.expect_unity_test_output()


@pytest.mark.generic
@pytest.mark.parametrize('config', configurations, indirect=True)
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_cxx_misc(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('[misc]')
    dut.expect_unity_test_output()


@pytest.mark.generic
@pytest.mark.parametrize('config', configurations, indirect=True)
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_cxx_stack_smash(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('"stack smashing protection CXX"')
    dut.expect_exact('Stack smashing protect failure!')
    dut.expect_exact('Rebooting...')
