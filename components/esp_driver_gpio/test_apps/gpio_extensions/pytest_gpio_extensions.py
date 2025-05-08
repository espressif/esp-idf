# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize

CONFIGS = [
    'iram_safe',
    'release',
]


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@idf_parametrize(
    'target',
    ['esp32c2', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32s2', 'esp32s3', 'esp32p4'],
    indirect=['target'],
)
def test_gpio_filter(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='gpio_filter')


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@idf_parametrize(
    'target',
    ['esp32c2', 'esp32c3', 'esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32s2', 'esp32s3', 'esp32p4'],
    indirect=['target'],
)
def test_dedic_gpio(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='dedic_gpio')
