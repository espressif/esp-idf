# SPDX-FileCopyrightText: 2023-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut


@pytest.mark.linux
@pytest.mark.host_test
def test_hal_utils(dut: Dut) -> None:
    dut.run_all_single_board_cases()
