# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.temp_skip_ci(targets=['esp32s3'], reason='skip due to duplication with test_ledc_psram')
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
        'release',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_ledc(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(reset=True)


@pytest.mark.octal_psram
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
        'release',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32s3'], indirect=['target'])
def test_ledc_psram(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(reset=True)


@pytest.mark.temp_skip_ci(targets=['esp32s3'], reason='s3 multi device runner has no psram')
@pytest.mark.generic_multi_device
@pytest.mark.parametrize(
    'count, config',
    [
        (2, 'iram_safe'),
        (2, 'release'),
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_ledc_multi_device(case_tester) -> None:  # type: ignore
    case_tester.run_all_multi_dev_cases(reset=True)
