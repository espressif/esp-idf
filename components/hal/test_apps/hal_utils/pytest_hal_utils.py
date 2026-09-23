# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@idf_parametrize('target', ['linux'], indirect=['target'])
def test_hal_utils(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('*')
    dut.expect_unity_test_output()
