# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.flash_encryption
@pytest.mark.parametrize(
    'config',
    [
        'release',
        'verify',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_flash_encryption(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.flash_encryption
@pytest.mark.parametrize(
    'config',
    [
        'rom_impl',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_flash_encryption_rom_impl(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.flash_encryption_f4r8
@pytest.mark.parametrize(
    'config',
    [
        'release_f4r8',
        'rom_impl',
        'verify',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_flash_encryption_f4r8(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.flash_encryption_f8r8
@pytest.mark.parametrize(
    'config',
    [
        'release_f8r8',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_flash_encryption_f8r8(dut: Dut) -> None:
    dut.run_all_single_board_cases()
