# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32p4
@pytest.mark.temp_skip_ci(targets=['esp32s2'], reason='lack of runners with usb_host_flash_disk tag')
@pytest.mark.usb_host_flash_disk
def test_usb_hcd(dut: Dut) -> None:
    if (dut.target == 'esp32s3'):
        dut.run_all_single_board_cases(group='full_speed')
    else:
        dut.run_all_single_board_cases(group='high_speed')
