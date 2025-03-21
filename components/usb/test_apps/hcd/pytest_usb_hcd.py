# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.temp_skip_ci(targets=['esp32s2'], reason='lack of runners with usb_host_flash_disk tag')
@pytest.mark.usb_host_flash_disk
@idf_parametrize(
    'config,target',
    [('default', 'esp32s2'), ('default', 'esp32s3'), ('default', 'esp32p4'), ('esp32p4_psram', 'esp32p4')],
    indirect=['config', 'target'],
)
def test_usb_hcd(dut: Dut) -> None:
    if dut.target == 'esp32p4':
        dut.run_all_single_board_cases(group='high_speed', reset=True)
    else:
        dut.run_all_single_board_cases(group='full_speed', reset=True)
