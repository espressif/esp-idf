# SPDX-FileCopyrightText: 2024-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


def _skip_board_specific_cases(case_tester) -> None:  # type: ignore
    for case in case_tester.test_menu:
        if 'TEST esp_cam DVP on ov2640' in case.name:
            continue
        case_tester.run_normal_case(case=case, reset=True)


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    ['cache_safe', 'release', 'pm_enable'],
    indirect=True,
)
@idf_parametrize('target', ['esp32p4', 'esp32s31'], indirect=['target'])
def test_dvp(case_tester) -> None:  # type: ignore
    _skip_board_specific_cases(case_tester)


@pytest.mark.esp32s3_eye
@pytest.mark.parametrize(
    'config',
    ['cache_safe', 'release', 'pm_enable', 'esp32s3_ov2640'],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_dvp_esp32s3(dut: Dut) -> None:
    dut.run_all_single_board_cases()
