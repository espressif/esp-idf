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
def test_csi_driver(case_tester) -> None:  # type: ignore
    for case in case_tester.test_menu:
        if 'TEST esp_cam on ov5647' in case.name:
            continue
        case_tester.run_normal_case(case=case, reset=True)


@pytest.mark.camera
@pytest.mark.ov5647
@pytest.mark.parametrize(
    'config',
    ['cache_safe', 'release'],
    indirect=True,
)
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_csi_camera(dut: Dut) -> None:
    dut.run_all_single_board_cases()
