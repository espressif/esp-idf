# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize(
    'target', ['esp32h2', 'esp32c3', 'esp32s2', 'esp32s3', 'esp32c6', 'esp32c61', 'esp32c5'], indirect=['target']
)
def test_external_coex_unit_test(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.xtal_26mhz
@pytest.mark.generic
@pytest.mark.parametrize(
    'config, baud',
    [
        ('esp32c2_xtal26m', '74880'),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c2'], indirect=['target'])
def test_external_coex_unit_test_esp32c2_xtal26m(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32c2eco4
@pytest.mark.xtal_26mhz
@pytest.mark.generic
@pytest.mark.parametrize(
    'config, baud',
    [
        ('esp32c2eco4_xtal26m', '74880'),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c2'], indirect=['target'])
def test_external_coex_unit_test_esp32c2eco4_xtal26m(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.esp32c3eco7
@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'esp32c3eco7',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_external_coex_unit_test_esp32c3eco7(dut: Dut) -> None:
    dut.run_all_single_board_cases()
