# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    ['cache_safe', 'release', 'pm_enable'],
    indirect=True,
)
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_dvp(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.octal_psram
@pytest.mark.parametrize(
    'config',
    ['cache_safe', 'release', 'pm_enable'],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_dvp_driver(case_tester) -> None:  # type: ignore
    for case in case_tester.test_menu:
        if 'TEST DVP camera on esp32s3_eye' in case.name:
            continue
        case_tester.run_normal_case(case=case, reset=True)


@pytest.mark.esp32s3_eye
@pytest.mark.parametrize(
    'config',
    ['esp32s3_eye'],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_dvp_s3eye(dut: Dut) -> None:
    dut.run_all_single_board_cases()
