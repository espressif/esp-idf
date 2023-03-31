# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'esp32_release',
        'esp32_2t',
    ],
    indirect=True,
)
def test_psram_esp32(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32s2
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'esp32s2_release',
        'esp32s2_advanced',
    ],
    indirect=True,
)
def test_psram_esp32s2(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32s3
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'esp32s3_quad_release',
        'esp32s3_quad_advanced',
    ],
    indirect=True,
)
def test_psram_esp32s3(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32s3
@pytest.mark.octal_psram
@pytest.mark.parametrize(
    'config',
    [
        'esp32s3_octal_release',
        'esp32s3_octal_advanced',
    ],
    indirect=True,
)
def test_psram_esp32s3_octal(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32
@pytest.mark.psramv0
@pytest.mark.parametrize(
    'config',
    [
        'esp32_hspi',
        'esp32_vspi',
    ],
    indirect=True,
)
def test_psram_esp32_psramv0(dut: Dut) -> None:
    dut.run_all_single_board_cases()
