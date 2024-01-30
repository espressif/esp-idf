# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32p4
@pytest.mark.temp_skip_ci(targets=['esp32s2'], reason='lack of runners with usb_host_flash_disk tag')
@pytest.mark.usb_host_flash_disk
def test_usb_host_lib_example(dut: Dut) -> None:
    # register client
    dut.expect_exact('CLASS: Registering Client')

    # expect device descriptor
    dut.expect_exact('CLASS: Getting device descriptor')

    # confirm device descriptor
    dut.expect_exact('*** Device descriptor ***')

    # expect configuration descriptor
    dut.expect_exact('CLASS: Getting config descriptor')

    # confirm configuration descriptor
    dut.expect_exact('*** Configuration descriptor ***')
