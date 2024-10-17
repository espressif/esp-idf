# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.generic
@pytest.mark.parametrize('config', [
    'default',
    'release',
], indirect=True)
def test_spiffs_generic(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)


@pytest.mark.esp32s3
@pytest.mark.quad_psram
@pytest.mark.parametrize('config', [
    'psram',
], indirect=True)
def test_spiffs_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(timeout=120)
