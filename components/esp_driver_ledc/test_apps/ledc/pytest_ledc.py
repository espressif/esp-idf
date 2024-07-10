# SPDX-FileCopyrightText: 2021-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf import IdfDut


@pytest.mark.supported_targets
# TODO: [ESP32P4] IDF-8969, [ESP32C5] IDF-10333
@pytest.mark.temp_skip_ci(targets=['esp32s3', 'esp32p4', 'esp32c5'],
                          reason='skip due to duplication with test_ledc_psram, p4 TBD, c5 test failed')
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
    dut.run_all_single_board_cases()


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
    dut.run_all_single_board_cases()
