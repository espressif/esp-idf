# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    ['esp32s2', 'esp32c2', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32p4'],
    indirect=['target'],
)
def test_dma(dut: Dut) -> None:
    dut.run_all_single_board_cases(reset=True)


@pytest.mark.octal_psram
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_dma_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(reset=True)


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'weighted_arbitration',
    ],
    indirect=True,
)
@idf_parametrize('target', soc_filtered_targets('SOC_GDMA_SUPPORT_WEIGHTED_ARBITRATION == 1'), indirect=['target'])
def test_dma_weighted_arbitration(dut: Dut) -> None:
    dut.run_all_single_board_cases(reset=True)


@pytest.mark.flash_encryption
@pytest.mark.parametrize(
    'config',
    [
        'ext_mem_encryption',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32p4', 'esp32c5'], indirect=['target'])
def test_dma_ext_mem_encryption(dut: Dut) -> None:
    dut.run_all_single_board_cases(reset=True)


@pytest.mark.flash_encryption_f4r8
@pytest.mark.parametrize(
    'config',
    [
        'ext_mem_encryption',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_dma_ext_mem_encryption_s3_f4r8(dut: Dut) -> None:
    dut.run_all_single_board_cases(reset=True)
