# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
def test_pthread_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=10)
