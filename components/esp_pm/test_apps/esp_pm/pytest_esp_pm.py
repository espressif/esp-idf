# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import pytest
from pytest_embedded import Dut

CONFIGS = [
    pytest.param('default', marks=[pytest.mark.supported_targets, pytest.mark.temp_skip_ci(targets=['esp32h2'], reason='TODO: IDF-7657')]),
    pytest.param('limits', marks=[pytest.mark.supported_targets, pytest.mark.temp_skip_ci(targets=['esp32h2'], reason='TODO: IDF-7657')]),
    pytest.param('options', marks=[pytest.mark.supported_targets, pytest.mark.temp_skip_ci(targets=['esp32h2'], reason='TODO: IDF-7657')]),
]


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS, indirect=True)
def test_esp_pm(dut: Dut) -> None:
    dut.run_all_single_board_cases()
