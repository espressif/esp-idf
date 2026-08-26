# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@idf_parametrize(
    'target,markers',
    [
        ('supported_targets', pytest.mark.generic),
        ('linux', pytest.mark.host_test),
    ],
    indirect=['target'],
)
def test_freertos_real_time_stats(dut: Dut) -> None:
    for _ in range(0, 3):
        dut.expect_exact('Real time stats obtained')
