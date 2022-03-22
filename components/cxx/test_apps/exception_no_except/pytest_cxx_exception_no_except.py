# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.generic
def test_cxx_noexcept_out_of_range(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('1')
    dut.expect_exact('abort() was called')
    dut.expect_exact('Rebooting...')


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.generic
def test_cxx_noexcept_bad_alloc(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('2')
    dut.expect_exact('abort() was called')
    dut.expect_exact('Rebooting...')
