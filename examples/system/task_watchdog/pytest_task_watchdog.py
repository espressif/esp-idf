# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic
def test_task_watchdog(dut: Dut) -> None:

    dut.expect_exact('Initialize TWDT')
    dut.expect_exact('Delay for 10 seconds')
    dut.expect_exact('Unsubscribing and deleting tasks')
    dut.expect_exact('Complete')
