# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@idf_parametrize('target', ['supported_targets'], indirect=['target'])
def test_cmakev2_import_lib_direct(dut: IdfDut) -> None:
    dut.expect_exact('lwjson library imported directly (downloaded, built, linked) without IDF component wrapper')
    dut.expect_exact('Parsing JSON string...')
    dut.expect_exact('Device name: ESP32')
    dut.expect_exact('Number of cores: 2')
    dut.expect_exact('Features:')
    dut.expect_exact('- WiFi')
    dut.expect_exact('- Bluetooth')
    dut.expect_exact('- GPIO')
    dut.expect_exact('Specifications:')
    dut.expect_exact('Flash: 4MB')
    dut.expect_exact('RAM: 520KB')
    dut.expect_exact('Example complete!')
