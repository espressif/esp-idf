# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
@idf_parametrize(
    'target', ['esp32', 'esp32c3'], indirect=['target']
)  # should be sufficient to test on one Xtensa and one RISC-V target
def test_esp_flash_blockdev(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='esp_flash')
