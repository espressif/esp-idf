# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import os.path
from typing import Tuple

import pexpect
import pytest
from pytest_embedded_idf.dut import IdfDut


# Case 1: BLE power save test
@pytest.mark.esp32c6
@pytest.mark.esp32h2
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.esp32
@pytest.mark.wifi_two_dut
@pytest.mark.parametrize(
    'count, app_path', [
        (2,
         f'{os.path.join(os.path.dirname(__file__), "power_save")}|{os.path.join(os.path.dirname(__file__), "blecent")}'),
    ],
    indirect=True,
)
def test_power_save_conn(app_path: str, dut: Tuple[IdfDut, IdfDut]) -> None:
    peripheral = dut[0]
    central = dut[1]

    peripheral.expect('NimBLE_BLE_PRPH: BLE Host Task Started', timeout=30)
    central.expect('NimBLE_BLE_CENT: BLE Host Task Started', timeout=30)
    peripheral.expect('Returned from app_main()', timeout=30)
    central.expect('Returned from app_main()', timeout=30)
    central.expect('Connection established', timeout=30)
    peripheral.expect('connection established; status=0', timeout=30)
    output = peripheral.expect(pexpect.TIMEOUT, timeout=30)
    assert 'rst:' not in str(output) and 'boot:' not in str(output)


# Case 2: BLE power save test for ESP32C2
@pytest.mark.esp32c2
@pytest.mark.wifi_two_dut
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'config, count, app_path, baud', [
        ('esp32c2_xtal26m', 2,
         f'{os.path.join(os.path.dirname(__file__), "power_save")}|{os.path.join(os.path.dirname(__file__), "blecent")}',
         '74880'),
    ],
    indirect=True,
)
def test_power_save_conn_esp32c2_26mhz(dut: Tuple[IdfDut, IdfDut]) -> None:
    peripheral = dut[0]
    central = dut[1]

    peripheral.expect('NimBLE_BLE_PRPH: BLE Host Task Started', timeout=5)
    central.expect('NimBLE_BLE_CENT: BLE Host Task Started', timeout=5)
    peripheral.expect('Returned from app_main()', timeout=5)
    central.expect('Returned from app_main()', timeout=5)
    central.expect('Connection established', timeout=30)
    peripheral.expect('connection established; status=0', timeout=30)
    output = peripheral.expect(pexpect.TIMEOUT, timeout=30)
    assert 'rst:' not in str(output) and 'boot:' not in str(output)
