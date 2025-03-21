# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


CONFIGS = [
    pytest.param('default', marks=[pytest.mark.esp32s2, pytest.mark.esp32s3, pytest.mark.esp32p4]),
    pytest.param('esp32p4_psram', marks=[pytest.mark.esp32p4])
]


@pytest.mark.usb_host_flash_disk
@pytest.mark.temp_skip_ci(targets=['esp32s2'], reason='lack of runners with usb_host_flash_disk tag')
@pytest.mark.parametrize('config', CONFIGS, indirect=True)
def test_usb_hcd(dut: Dut) -> None:
    if dut.target == 'esp32p4':
        dut.run_all_single_board_cases(group='high_speed', reset=True)
    else:
        dut.run_all_single_board_cases(group='full_speed', reset=True)
