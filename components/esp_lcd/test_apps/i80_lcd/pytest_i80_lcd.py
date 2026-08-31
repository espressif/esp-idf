# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
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
@idf_parametrize('target', soc_filtered_targets('SOC_LCD_I80_SUPPORTED == 1'), indirect=['target'])
def test_i80_lcd(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.parametrize(
    'config, target',
    [
        pytest.param('flash_enc', target, marks=get_flash_encryption_marks(target))
        for target in soc_filtered_targets(
            'SOC_LCD_I80_SUPPORTED == 1 and SOC_PSRAM_DMA_CAPABLE == 1 and SOC_FLASH_ENC_SUPPORTED == 1'
        )
    ],
    indirect=True,
)
def test_i80_lcd_with_flash_encryption(dut: Dut) -> None:
    dut.run_all_single_board_cases()
