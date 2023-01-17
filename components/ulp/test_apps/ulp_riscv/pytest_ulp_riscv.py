# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest


@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
def test_ulp_riscv(case_tester) -> None:  # type: ignore
    case_tester.run_all_cases()
