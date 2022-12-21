# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import pytest
from pytest_embedded_idf import IdfDut

CONFIGS = [
    'iram_safe',
    'release',
]


@pytest.mark.supported_targets
@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS, indirect=True)
def test_gpio(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='gpio')


@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS, indirect=True)
def test_gpio_filter(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='gpio_filter')


@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS, indirect=True)
def test_dedic_gpio(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='dedic_gpio')


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS, indirect=True)
def test_sigma_delta(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='sigma_delta')


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS, indirect=True)
def test_rtc_io(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='rtcio')
