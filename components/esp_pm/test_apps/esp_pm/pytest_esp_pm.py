# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.temp_skip_ci(targets=['esp32c61'], reason='not supported yet')  # TODO: [ESP32C61] IDF-9250 IDF-10985
@pytest.mark.parametrize(
    'config',
    [
        'default',
        'slp_iram_opt',
        'limits',
        'options',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_esp_pm(dut: Dut) -> None:
    dut.run_all_single_board_cases()


# psram attr tests with xip_psram
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    ['pm_xip_psram_esp32s2'],
    indirect=True,
)
@idf_parametrize('target', ['esp32s2'], indirect=['target'])
def test_esp_attr_xip_psram_esp32s2(dut: Dut) -> None:
    dut.run_all_single_board_cases()


# psram attr tests with xip_psram
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    ['pm_xip_psram_esp32s3'],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_esp_attr_xip_psram_esp32s3(dut: Dut) -> None:
    dut.run_all_single_board_cases()


# power down CPU and TOP domain in auto-lightsleep
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    ['pm_pd_top_sleep'],
)
@idf_parametrize('target', ['esp32c5', 'esp32c6', 'esp32h2', 'esp32p4'], indirect=['target'])
def test_esp_pd_top_and_cpu_sleep(dut: Dut) -> None:
    dut.run_all_single_board_cases()
