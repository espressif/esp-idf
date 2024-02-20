# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
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
    ],
    indirect=True)
def test_partition_table_readonly(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.flash_encryption
@pytest.mark.parametrize(
    'config',
    [
        'encrypted',
    ],
    indirect=True)
def test_partition_table_readonly_flash_encryption(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)
