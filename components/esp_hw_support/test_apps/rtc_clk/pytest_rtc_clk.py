# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
from typing import Any

import pytest
from pytest_embedded import Dut


@pytest.mark.generic
@pytest.mark.esp32
def test_rtc_clk(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='rtc_clk')


@pytest.mark.esp32
@pytest.mark.xtal32k
def test_rtc_xtal32k(dut: Dut) -> None:
    dut.run_all_single_board_cases(attributes={'test_env': 'xtal32k'})


@pytest.mark.esp32
@pytest.mark.no32kXtal
def test_rtc_no_xtal32k(dut: Dut) -> None:
    dut.run_all_single_board_cases(attributes={'test_env': 'noXtal32k'})


@pytest.mark.generic
@pytest.mark.supported_targets
# TODO: [ESP32P4] IDF-8973 [ESP32C5] IDF-10309 [ESP32C61] IDF-9274 IDF-10984
@pytest.mark.temp_skip_ci(targets=['esp32c6', 'esp32h2', 'esp32p4', 'esp32c5', 'esp32c61'], reason='support TBD')
def test_rtc_calib(case_tester: Any) -> None:
    case_tester.run_all_multi_stage_cases()
