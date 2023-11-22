# SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest


@pytest.mark.supported_targets
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
        'release',
    ],
    indirect=True,
)
def test_uart_single_dev(case_tester) -> None:                # type: ignore
    case_tester.run_all_normal_cases(reset=True)
