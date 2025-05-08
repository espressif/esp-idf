# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize(
    'config,target',
    [
        ('general', 'supported_targets'),
        ('release', 'supported_targets'),
        ('single_core', 'esp32'),
        ('freertos_compliance', 'esp32'),
        ('isr_dispatch_esp32', 'esp32'),
        ('isr_dispatch_esp32c3', 'esp32c3'),
        ('cpu1_esp32', 'esp32'),
        ('any_cpu_esp32', 'esp32'),
        ('cpu1_esp32s3', 'esp32s3'),
        ('any_cpu_esp32s3', 'esp32s3'),
    ],
    indirect=['config', 'target'],
)
def test_esp_timer(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)


@pytest.mark.generic
@pytest.mark.psram
@pytest.mark.parametrize(
    'config',
    [
        'psram',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_esp_timer_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)


@pytest.mark.generic
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'config, baud',
    [
        ('26mhz_esp32c2', '74880'),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c2'], indirect=['target'])
def test_esp_timer_esp32c2_xtal_26mhz(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)


@pytest.mark.flash_suspend
@pytest.mark.parametrize(
    'config',
    [
        'flash_auto_suspend',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_esp_timer_flash_auto_suspend(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)
