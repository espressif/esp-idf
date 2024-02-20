# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

from typing import Any

import pytest
from pytest_embedded import Dut


@pytest.mark.generic
@pytest.mark.esp32
def test_rtc_clk(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('[rtc_clk]')
    dut.expect_unity_test_output()


@pytest.mark.esp32
@pytest.mark.xtal32k
def test_rtc_xtal32k(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('[test_env=xtal32k]')
    dut.expect_unity_test_output()


@pytest.mark.esp32
@pytest.mark.no32kXtal
def test_rtc_no_xtal32k(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('[test_env=noXtal32k]')
    dut.expect_unity_test_output()


@pytest.mark.generic
@pytest.mark.supported_targets
@pytest.mark.temp_skip_ci(targets=['esp32c6', 'esp32h2', 'esp32p4'], reason='c6/h2/p4 support TBD')  # TODO: IDF-8973
def test_rtc_calib(case_tester: Any) -> None:
    case_tester.run_all_multi_stage_cases()
