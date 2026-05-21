# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'default',
        'release',
        'fastseek',
    ],
    indirect=True,
)
def test_fatfs_flash_wl_generic(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=240)


@pytest.mark.supported_targets
@pytest.mark.psram
@pytest.mark.parametrize(
    'config',
    [
        'psram',
    ],
    indirect=True,
)
def test_fatfs_flash_wl_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=240)
