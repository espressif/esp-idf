# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import os.path
from typing import Tuple

import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize
# Case 1: ble50 security client and ble50 security server test
# EXAMPLE_CI_ID=6


@pytest.mark.wifi_two_dut
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            f'{os.path.join(os.path.dirname(__file__), "ble50_security_server")}|{os.path.join(os.path.dirname(__file__), "ble50_security_client")}',
            'name',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c3', 'esp32c6', 'esp32c5', 'esp32h2', 'esp32s3', 'esp32c61'], indirect=['target'])
def test_ble50_security_func(app_path: str, dut: Tuple[IdfDut, IdfDut]) -> None:
    server = dut[0]
    client = dut[1]
    client_addr = (
        client.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )
    server_addr = (
        server.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )

    server.expect_exact('Extended advertising params set, status 0', timeout=30)
    server.expect_exact('Extended advertising data set, status 0', timeout=30)
    server.expect_exact('Extended advertising start, status 0', timeout=30)
    client.expect_exact('Extended scanning start successfully', timeout=30)
    client.expect_exact(f'Connected, conn_id 0, remote {server_addr}', timeout=30)
    server.expect_exact(f'Connected, conn_id 0, remote {client_addr}', timeout=30)
    server.expect_exact('Pairing successfully', timeout=30)
    client.expect_exact('Pairing successfully', timeout=30)
    server.expect_exact('Bonded devices number 1', timeout=30)
    server.expect_exact('Characteristic write', timeout=30)
    client.expect_exact('Service discover complete', timeout=30)
    client.expect_exact('Service search complete', timeout=30)
    client.expect_exact('MTU exchange, status 0', timeout=30)
    client.expect_exact('Descriptor write successfully', timeout=30)


# Case 2: ble50 security client and ble50 security server test for ESP32C2 26mhz xtal
# EXAMPLE_CI_ID=6
@pytest.mark.wifi_two_dut
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, target, baud, app_path, config, erase_nvs',
    [
        (
            2,
            'esp32c2|esp32c2',
            '74880',
            f'{os.path.join(os.path.dirname(__file__), "ble50_security_server")}|{os.path.join(os.path.dirname(__file__), "ble50_security_client")}',
            'esp32c2_xtal26m',
            'y',
        ),
    ],
    indirect=True,
)
def test_c2_26mhz_xtal_ble50_security_func(app_path: str, dut: Tuple[IdfDut, IdfDut]) -> None:
    server = dut[0]
    client = dut[1]
    client_addr = (
        client.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )
    server_addr = (
        server.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )

    server.expect_exact('Extended advertising params set, status 0', timeout=30)
    server.expect_exact('Extended advertising data set, status 0', timeout=30)
    server.expect_exact('Extended advertising start, status 0', timeout=30)
    client.expect_exact('Extended scanning start successfully', timeout=30)
    client.expect_exact(f'Connected, conn_id 0, remote {server_addr}', timeout=30)
    server.expect_exact(f'Connected, conn_id 0, remote {client_addr}', timeout=30)
    server.expect_exact('Pairing successfully', timeout=30)
    client.expect_exact('Pairing successfully', timeout=30)
    server.expect_exact('Bonded devices number 1', timeout=30)
    server.expect_exact('Characteristic write', timeout=30)
    client.expect_exact('Service discover complete', timeout=30)
    client.expect_exact('Service search complete', timeout=30)
    client.expect_exact('MTU exchange, status 0', timeout=30)
    client.expect_exact('Descriptor write successfully', timeout=30)


# Case 3: period_adv and period_sync test
# EXAMPLE_CI_ID=8
@pytest.mark.wifi_two_dut
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            f'{os.path.join(os.path.dirname(__file__), "periodic_adv")}|{os.path.join(os.path.dirname(__file__), "periodic_sync")}',
            'name',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c3', 'esp32c6', 'esp32c5', 'esp32h2', 'esp32s3', 'esp32c61'], indirect=['target'])
def test_period_adv_sync_func(app_path: str, dut: Tuple[IdfDut, IdfDut]) -> None:
    adv_dut = dut[0]
    sync_dut = dut[1]

    adv_dut.expect_exact('Extended advertising params set, status 0', timeout=30)
    adv_dut.expect_exact('Extended advertising random address set, status 0', timeout=30)
    adv_dut.expect_exact('Extended advertising data set, status 0', timeout=30)
    adv_dut.expect_exact('Extended advertising start, status 0', timeout=30)
    adv_dut.expect_exact('Periodic advertising params set, status 0', timeout=30)
    adv_dut.expect_exact('Periodic advertising data set, status 0', timeout=30)
    adv_dut.expect_exact('Periodic advertising start, status 0', timeout=30)
    sync_dut.expect_exact('Extended scanning params set, status 0', timeout=30)
    sync_dut.expect_exact('Extended scanning start, status 0', timeout=30)
    sync_dut.expect_exact(f'Create sync with the peer device BE', timeout=30)
    sync_dut.expect_exact('Periodic advertising sync establish, status 0', timeout=30)
    sync_dut.expect_exact('Periodic adv report, sync handle ', timeout=30)


# Case 4: period_adv and period_sync test for ESP32C2 26mhz xtal
# EXAMPLE_CI_ID=8
@pytest.mark.wifi_two_dut
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, target, baud, app_path, config, erase_nvs',
    [
        (
            2,
            'esp32c2|esp32c2',
            '74880',
            f'{os.path.join(os.path.dirname(__file__), "periodic_adv")}|{os.path.join(os.path.dirname(__file__), "periodic_sync")}',
            'esp32c2_xtal26m',
            'y',
        ),
    ],
    indirect=True,
)
def test_c2_26mhz_xtal_period_adv_sync_func(app_path: str, dut: Tuple[IdfDut, IdfDut]) -> None:
    adv_dut = dut[0]
    sync_dut = dut[1]

    adv_dut.expect_exact('Extended advertising params set, status 0', timeout=30)
    adv_dut.expect_exact('Extended advertising random address set, status 0', timeout=30)
    adv_dut.expect_exact('Extended advertising data set, status 0', timeout=30)
    adv_dut.expect_exact('Extended advertising start, status 0', timeout=30)
    adv_dut.expect_exact('Periodic advertising params set, status 0', timeout=30)
    adv_dut.expect_exact('Periodic advertising data set, status 0', timeout=30)
    adv_dut.expect_exact('Periodic advertising start, status 0', timeout=30)
    sync_dut.expect_exact('Extended scanning params set, status 0', timeout=30)
    sync_dut.expect_exact('Extended scanning start, status 0', timeout=30)
    sync_dut.expect_exact(f'Create sync with the peer device BE', timeout=30)
    sync_dut.expect_exact('Periodic advertising sync establish, status 0', timeout=30)
    sync_dut.expect_exact('Periodic adv report, sync handle ', timeout=30)


# Case 5: ble50 security client and ble50 security server config test
# EXAMPLE_CI_ID=7
@pytest.mark.wifi_two_dut
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            f'{os.path.join(os.path.dirname(__file__), "ble50_security_server")}|{os.path.join(os.path.dirname(__file__), "ble50_security_client")}',
            'cfg_test',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c3', 'esp32c6', 'esp32c5', 'esp32h2', 'esp32s3', 'esp32c61'], indirect=['target'])
def test_ble50_security_config_func(app_path: str, dut: Tuple[IdfDut, IdfDut]) -> None:
    server = dut[0]
    client = dut[1]
    client_addr = (
        client.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )
    server_addr = (
        server.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )

    server.expect_exact('Extended advertising params set, status 0', timeout=30)
    server.expect_exact('Extended advertising data set, status 0', timeout=30)
    server.expect_exact('Extended advertising start, status 0', timeout=30)
    client.expect_exact('Extended scanning start successfully', timeout=30)
    client.expect_exact(f'Connected, conn_id 0, remote {server_addr}', timeout=30)
    server.expect_exact(f'Connected, conn_id 0, remote {client_addr}', timeout=30)
    server.expect_exact('Pairing successfully', timeout=30)
    client.expect_exact('Pairing successfully', timeout=30)
    server.expect_exact('Bonded devices number 1', timeout=30)
    server.expect_exact('Characteristic write', timeout=30)
    client.expect_exact('Service discover complete', timeout=30)
    client.expect_exact('Service search complete', timeout=30)
    client.expect_exact('MTU exchange, status 0', timeout=30)
    client.expect_exact('Descriptor write successfully', timeout=30)
    client.serial.erase_flash()
    server.serial.erase_flash()


# Case 6: ble50 security client and ble50 security server config test for ESP32C2 26mhz xtal
# EXAMPLE_CI_ID=7
@pytest.mark.wifi_two_dut
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, target, baud, app_path, config, erase_nvs',
    [
        (
            2,
            'esp32c2|esp32c2',
            '74880',
            f'{os.path.join(os.path.dirname(__file__), "ble50_security_server")}|{os.path.join(os.path.dirname(__file__), "ble50_security_client")}',
            'esp32c2_cfg_test',
            'y',
        ),
    ],
    indirect=True,
)
def test_c2_26mhz_xtal_ble50_security_config_func(app_path: str, dut: Tuple[IdfDut, IdfDut]) -> None:
    server = dut[0]
    client = dut[1]
    client_addr = (
        client.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )
    server_addr = (
        server.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )

    server.expect_exact('Extended advertising params set, status 0', timeout=30)
    server.expect_exact('Extended advertising data set, status 0', timeout=30)
    server.expect_exact('Extended advertising start, status 0', timeout=30)
    client.expect_exact('Extended scanning start successfully', timeout=30)
    client.expect_exact(f'Connected, conn_id 0, remote {server_addr}', timeout=30)
    server.expect_exact(f'Connected, conn_id 0, remote {client_addr}', timeout=30)
    server.expect_exact('Pairing successfully', timeout=30)
    client.expect_exact('Pairing successfully', timeout=30)
    server.expect_exact('Bonded devices number 1', timeout=30)
    server.expect_exact('Characteristic write', timeout=30)
    client.expect_exact('Service discover complete', timeout=30)
    client.expect_exact('Service search complete', timeout=30)
    client.expect_exact('MTU exchange, status 0', timeout=30)
    client.expect_exact('Descriptor write successfully', timeout=30)
    client.serial.erase_flash()
    server.serial.erase_flash()
