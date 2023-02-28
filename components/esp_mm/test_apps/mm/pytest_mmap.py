# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


# normal mmu tests
@pytest.mark.supported_targets
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
def test_mmap(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='mmu')


# mmu tests with psram enabled
@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'psram_release',
    ],
    indirect=True,
)
def test_mmap_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='mmu')


# mmu tests with xip_psram
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'xip_psram'
    ],
    indirect=True,
)
def test_mmap_xip_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='mmu')


# normal cache tests
@pytest.mark.supported_targets
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
def test_cache(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='cache')


# cache tests with psram enabled
@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'psram_release',
    ],
    indirect=True,
)
def test_cache_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='cache')


# cache tests with xip_psram
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'xip_psram'
    ],
    indirect=True,
)
def test_cache_xip_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='cache')
