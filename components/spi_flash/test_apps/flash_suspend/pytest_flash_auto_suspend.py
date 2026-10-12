# SPDX-FileCopyrightText: 2023-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.flash_suspend
@pytest.mark.parametrize(
    'config',
    [
        'release',
        'text_in_flash_when_suspend',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_flash_auto_suspend(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=30)


# Generic test for all targets that support flash auto suspend
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
        pytest.param(
            'text_in_flash_when_suspend',
            marks=pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='cannot pass'),  # TODO: IDF-16195
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c5', 'esp32c61', 'esp32s31', 'esp32h21'], indirect=['target'])
def test_flash_auto_suspend_generic(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=30)
