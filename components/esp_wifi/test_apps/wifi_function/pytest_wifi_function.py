# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.esp32s2
@pytest.mark.esp32s3
@pytest.mark.esp32c6
@pytest.mark.esp32c61
@pytest.mark.esp32c5
# @pytest.mark.esp32c2  # esp32c2 are using xtal_26mhz
@pytest.mark.generic
def test_wifi_unit_test(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32c2
@pytest.mark.xtal_26mhz
@pytest.mark.generic
@pytest.mark.parametrize(
    'config, baud',
    [
        ('esp32c2_xtal26m', '74880'),
    ],
    indirect=True,
)
def test_wifi_unit_test_esp32c2_xtal26m(dut: Dut) -> None:
    dut.run_all_single_board_cases()
