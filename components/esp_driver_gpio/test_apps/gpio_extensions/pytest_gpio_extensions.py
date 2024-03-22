# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf import IdfDut

CONFIGS = [
    'iram_safe',
    'release',
]


@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32p4
@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS, indirect=True)
def test_gpio_filter(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='gpio_filter')


@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32p4
@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS, indirect=True)
def test_dedic_gpio(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='dedic_gpio')
