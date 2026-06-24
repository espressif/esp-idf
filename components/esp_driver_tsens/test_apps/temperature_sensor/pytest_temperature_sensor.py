# SPDX-FileCopyrightText: 2021-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from pytest_embedded_idf.utils import soc_filtered_targets


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    soc_filtered_targets('SOC_TEMP_SENSOR_SUPPORTED == 1 and IDF_TARGET not in ["esp32c5"]'),
    indirect=['target'],
)
def test_temperature_sensor_driver(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.esp32c5_rev1
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    ['esp32c5'],
    indirect=['target'],
)
def test_temperature_sensor_driver_esp32c5_rev1(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
    ],
    indirect=True,
)
@idf_parametrize(
    'target', ['esp32c6', 'esp32h2', 'esp32p4', 'esp32c5', 'esp32c61', 'esp32s31', 'esp32h4'], indirect=['target']
)
def test_temperature_sensor_cbs(dut: Dut) -> None:
    dut.run_all_single_board_cases()
