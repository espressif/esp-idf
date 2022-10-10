# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic
def test_fatfs_flash_ro(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('')
    dut.expect_exact('Enter test for running.')
    dut.write('*')
    dut.expect_unity_test_output()
