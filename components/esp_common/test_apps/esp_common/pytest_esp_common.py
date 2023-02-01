# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded import Dut


@pytest.mark.generic
@pytest.mark.supported_targets
@pytest.mark.parametrize(
    'config',
    [
        'default'
    ]
)
def test_esp_common(dut: Dut) -> None:
    dut.run_all_single_board_cases()


def run_multiple_stages(dut: Dut, test_case_num: int, stages: int) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.esp32
@pytest.mark.parametrize(
    'config',
    [
        'esp32_psram'
    ]
)
def test_esp_common_psram_esp32(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.esp32s2
@pytest.mark.parametrize(
    'config',
    [
        'esp32s2_psram'
    ]
)
def test_esp_common_psram_esp32s2(dut: Dut) -> None:
    dut.run_all_single_board_cases()
