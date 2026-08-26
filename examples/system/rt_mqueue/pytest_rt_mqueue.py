# SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@idf_parametrize(
    'target,markers',
    [
        ('esp32', pytest.mark.generic),
        ('esp32c3', pytest.mark.generic),
        ('linux', pytest.mark.host_test),
    ],
    indirect=['target'],
)
def test_rt_mqueue_example(dut: Dut) -> None:
    dut.expect_exact('sending: 0')
    dut.expect_exact('received: 0')
    dut.expect_exact('sending: 1')
    dut.expect_exact('received: 1')
