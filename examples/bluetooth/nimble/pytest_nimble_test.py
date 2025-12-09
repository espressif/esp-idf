# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
from pathlib import Path

import pexpect
import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize

CUR_DIR = Path(__file__).parent.resolve()


# Case 1: BLE power save test
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path',
    [
        (
            2,
            f'{str(CUR_DIR / "power_save")}|{str(CUR_DIR / "blecent")}',
        ),
    ],
    indirect=True,
)
@idf_parametrize(
    'target', ['esp32c6', 'esp32h2', 'esp32c3', 'esp32s3', 'esp32c5', 'esp32c61', 'esp32'], indirect=['target']
)
def test_power_save_conn(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
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
@pytest.mark.two_duts
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'config, count, app_path, baud',
    [
        (
            'esp32c2_xtal26m',
            2,
            f'{str(CUR_DIR / "power_save")}|{str(CUR_DIR / "blecent")}',
            '74880',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c2'], indirect=['target'])
def test_power_save_conn_esp32c2_26mhz(dut: tuple[IdfDut, IdfDut]) -> None:
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


# Case 2: BLE power save test for ESP32C2ECO4
@pytest.mark.two_duts
@pytest.mark.xtal_26mhz
@pytest.mark.esp32c2eco4
@pytest.mark.parametrize(
    'config, count, app_path, baud',
    [
        (
            'esp32c2eco4_xtal26m',
            2,
            f'{str(CUR_DIR / "power_save")}|{str(CUR_DIR / "blecent")}',
            '74880',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c2'], indirect=['target'])
def test_power_save_conn_esp32c2eco4(dut: tuple[IdfDut, IdfDut]) -> None:
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


# Case 2: BLE power save test for ESP32C3ECO7
@pytest.mark.two_duts
@pytest.mark.esp32c3eco7
@pytest.mark.parametrize(
    'config, count, app_path',
    [
        (
            'esp32c3eco7',
            2,
            f'{str(CUR_DIR / "power_save")}|{str(CUR_DIR / "blecent")}',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_power_save_conn_esp32c3eco7(dut: tuple[IdfDut, IdfDut]) -> None:
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
