# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.generic
@pytest.mark.supported_targets
def test_console(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('![ignore]')
    dut.expect_unity_test_output()
