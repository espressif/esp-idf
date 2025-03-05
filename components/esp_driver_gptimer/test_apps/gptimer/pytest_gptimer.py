# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize(
    'config',
    [
        'cache_safe',
        'release',
    ],
    indirect=True,
)
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_gptimer(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'config, baud',
    [
        ('esp32c2_xtal26m', '74880'),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c2'], indirect=['target'])
def test_gptimer_esp32c2_xtal_26mhz(dut: Dut) -> None:
    dut.run_all_single_board_cases()
