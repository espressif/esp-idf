# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
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
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_gpio(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='gpio')


@pytest.mark.generic
@pytest.mark.parametrize('config', CONFIGS, indirect=True)
@idf_parametrize(
    'target', ['esp32', 'esp32s2', 'esp32s3', 'esp32c6', 'esp32h2', 'esp32p4', 'esp32c5'], indirect=['target']
)
def test_rtc_io(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='rtcio')
