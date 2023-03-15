# SPDX-FileCopyrightText: 2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.generic
def test_ecc(dut: Dut) -> None:
    dut.run_all_single_board_cases()
