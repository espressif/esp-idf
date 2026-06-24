# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.host_test
@idf_parametrize('target', ['linux'], indirect=['target'])
def test_sys_init_fn_linux(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(timeout=60)


@pytest.mark.host_test
@pytest.mark.macos
@idf_parametrize('target', ['linux'], indirect=['target'])
def test_sys_init_fn_macos(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(timeout=60)
