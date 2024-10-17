# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import re

import pytest
from pytest_embedded import Dut

DEFAULT_TIMEOUT = 20
TEST_SUBMENU_PATTERN_PYTEST = re.compile(rb'\s+\((\d+)\)\s+"([^"]+)"\r?\n')


@pytest.mark.supported_targets
@pytest.mark.temp_skip_ci(targets=['esp32c5'], reason='C5 has not supported deep sleep')  # TODO: [ESP32C5] IDF-8640, IDF-10317
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'defaults',
    ],
    indirect=True,
)
def test_app_update(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=90)


@pytest.mark.supported_targets
# TODO: [ESP32C61] IDF-9245, IDF-10983
@pytest.mark.temp_skip_ci(targets=['esp32c61'], reason='C61 has not supported deep sleep')
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'xip_psram',
    ],
    indirect=True,
)
def test_app_update_xip_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=90)


@pytest.mark.supported_targets
@pytest.mark.temp_skip_ci(targets=['esp32c5'], reason='C5 has not supported deep sleep')  # TODO: [ESP32C5] IDF-8640, IDF-10317
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'xip_psram_with_rom_impl',
    ],
    indirect=True,
)
def test_app_update_xip_psram_rom_impl(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=90)
