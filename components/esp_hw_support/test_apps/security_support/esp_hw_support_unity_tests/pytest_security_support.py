# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        pytest.param('default', marks=[pytest.mark.supported_targets, pytest.mark.temp_skip_ci(targets=['esp32h2'], reason='test failed IDF-6898')]),
        pytest.param('release', marks=[pytest.mark.supported_targets, pytest.mark.temp_skip_ci(targets=['esp32h2'], reason='test failed IDF-6898')]),
    ],
    indirect=True,
)
def test_security_support(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)
