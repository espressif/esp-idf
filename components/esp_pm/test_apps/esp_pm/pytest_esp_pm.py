# SPDX-FileCopyrightText: 2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut


@pytest.mark.generic
@pytest.mark.supported_targets
@pytest.mark.parametrize('config', [
    'default',
    'slp_iram_opt',
    'limits',
    'options',
], indirect=True)
def test_esp_pm(dut: Dut) -> None:
    dut.run_all_single_board_cases()


# psram attr tests with xip_psram
@pytest.mark.esp32s2
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'pm_xip_psram_esp32s2'
    ],
    indirect=True,
)
def test_esp_attr_xip_psram_esp32s2(dut: Dut) -> None:
    dut.run_all_single_board_cases()


# psram attr tests with xip_psram
@pytest.mark.esp32s3
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'pm_xip_psram_esp32s3'
    ],
    indirect=True,
)
def test_esp_attr_xip_psram_esp32s3(dut: Dut) -> None:
    dut.run_all_single_board_cases()


# power down CPU and TOP domain in auto-lightsleep
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32p4
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'pm_pd_top_sleep'
    ],
)
def test_esp_pd_top_and_cpu_sleep(dut: Dut) -> None:
    dut.run_all_single_board_cases()
