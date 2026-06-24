# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'pm_disabled_240mhz',
        'pm_disabled_160mhz',
        'pm_enabled_240mhz',
        'pm_enabled_160mhz',
        'pm_disabled_240mhz_xip_psram',
        'pm_enabled_240mhz_xip_psram',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c5'], indirect=['target'])
def test_esp_flash_freq_limit(dut: Dut) -> None:
    dut.run_all_single_board_cases(group='esp_flash_freq_limit', timeout=10)
