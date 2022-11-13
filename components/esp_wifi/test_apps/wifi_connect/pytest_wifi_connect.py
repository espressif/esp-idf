# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

import pytest


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.wifi_two_dut
@pytest.mark.parametrize('count', [2], indirect=True)
def test_wifi_connect_cases(case_tester) -> None:  # type: ignore
    case_tester.run_all_cases()
