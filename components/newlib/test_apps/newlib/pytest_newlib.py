# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        pytest.param('default', marks=[pytest.mark.supported_targets]),
        pytest.param('options', marks=[pytest.mark.supported_targets]),
        pytest.param('single_core_esp32', marks=[pytest.mark.esp32]),
        pytest.param('psram_esp32', marks=[pytest.mark.esp32]),
        pytest.param('release_esp32', marks=[pytest.mark.esp32]),
        pytest.param('release_esp32c2', marks=[pytest.mark.esp32c2]),
    ],
    indirect=True
)
def test_newlib(dut: Dut) -> None:
    dut.run_all_single_board_cases()
