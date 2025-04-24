# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


def verify_test_order(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('\n')
    dut.expect('Test 1')
    dut.expect('Test 2')
    dut.expect('Test 3')
    dut.expect('Test 4')


@pytest.mark.generic
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_unit_test_order(dut: Dut) -> None:
    verify_test_order(dut)


@pytest.mark.host_test
@idf_parametrize('target', ['linux'], indirect=['target'])
def test_unit_test_order_linux(dut: Dut) -> None:
    verify_test_order(dut)
