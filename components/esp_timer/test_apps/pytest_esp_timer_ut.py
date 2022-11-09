# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

import pytest
from pytest_embedded import Dut

CONFIGS = [
    pytest.param('general', marks=[pytest.mark.supported_targets]),
    pytest.param('release', marks=[pytest.mark.supported_targets]),
    pytest.param('single_core', marks=[pytest.mark.esp32]),
    pytest.param('freertos_compliance', marks=[pytest.mark.esp32]),
    pytest.param('isr_dispatch_esp32', marks=[pytest.mark.esp32]),
]


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS, indirect=True)
def test_esp_timer(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('*')
    dut.expect_unity_test_output(timeout=240)


@pytest.mark.esp32
@pytest.mark.quad_psram
@pytest.mark.parametrize('config', [
    'psram',
], indirect=True)
def test_esp_timer_psram(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('*')
    dut.expect_unity_test_output(timeout=240)


@pytest.mark.esp32c2
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'config, baud',
    [
        ('26mhz_esp32c2', '74880'),
    ],
    indirect=True,
)
def test_esp_timer_esp32c2_xtal_26mhz(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('*')
    dut.expect_unity_test_output(timeout=240)
