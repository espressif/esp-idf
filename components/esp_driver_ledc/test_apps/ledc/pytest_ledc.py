# SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf import IdfDut


@pytest.mark.supported_targets
@pytest.mark.temp_skip_ci(targets=['esp32s3'],
                          reason='skip due to duplication with test_ledc_psram')
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
        'release',
    ],
    indirect=True,
)
def test_ledc(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(reset=True)


@pytest.mark.esp32s3
@pytest.mark.octal_psram
@pytest.mark.parametrize(
    'config',
    [
        'iram_safe',
        'release',
    ],
    indirect=True,
)
def test_ledc_psram(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(reset=True)


@pytest.mark.supported_targets
@pytest.mark.temp_skip_ci(targets=['esp32s3', 'esp32c61'],
                          reason='s3 multi device runner has no psram, c61 lack of runner IDF-10949')
@pytest.mark.generic_multi_device
@pytest.mark.parametrize(
    'count, config',
    [
        (2, 'iram_safe',),
        (2, 'release',),
    ],
    indirect=True
)
def test_ledc_multi_device(case_tester) -> None:        # type: ignore
    case_tester.run_all_multi_dev_cases(reset=True)
