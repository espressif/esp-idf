# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

import pytest
from pytest_embedded_idf.dut import IdfDut


@pytest.mark.esp32s2
@pytest.mark.esp32s3
def test_hid_basic(dut: IdfDut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('[auto]')
    dut.expect_unity_test_output()
