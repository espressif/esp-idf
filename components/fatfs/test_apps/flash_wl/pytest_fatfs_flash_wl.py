# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'default',
        'release',
        'fastseek',
        'auto_fsync',
        'no_dyn_buffers',
    ]
)
def test_fatfs_flash_wl_generic(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=240)


@pytest.mark.esp32
@pytest.mark.generic
@pytest.mark.psram
@pytest.mark.parametrize(
    'config',
    [
        'psram',
    ]
)
def test_fatfs_flash_wl_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=180)
