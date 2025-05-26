# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.temp_skip_ci(targets=['esp32s2'], reason='lack of runners with usb_host_flash_disk tag')
@pytest.mark.usb_host_flash_disk
@idf_parametrize('target', ['esp32s2', 'esp32s3', 'esp32p4'], indirect=['target'])
def test_usb_host(dut: Dut) -> None:
    if dut.target == 'esp32p4':
        dut.run_all_single_board_cases(group='high_speed', reset=True)
    else:
        dut.run_all_single_board_cases(group='full_speed', reset=True)
