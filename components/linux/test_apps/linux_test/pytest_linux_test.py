# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf.dut import IdfDut


@pytest.mark.linux
@pytest.mark.host_test
def test_linux_component(dut: IdfDut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests.')
    dut.write('![ignore]')
    dut.expect(r'\d{1} Tests 0 Failures 0 Ignored', timeout=10)
