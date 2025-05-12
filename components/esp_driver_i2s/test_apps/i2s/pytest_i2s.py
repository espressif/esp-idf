# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
        'release',
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    ['esp32', 'esp32s2', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32h2', 'esp32p4', 'esp32c61'],
    indirect=['target'],
)
def test_i2s(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.octal_psram
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
        'release',
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    ['esp32s3'],
    indirect=['target'],
)
def test_i2s_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases()
