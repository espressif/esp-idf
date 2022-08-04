# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
def test_ulp_fsm(dut: Dut) -> None:
    dut.expect('Press ENTER to see the list of tests')
    dut.write('![ignore]')
    dut.expect_unity_test_output()
