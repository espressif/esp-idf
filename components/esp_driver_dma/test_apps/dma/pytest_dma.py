# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


def get_flash_encryption_marks(target: str) -> tuple[pytest.MarkDecorator, ...]:
    if target == 'esp32s3':
        return (pytest.mark.flash_encryption_f4r8,)

    return (pytest.mark.flash_encryption,)


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
    ['esp32s2', 'esp32s31', 'esp32c2', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32h4', 'esp32p4'],
    indirect=['target'],
)
def test_dma(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.esp32p4_rev1
@pytest.mark.parametrize(
    'config',
    [
        'esp32p4_rev1',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_dma_esp32p4_rev1(dut: Dut) -> None:
    dut.run_all_single_board_cases()


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
    dut.run_all_single_board_cases()


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
    dut.run_all_single_board_cases()


@pytest.mark.parametrize(
    'config, target',
    [
        pytest.param('flash_enc', target, marks=get_flash_encryption_marks(target))
        for target in soc_filtered_targets(
            'SOC_GDMA_SUPPORTED == 1 and SOC_PSRAM_DMA_CAPABLE == 1 and SOC_FLASH_ENC_SUPPORTED == 1'
        )
    ],
    indirect=True,
)
def test_dma_flash_encryption(dut: Dut) -> None:
    dut.run_all_single_board_cases()
