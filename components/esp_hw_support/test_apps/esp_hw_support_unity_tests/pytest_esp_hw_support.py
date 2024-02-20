# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='esp32p4 support TBD')  # TODO: IDF-8972
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        pytest.param('single_core_esp32', marks=[pytest.mark.esp32]),
        pytest.param('default', marks=[pytest.mark.supported_targets]),
        pytest.param('release', marks=[pytest.mark.supported_targets]),
    ],
    indirect=True,
)
def test_esp_hw_support(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=180)
