# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


@idf_parametrize('target', ['linux'], indirect=['target'])
def test_linux_component(dut: IdfDut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('![ignore]')
    dut.expect(r'\d{1} Tests 0 Failures 0 Ignored', timeout=10)
