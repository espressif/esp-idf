# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
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
    ]
)
def test_fatfs_flash_wl_generic(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('')
    dut.expect_exact('Enter test for running.')
    dut.write('*')
    dut.expect_unity_test_output(timeout=180)


@pytest.mark.supported_targets
@pytest.mark.psram
@pytest.mark.parametrize(
    'config',
    [
        'psram',
    ]
)
def test_fatfs_flash_wl_psram(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('')
    dut.expect_exact('Enter test for running.')
    dut.write('*')
    dut.expect_unity_test_output(timeout=180)
