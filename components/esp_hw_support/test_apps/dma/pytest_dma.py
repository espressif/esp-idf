# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32s2
@pytest.mark.esp32c2
@pytest.mark.esp32c3
@pytest.mark.esp32c5
@pytest.mark.esp32c6
@pytest.mark.esp32c61
@pytest.mark.esp32h2
@pytest.mark.esp32p4
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
def test_dma(dut: Dut) -> None:
    dut.run_all_single_board_cases(reset=True)


@pytest.mark.esp32s3
@pytest.mark.octal_psram
@pytest.mark.parametrize(
    'config',
    [
        'release',
    ],
    indirect=True,
)
def test_dma_psram(dut: Dut) -> None:
    dut.run_all_single_board_cases(reset=True)
