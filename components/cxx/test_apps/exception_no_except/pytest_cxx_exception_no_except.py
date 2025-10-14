# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['esp32', 'esp32c3', 'esp32c61', 'esp32p4'], indirect=['target'])
def test_cxx_noexcept_out_of_range(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('1')
    dut.expect_exact('abort() was called')
    dut.expect_exact('Rebooting...')


@pytest.mark.generic
@idf_parametrize('target', ['esp32', 'esp32c3', 'esp32c61', 'esp32p4'], indirect=['target'])
def test_cxx_noexcept_bad_alloc(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('2')
    dut.expect_exact('abort() was called')
    dut.expect_exact('Rebooting...')
