# SPDX-FileCopyrightText: 2023-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.flash_suspend
@pytest.mark.parametrize(
    'config',
    [
        'release',
        'i2c_isr_flash',
        'text_in_flash_when_suspend',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_flash_auto_suspend(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=30)
