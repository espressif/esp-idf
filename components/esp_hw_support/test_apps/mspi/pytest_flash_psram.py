# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import os
import pathlib

import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize

MSPI_F8R8_configs = [
    p.name.replace('sdkconfig.ci.', '')
    for p in pathlib.Path(os.path.dirname(__file__)).glob('sdkconfig.ci.esp32s3_f8r8*')
]


@pytest.mark.MSPI_F8R8
@pytest.mark.parametrize('config', MSPI_F8R8_configs, indirect=True)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_flash8_psram8(dut: IdfDut) -> None:
    dut.run_all_single_board_cases()


# For F4R8 board (Quad Flash and Octal PSRAM)
MSPI_F4R8_configs = [
    p.name.replace('sdkconfig.ci.', '')
    for p in pathlib.Path(os.path.dirname(__file__)).glob('sdkconfig.ci.esp32s3_f4r8*')
]


@pytest.mark.MSPI_F4R8
@pytest.mark.parametrize('config', MSPI_F4R8_configs, indirect=True)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_flash4_psram8(dut: IdfDut) -> None:
    dut.run_all_single_board_cases()


# For F4R4 board (Quad Flash and Quad PSRAM)
MSPI_F4R4_configs = [
    p.name.replace('sdkconfig.ci.', '')
    for p in pathlib.Path(os.path.dirname(__file__)).glob('sdkconfig.ci.esp32s3_f4r4*')
]


@pytest.mark.MSPI_F4R4
@pytest.mark.parametrize('config', MSPI_F4R4_configs, indirect=True)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_flash4_psram4(dut: IdfDut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.flash_120m
@pytest.mark.parametrize(
    'config',
    [
        'esp32p4_120sdr_200ddr',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_flash_psram_esp32p4(dut: IdfDut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        '120sdr_120sdr',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c5', 'esp32c61'], indirect=['target'])
def test_flash_psram_120sdr_120sdr(dut: IdfDut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.parametrize(
    'config',
    [
        'generic_timing_tuning_log_safe',
    ],
    indirect=True,
)
@idf_parametrize(
    'target,markers',
    [
        # S3 has no flash support auto suspend, this test is not applicable
        (
            'esp32p4',
            (pytest.mark.generic,),
        ),
        (
            'esp32c5',
            (pytest.mark.generic,),
        ),
        (
            'esp32c61',
            (pytest.mark.generic,),
        ),
    ],
    indirect=['target'],
)
def test_flash_psram_generic(dut: IdfDut) -> None:
    dut.run_all_single_board_cases()
