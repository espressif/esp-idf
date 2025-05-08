# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
# normal mmu tests


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_mmap(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='mmu')


# mmu tests with psram enabled
PSRAM_RELEASE_CONFIGS = [
    pytest.param('psram_release_esp32', marks=[pytest.mark.esp32]),
    pytest.param('psram_release_esp32s2', marks=[pytest.mark.esp32s2]),
    pytest.param('psram_release_esp32s3', marks=[pytest.mark.esp32s3]),
    pytest.param('psram_release_esp32p4', marks=[pytest.mark.esp32p4]),
]


@pytest.mark.generic
@idf_parametrize(
    'config,target',
    [
        ('psram_release_esp32', 'esp32'),
        ('psram_release_esp32s2', 'esp32s2'),
        ('psram_release_esp32s3', 'esp32s3'),
        ('psram_release_esp32p4', 'esp32p4'),
    ],
    indirect=['config', 'target'],
)
def test_mmap_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='mmu')


# mmu tests with xip_psram
XIP_CONFIGS = [
    pytest.param('xip_psram_esp32s2', marks=[pytest.mark.esp32s2]),
    pytest.param('xip_psram_esp32s3', marks=[pytest.mark.esp32s3]),
]


@pytest.mark.generic
@idf_parametrize(
    'config,target', [('xip_psram_esp32s2', 'esp32s2'), ('xip_psram_esp32s3', 'esp32s3')], indirect=['config', 'target']
)
def test_mmap_xip_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='mmu')


# normal cache tests
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_cache(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='cache')


# cache tests with psram enabled
@pytest.mark.generic
@idf_parametrize(
    'config,target',
    [
        ('psram_release_esp32', 'esp32'),
        ('psram_release_esp32s2', 'esp32s2'),
        ('psram_release_esp32s3', 'esp32s3'),
        ('psram_release_esp32p4', 'esp32p4'),
    ],
    indirect=['config', 'target'],
)
def test_cache_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='cache')


# cache tests with xip_psram
@pytest.mark.generic
@idf_parametrize(
    'config,target', [('xip_psram_esp32s2', 'esp32s2'), ('xip_psram_esp32s3', 'esp32s3')], indirect=['config', 'target']
)
def test_cache_xip_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='cache')
