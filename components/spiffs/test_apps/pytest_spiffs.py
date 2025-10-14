# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'default',
        'release',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_spiffs_generic(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)


@pytest.mark.quad_psram
@pytest.mark.parametrize(
    'config',
    [
        'psram',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_spiffs_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)
