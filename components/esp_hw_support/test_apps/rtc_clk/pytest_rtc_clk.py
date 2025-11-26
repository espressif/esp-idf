# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded_idf import CaseTester
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_CLK_TREE_SUPPORTED == 1'), indirect=['target'])
def test_rtc_clk(case_tester: CaseTester) -> None:
    for case in case_tester.test_menu:
        if 'test_env' in case.attributes:
            continue
        case_tester.run_normal_case(case=case)


@pytest.mark.xtal32k
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_rtc_xtal32k(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(attributes={'test_env': 'xtal32k'})


@pytest.mark.no32kXtal
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_rtc_no_xtal32k(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(attributes={'test_env': 'noXtal32k'})


@pytest.mark.generic
@idf_parametrize('target', soc_filtered_targets('SOC_CLK_TREE_SUPPORTED == 1'), indirect=['target'])
def test_rtc_calib_compensation_across_dslp(case_tester: CaseTester) -> None:
    case_tester.run_all_multi_stage_cases()
