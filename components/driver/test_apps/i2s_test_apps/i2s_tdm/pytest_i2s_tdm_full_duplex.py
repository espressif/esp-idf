# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest


@pytest.mark.esp32s3
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.generic_multi_device
@pytest.mark.parametrize('count', [
    2,
], indirect=True)
def test_i2s_tdm_full_duplex(case_tester) -> None:  # type: ignore
    case_tester.run_all_cases(timeout=30)
