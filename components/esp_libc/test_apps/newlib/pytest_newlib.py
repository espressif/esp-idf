# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize(
    'config,target',
    [
        ('default', 'supported_targets'),
        ('picolibc', 'supported_targets'),
        ('options', 'supported_targets'),
        ('single_core_esp32', 'esp32'),
        ('psram_esp32', 'esp32'),
        ('release_esp32', 'esp32'),
        ('release_esp32c2', 'esp32c2'),
        ('misaligned_mem', 'esp32c3'),
    ],
    indirect=['config', 'target'],
)
def test_newlib(dut: Dut) -> None:
    dut.run_all_single_board_cases()
