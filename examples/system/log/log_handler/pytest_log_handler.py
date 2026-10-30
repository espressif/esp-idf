# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


@pytest.mark.generic
@pytest.mark.parametrize('config', ['v1', 'v2'], indirect=True)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_log_handler(dut: Dut) -> None:
    dut.expect_exact('APP: Startup complete', timeout=60)
    dut.expect_exact('SENSOR: Temperature: 25 C')
    dut.expect_exact('WIFI: Connection lost, retry=1')
    dut.expect_exact('SENSOR: Read failed: error=-1')

    dut.expect_exact('--- /logs/warnings.log ---')
    dut.expect_exact('Connection lost, retry=1')
    dut.expect_exact('Read failed: error=-1')
    dut.expect_exact('--- /logs/sensor.log ---')
    dut.expect_exact('Temperature: 25 C')
    dut.expect_exact('Read failed: error=-1')
    dut.expect_exact('--- /logs/security.log ---')
    dut.expect_exact('Audit started')
    dut.expect_exact('Access attempt requires review')
    dut.expect_exact('Access denied')

    dut.expect_exact('Log handler example complete')
