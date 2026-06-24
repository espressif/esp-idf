# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'default',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_partition_table_readonly(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)


@pytest.mark.flash_encryption
@pytest.mark.parametrize(
    'config',
    [
        'encrypted',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_partition_table_readonly_flash_encryption(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)
