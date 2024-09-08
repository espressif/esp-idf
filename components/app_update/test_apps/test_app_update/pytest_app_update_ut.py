# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import re

import pytest
from pytest_embedded import Dut

DEFAULT_TIMEOUT = 20
TEST_SUBMENU_PATTERN_PYTEST = re.compile(rb'\s+\((\d+)\)\s+"([^"]+)"\r?\n')


@pytest.mark.supported_targets
@pytest.mark.temp_skip_ci(targets=['esp32c6', 'esp32h2'], reason='c6/h2 support TBD')
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


@pytest.mark.esp32s2
@pytest.mark.esp32s3
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


@pytest.mark.esp32s3
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
