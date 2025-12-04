# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
from typing import Any

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    ['default'],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_esp_common(dut: Dut) -> None:
    dut.run_all_single_board_cases()


# psram noinit attr tests with psram enabled
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'psram_noinit',
    ],
    indirect=True,
)
@idf_parametrize('target', soc_filtered_targets('SOC_SPIRAM_SUPPORTED == 1'), indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_esp_attr_psram_noinit(dut: Dut) -> None:
    dut.run_all_single_board_cases()


# psram noinit memory tests with psram enabled
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    ['psram_noinit'],
    indirect=True,
)
@idf_parametrize('target', soc_filtered_targets('SOC_SPIRAM_SUPPORTED == 1'), indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_esp_attr_psram_noinit_multiple_stages(case_tester: Any) -> None:
    case_tester.run_all_multi_stage_cases()


# psram attr tests with psram enabled
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'psram',
    ],
    indirect=True,
)
@idf_parametrize('target', soc_filtered_targets('SOC_SPIRAM_SUPPORTED == 1'), indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_esp_attr_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases()


# psram attr tests with xip_psram
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    ['xip_psram'],
    indirect=True,
)
@idf_parametrize('target', soc_filtered_targets('SOC_SPIRAM_XIP_SUPPORTED == 1'), indirect=['target'])
@pytest.mark.temp_skip_ci(targets=['esp32p4'], reason='p4 rev3 migration')
def test_esp_attr_xip_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases()
