# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.host_test
@pytest.mark.qemu
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_err_codes_check(dut: IdfDut) -> None:
    dut.expect_exact('SUCCESS: All error codes are correctly registered.', timeout=120)


@pytest.mark.host_test
@idf_parametrize('target', ['linux'], indirect=['target'])
def test_err_codes_check_linux(dut: IdfDut) -> None:
    dut.expect_exact('SUCCESS: All error codes are correctly registered.', timeout=120)


@pytest.mark.host_test
@pytest.mark.macos
@idf_parametrize('target', ['linux'], indirect=['target'])
def test_err_codes_check_macos(dut: IdfDut) -> None:
    dut.expect_exact('SUCCESS: All error codes are correctly registered.', timeout=120)
