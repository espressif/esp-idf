# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import itertools

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize(
    'config,target',
    itertools.chain(
        itertools.product(['gpio'], ['esp32', 'esp32s3', 'esp32c5', 'esp32c6', 'esp32h2', 'esp32p4']),
        itertools.product(['tez'], ['esp32', 'esp32s3', 'esp32c5', 'esp32c6', 'esp32h2', 'esp32p4']),
        itertools.product(['soft'], ['esp32s3', 'esp32c5', 'esp32c6', 'esp32h2', 'esp32p4']),
    ),
    indirect=['config', 'target'],
)
def test_mcpwm_sync_example(dut: Dut) -> None:
    dut.expect_exact('example: Create timers')
    dut.expect_exact('example: Create operators')
    dut.expect_exact('example: Create comparators')
    dut.expect_exact('example: Create generators')
    dut.expect_exact('example: Set generator actions on timer and compare event')
    dut.expect_exact('example: Start timers one by one, so they are not synced')
    dut.expect_exact('example: Setup sync strategy')
    dut.expect_exact('example: Now the output PWMs should in sync')
