# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32p4
@pytest.mark.generic
def test_gptimer_capture(dut: Dut) -> None:
    dut.expect_exact('Configure trig gpio')
    dut.expect_exact('Configure echo gpio')
    dut.expect_exact('Create etm event handle for echo gpio')
    dut.expect_exact('Create gptimer handle')
    dut.expect_exact('Get gptimer etm task handle (capture)')
    dut.expect_exact('Create ETM channel then connect gpio event and gptimer task')
    dut.expect_exact('Install GPIO edge interrupt')
    dut.expect_exact('Enable etm channel and gptimer')
    dut.expect_exact('Start gptimer')
