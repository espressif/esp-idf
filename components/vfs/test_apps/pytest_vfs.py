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
        'iram',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c2', 'esp32c3', 'esp32c6', 'esp32h2'], indirect=['target'])
def test_vfs_default(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'ccomp',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_vfs_ccomp(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.quad_psram
@pytest.mark.parametrize(
    'config',
    [
        'psram',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_vfs_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases()
