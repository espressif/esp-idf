# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.generic
@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
def test_perfmon_ut(dut: Dut) -> None:
    dut.run_all_single_board_cases()
