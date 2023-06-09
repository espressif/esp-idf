# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

import pytest
from pytest_embedded import Dut

CONFIGS = [
    pytest.param('general', marks=[pytest.mark.supported_targets, pytest.mark.temp_skip_ci(targets=['esp32h2'], reason='h2 support TBD')]),
    pytest.param('release', marks=[pytest.mark.supported_targets, pytest.mark.temp_skip_ci(targets=['esp32h2'], reason='h2 support TBD')]),
    pytest.param('single_core', marks=[pytest.mark.esp32]),
    pytest.param('freertos_compliance', marks=[pytest.mark.esp32]),
    pytest.param('isr_dispatch_esp32', marks=[pytest.mark.esp32]),
    pytest.param('isr_dispatch_esp32c3', marks=[pytest.mark.esp32c3]),
    pytest.param('cpu1_esp32', marks=[pytest.mark.esp32]),
    pytest.param('any_cpu_esp32', marks=[pytest.mark.esp32]),
    pytest.param('cpu1_esp32s3', marks=[pytest.mark.esp32s3]),
    pytest.param('any_cpu_esp32s3', marks=[pytest.mark.esp32s3]),
]


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS, indirect=True)
def test_esp_timer(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)


@pytest.mark.esp32
@pytest.mark.quad_psram
@pytest.mark.parametrize('config', [
    'psram',
], indirect=True)
def test_esp_timer_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)


@pytest.mark.esp32c2
@pytest.mark.generic
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'config, baud',
    [
        ('26mhz_esp32c2', '74880'),
    ],
    indirect=True,
)
def test_esp_timer_esp32c2_xtal_26mhz(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)
