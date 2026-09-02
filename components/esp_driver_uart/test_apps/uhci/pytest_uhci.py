# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
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
        'cache_safe',
    ],
    indirect=True,
)
@idf_parametrize(
    'target', ['esp32c3', 'esp32c5', 'esp32c6', 'esp32h2', 'esp32p4', 'esp32s31', 'esp32h4'], indirect=['target']
)
def test_uhci(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.octal_psram
@pytest.mark.parametrize(
    'config',
    [
        'cache_safe',
        'release',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_uhci_psram_s3(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.parametrize(
    'config, target',
    [
        pytest.param('flash_enc', target, marks=get_flash_encryption_marks(target))
        for target in soc_filtered_targets(
            'SOC_UHCI_SUPPORTED == 1 and SOC_PSRAM_DMA_CAPABLE == 1 and SOC_FLASH_ENC_SUPPORTED == 1'
        )
    ],
    indirect=True,
)
def test_uhci_flash_encryption(dut: Dut) -> None:
    dut.run_all_single_board_cases()
