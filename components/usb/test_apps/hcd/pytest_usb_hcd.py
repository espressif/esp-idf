# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.usb_host_flash_disk
def test_usb_hcd(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('[full_speed]')
    dut.expect_unity_test_output()
