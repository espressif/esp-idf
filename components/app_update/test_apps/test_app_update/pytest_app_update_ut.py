# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import re

import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

DEFAULT_TIMEOUT = 20
TEST_SUBMENU_PATTERN_PYTEST = re.compile(rb'\s+\((\d+)\)\s+"([^"]+)"\r?\n')


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'defaults',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_app_update(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=180)


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'xip_psram',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_app_update_xip_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=180)


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'xip_psram_with_rom_impl',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_app_update_xip_psram_rom_impl(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=180)


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'rollback',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32', 'esp32c3', 'esp32s3', 'esp32p4'], indirect=['target'])
def test_app_update_with_rollback(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=180)
