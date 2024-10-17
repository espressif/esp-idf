# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.supported_targets
@pytest.mark.generic
def test_rt_mqueue_example(dut: Dut) -> None:
    dut.expect_exact('sending: 0')
    dut.expect_exact('received: 0')
    dut.expect_exact('sending: 1')
    dut.expect_exact('received: 1')
