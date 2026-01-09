# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
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
    ['esp32', 'esp32c3', 'esp32c6', 'esp32h2', 'esp32s2', 'esp32s3', 'esp32h2', 'esp32p4'],
    indirect=['target'],
)
def test_sdm(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='sdm')


@pytest.mark.generic
@pytest.mark.parametrize('config', ['iram_safe'], indirect=True)
@idf_parametrize('target', ['esp32c5'], indirect=['target'])
def test_sdm_esp32c5(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='sdm')


@pytest.mark.generic
@pytest.mark.esp32c5_eco3
@pytest.mark.parametrize('config', ['release'], indirect=True)
@idf_parametrize('target', ['esp32c5'], indirect=['target'])
def test_sdm_esp32c5_eco3(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='sdm')
