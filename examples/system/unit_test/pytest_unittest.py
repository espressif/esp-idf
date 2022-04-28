# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic
def test_unit_test(dut: Dut) -> None:

    def get_reg_nums(number: int) -> str:
        return r'\d{1,2}\s+' * number

    dut.expect_exact('In main application. Collecting 32 random numbers from 1 to 100:')
    dut.expect(get_reg_nums(10))
    dut.expect(get_reg_nums(10))
    dut.expect(get_reg_nums(10))
    dut.expect(get_reg_nums(2))
