# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.host_test
@idf_parametrize('target', ['linux'], indirect=['target'])
def test_linux_full_build(dut: IdfDut) -> None:
    dut.expect_exact('Linux full component build test done')
