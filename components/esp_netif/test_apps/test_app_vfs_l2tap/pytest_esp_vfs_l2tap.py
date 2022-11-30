# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.ethernet
def test_esp_netif(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('\n')
    dut.expect_exact('Enter test for running.')
    dut.write('*')
    dut.expect_unity_test_output()
