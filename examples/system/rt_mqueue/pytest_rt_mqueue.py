# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_rt_mqueue_example(dut: Dut) -> None:
    dut.expect_exact('sending: 0')
    dut.expect_exact('received: 0')
    dut.expect_exact('sending: 1')
    dut.expect_exact('received: 1')
