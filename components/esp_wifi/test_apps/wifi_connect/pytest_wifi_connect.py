# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

from typing import Tuple

import pytest
from pytest_embedded_idf.dut import IdfDut


def multi_device_case_run(dut: Tuple[IdfDut, IdfDut], test_case: str, sub_cases: list, timeout: int) -> None:
    dut_num = len(sub_cases)
    # start
    for i in range(dut_num):
        dut[i].write(f'"{test_case}"')
    for i in range(dut_num):
        dut[i].expect(r'\t\(2\)\s+\".+\"')
    for i in range(dut_num):
        dut[i].write(f'{i+1}')
    # end
    for i in range(dut_num):
        dut[i].expect_unity_test_output(timeout=timeout)
        dut[i].expect_exact("Enter next test, or 'enter' to see menu")


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.wifi_two_dut
@pytest.mark.parametrize('count', [2], indirect=True)
def test_wifi_connect_cases(dut: Tuple[IdfDut, IdfDut]) -> None:
    test_menu = dut[0].parse_test_menu()
    dut[1].expect_exact('Press ENTER to see the list of tests')
    for test_case in test_menu:
        timeout = int(test_case.attributes.get('timeout', 60))
        multi_device_case_run(dut, test_case.name, test_case.subcases, timeout)
