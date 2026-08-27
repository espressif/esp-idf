# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('config', ['default'], indirect=['config'])
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_cache_panic(dut: Dut) -> None:
    dut.run_all_single_board_cases()


@pytest.mark.generic
@idf_parametrize('config', ['panic_iram'], indirect=['config'])
@idf_parametrize('target', ['esp32p4', 'esp32s31'], indirect=['target'])
def test_cache_panic_axi(dut: Dut) -> None:
    dut.run_all_single_board_cases()
