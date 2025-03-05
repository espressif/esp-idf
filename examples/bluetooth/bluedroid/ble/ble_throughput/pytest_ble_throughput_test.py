# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import os.path
import time
from typing import Tuple

import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize
# Case 1: gatt write throughput test(EXAMPLE_CI_ID = 2)


@pytest.mark.wifi_two_dut
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            f'{os.path.join(os.path.dirname(__file__), "throughput_server")}|{os.path.join(os.path.dirname(__file__), "throughput_client")}',
            'write',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize(
    'target', ['esp32', 'esp32c3', 'esp32c6', 'esp32c61', 'esp32c5', 'esp32h2', 'esp32s3'], indirect=['target']
)
def test_gatt_write_throughput(app_path: str, dut: Tuple[IdfDut, IdfDut]) -> None:
    server = dut[0]
    client = dut[1]
    client_addr = (
        client.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )
    server_addr = (
        server.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )

    client.expect_exact('GATT client register, status 0', timeout=30)
    server.expect_exact('GATT server register, status 0', timeout=30)
    server.expect_exact('Advertising start successfully', timeout=30)
    client.expect_exact('Scanning start successfully', timeout=30)
    client.expect_exact('Device found ', timeout=30)
    server.expect_exact('Service start, status 0', timeout=30)
    server.expect_exact(f'Connected, conn_id 0, remote {client_addr}', timeout=30)
    client.expect_exact(f'Connected, conn_id 0, remote {server_addr}', timeout=30)
    client.expect_exact('MTU exchange, status 0, MTU 517', timeout=30)
    server.expect_exact('MTU exchange, MTU 517', timeout=30)
    client.expect_exact('Service search complete', timeout=30)
    start_time = time.time()
    while time.time() - start_time < 30:
        throughput = int(server.expect(r'GATTC write Bit rate = (\d+) Byte/s', timeout=30).group(1).decode('utf8'))
        assert throughput > 50000 or throughput < 95000


# Case 2: gatt write throughput test for ESP32C2 26mhz xtal(EXAMPLE_CI_ID = 2)
@pytest.mark.wifi_two_dut
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, target, baud, app_path, config, erase_nvs',
    [
        (
            2,
            'esp32c2|esp32c2',
            '74880',
            f'{os.path.join(os.path.dirname(__file__), "throughput_server")}|{os.path.join(os.path.dirname(__file__), "throughput_client")}',
            'esp32c2_xtal26m_write',
            'y',
        ),
    ],
    indirect=True,
)
def test_c2_26mhz_xtal_write_throughput(app_path: str, dut: Tuple[IdfDut, IdfDut]) -> None:
    server = dut[0]
    client = dut[1]
    client_addr = (
        client.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )
    server_addr = (
        server.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )

    client.expect_exact('GATT client register, status 0', timeout=30)
    server.expect_exact('GATT server register, status 0', timeout=30)
    server.expect_exact('Advertising start successfully', timeout=30)
    client.expect_exact('Scanning start successfully', timeout=30)
    client.expect_exact('Device found ', timeout=30)
    server.expect_exact('Service start, status 0', timeout=30)
    server.expect_exact(f'Connected, conn_id 0, remote {client_addr}', timeout=30)
    client.expect_exact(f'Connected, conn_id 0, remote {server_addr}', timeout=30)
    client.expect_exact('MTU exchange, status 0, MTU 517', timeout=30)
    server.expect_exact('MTU exchange, MTU 517', timeout=30)
    client.expect_exact('Service search complete', timeout=30)
    start_time = time.time()
    while time.time() - start_time < 30:
        throughput = int(server.expect(r'GATTC write Bit rate = (\d+) Byte/s', timeout=30).group(1).decode('utf8'))
        assert throughput > 50000 or throughput < 95000


# Case 3: gatt notify throughput test(EXAMPLE_CI_ID = 1)
@pytest.mark.wifi_two_dut
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            f'{os.path.join(os.path.dirname(__file__), "throughput_server")}|{os.path.join(os.path.dirname(__file__), "throughput_client")}',
            'notify',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize(
    'target', ['esp32', 'esp32c3', 'esp32c6', 'esp32c61', 'esp32c5', 'esp32h2', 'esp32s3'], indirect=['target']
)
def test_gatt_notify_throughput(app_path: str, dut: Tuple[IdfDut, IdfDut]) -> None:
    server = dut[0]
    client = dut[1]
    client_addr = (
        client.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )
    server_addr = (
        server.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )

    client.expect_exact('GATT client register, status 0', timeout=30)
    server.expect_exact('GATT server register, status 0', timeout=30)
    server.expect_exact('Advertising start successfully', timeout=30)
    client.expect_exact('Scanning start successfully', timeout=30)
    client.expect_exact('Device found ', timeout=30)
    server.expect_exact('Service start, status 0', timeout=30)
    server.expect_exact(f'Connected, conn_id 0, remote {client_addr}', timeout=30)
    client.expect_exact(f'Connected, conn_id 0, remote {server_addr}', timeout=30)
    client.expect_exact('MTU exchange, status 0, MTU 517', timeout=30)
    server.expect_exact('MTU exchange, MTU 517', timeout=30)
    client.expect_exact('Service search complete', timeout=30)
    client.expect_exact('Notification register successfully', timeout=30)
    server.expect_exact('Notification enable', timeout=30)
    start_time = time.time()
    while time.time() - start_time < 30:
        throughput = int(client.expect(r'Notify Bit rate = (\d+) Byte/s', timeout=30).group(1).decode('utf8'))
        assert throughput > 50000 or throughput < 95000


# Case 4: gatt notify throughput test for ESP32C2 26mhz xtal(EXAMPLE_CI_ID = 1)
@pytest.mark.wifi_two_dut
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, target, baud, app_path, config, erase_nvs',
    [
        (
            2,
            'esp32c2|esp32c2',
            '74880',
            f'{os.path.join(os.path.dirname(__file__), "throughput_server")}|{os.path.join(os.path.dirname(__file__), "throughput_client")}',
            'esp32c2_xtal26m_notify',
            'y',
        ),
    ],
    indirect=True,
)
def test_c2_26mhz_xtal_notify_throughput(app_path: str, dut: Tuple[IdfDut, IdfDut]) -> None:
    server = dut[0]
    client = dut[1]
    client_addr = (
        client.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )
    server_addr = (
        server.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )

    client.expect_exact('GATT client register, status 0', timeout=30)
    server.expect_exact('GATT server register, status 0', timeout=30)
    server.expect_exact('Advertising start successfully', timeout=30)
    client.expect_exact('Scanning start successfully', timeout=30)
    client.expect_exact('Device found ', timeout=30)
    server.expect_exact('Service start, status 0', timeout=30)
    server.expect_exact(f'Connected, conn_id 0, remote {client_addr}', timeout=30)
    client.expect_exact(f'Connected, conn_id 0, remote {server_addr}', timeout=30)
    client.expect_exact('MTU exchange, status 0, MTU 517', timeout=30)
    server.expect_exact('MTU exchange, MTU 517', timeout=30)
    client.expect_exact('Service search complete', timeout=30)
    client.expect_exact('Notification register successfully', timeout=30)
    server.expect_exact('Notification enable', timeout=30)
    start_time = time.time()
    while time.time() - start_time < 30:
        throughput = int(client.expect(r'Notify Bit rate = (\d+) Byte/s', timeout=30).group(1).decode('utf8'))
        assert throughput > 50000 or throughput < 95000
