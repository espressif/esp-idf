# SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest


@pytest.mark.supported_targets
@pytest.mark.temp_skip_ci(targets=['esp32s3', 'esp32p4'], reason='skip due to duplication with test_uart_single_dev_psram, p4 TBD')  # TODO: IDF-8971
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


@pytest.mark.esp32s3
@pytest.mark.octal_psram
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
        'release',
    ],
    indirect=True,
)
def test_uart_single_dev_psram(case_tester) -> None:          # type: ignore
    case_tester.run_all_normal_cases(reset=True)
