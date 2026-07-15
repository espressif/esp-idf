# SPDX-FileCopyrightText: 2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
from pathlib import Path

import pexpect
import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize

CUR_DIR = Path(__file__).parent.resolve()

THROUGHPUT_APP_PATH = f'{str(CUR_DIR / "throughput_server")}|{str(CUR_DIR / "throughput_client")}'
BLE50_THROUGHPUT_TARGETS = [
    'esp32c3',
    'esp32c6',
    'esp32c5',
    'esp32h2',
    'esp32s3',
    'esp32c61',
    'esp32h4',
    'esp32s31',
]


def _run_ble50_write_throughput_func(dut: tuple[IdfDut, IdfDut]) -> None:
    server = dut[0]
    client = dut[1]

    server_addr = (
        server.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )
    client_addr = (
        client.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )
    name = server.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    peer_name = client.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    assert peer_name == name, (
        'central and peripheral must use the same CI device name from '
        'esp_bluedroid_get_example_name(); mismatch means sdkconfig.ci or CI ID is wrong'
    )

    client.expect_exact('GATT client register, status 0', timeout=30)
    server.expect_exact('GATT server register, status 0', timeout=30)
    server.expect_exact('Extended advertising params set, status 0', timeout=30)
    server.expect_exact('Extended advertising data set, status 0', timeout=30)
    server.expect_exact('Service start, status 0', timeout=30)
    server.expect_exact('Extended advertising start, status 0', timeout=30)
    client.expect_exact('Extended scanning start successfully', timeout=30)

    found_addr = (
        client.expect(
            rf'Found device: (([0-9a-fA-F]{{2}}:){{5}}[0-9a-fA-F]{{2}}), name: {name}',
            timeout=60,
        )
        .group(1)
        .decode('utf8')
    )
    assert found_addr == server_addr, (
        'scan must report the peripheral board MAC, not another advertiser with the same CI name'
    )
    client.expect_exact('Target device found, connecting...', timeout=30)

    connected_remote = (
        client.expect(
            r'Connected, conn_id 0, remote (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})',
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert connected_remote == server_addr, (
        'central must connect to the peripheral board MAC, not a colliding device in RF environment'
    )
    server_remote = (
        server.expect(
            r'Connected, conn_id \d+, remote (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})',
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert server_remote == client_addr, 'peripheral must see central board MAC as connection peer'

    client.expect_exact('MTU exchange, status 0, MTU 517', timeout=30)
    server.expect_exact('MTU exchange, MTU 517', timeout=30)
    client.expect_exact('Service search complete', timeout=30)
    client.expect_exact('Descriptor write successfully', timeout=30)
    server.expect(r'GATTC write Bit rate = (\d+) Byte/s', timeout=30).group(1).decode('utf8')

    out_client = client.expect(pexpect.TIMEOUT, timeout=10)
    out_server = server.expect(pexpect.TIMEOUT, timeout=10)
    assert 'rst:' not in str(out_client) and 'boot:' not in str(out_client)
    assert 'rst:' not in str(out_server) and 'boot:' not in str(out_server)
    assert 'Disconnected' not in str(out_client)
    assert 'Disconnected' not in str(out_server)


def _run_ble50_notify_throughput_func(dut: tuple[IdfDut, IdfDut]) -> None:
    server = dut[0]
    client = dut[1]

    server_addr = (
        server.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )
    client_addr = (
        client.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )
    name = server.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    peer_name = client.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    assert peer_name == name, (
        'central and peripheral must use the same CI device name from '
        'esp_bluedroid_get_example_name(); mismatch means sdkconfig.ci or CI ID is wrong'
    )

    client.expect_exact('GATT client register, status 0', timeout=30)
    server.expect_exact('GATT server register, status 0', timeout=30)
    server.expect_exact('Extended advertising params set, status 0', timeout=30)
    server.expect_exact('Extended advertising data set, status 0', timeout=30)
    server.expect_exact('Service start, status 0', timeout=30)
    server.expect_exact('Extended advertising start, status 0', timeout=30)
    client.expect_exact('Extended scanning start successfully', timeout=30)

    found_addr = (
        client.expect(
            rf'Found device: (([0-9a-fA-F]{{2}}:){{5}}[0-9a-fA-F]{{2}}), name: {name}',
            timeout=60,
        )
        .group(1)
        .decode('utf8')
    )
    assert found_addr == server_addr, (
        'scan must report the peripheral board MAC, not another advertiser with the same CI name'
    )
    client.expect_exact('Target device found, connecting...', timeout=30)

    connected_remote = (
        client.expect(
            r'Connected, conn_id 0, remote (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})',
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert connected_remote == server_addr, (
        'central must connect to the peripheral board MAC, not a colliding device in RF environment'
    )
    server_remote = (
        server.expect(
            r'Connected, conn_id \d+, remote (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})',
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert server_remote == client_addr, 'peripheral must see central board MAC as connection peer'

    client.expect_exact('MTU exchange, status 0, MTU 517', timeout=30)
    server.expect_exact('MTU exchange, MTU 517', timeout=30)
    client.expect_exact('Service search complete', timeout=30)
    client.expect_exact('Notification register successfully', timeout=30)
    client.expect_exact('Descriptor write successfully', timeout=30)
    server.expect_exact('Notification enable', timeout=30)

    client.expect(r'Notify Bit rate = (\d+) Byte/s', timeout=30).group(1).decode('utf8')

    out_client = client.expect(pexpect.TIMEOUT, timeout=10)
    out_server = server.expect(pexpect.TIMEOUT, timeout=10)
    assert 'rst:' not in str(out_client) and 'boot:' not in str(out_client)
    assert 'rst:' not in str(out_server) and 'boot:' not in str(out_server)
    assert 'Disconnected' not in str(out_client)
    assert 'Disconnected' not in str(out_server)


# Case 1: ble50 gatt write throughput (EXAMPLE_CI_ID=24)
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            THROUGHPUT_APP_PATH,
            'write',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', BLE50_THROUGHPUT_TARGETS, indirect=['target'])
def test_ble50_gatt_write_throughput(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_ble50_write_throughput_func(dut)


# Case 2: ble50 gatt write throughput for ESP32C2 26mhz xtal (EXAMPLE_CI_ID=24)
@pytest.mark.two_duts
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, target, baud, app_path, config, erase_nvs',
    [
        (
            2,
            'esp32c2|esp32c2',
            '74880',
            THROUGHPUT_APP_PATH,
            'esp32c2_xtal26m_write',
            'y',
        ),
    ],
    indirect=True,
)
def test_c2_26mhz_xtal_ble50_write_throughput(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_ble50_write_throughput_func(dut)


# Case 3: ble50 gatt notify throughput (EXAMPLE_CI_ID=23)
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            THROUGHPUT_APP_PATH,
            'notify',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', BLE50_THROUGHPUT_TARGETS, indirect=['target'])
def test_ble50_gatt_notify_throughput(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_ble50_notify_throughput_func(dut)


# Case 4: ble50 gatt notify throughput for ESP32C2 26mhz xtal (EXAMPLE_CI_ID=23)
@pytest.mark.two_duts
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, target, baud, app_path, config, erase_nvs',
    [
        (
            2,
            'esp32c2|esp32c2',
            '74880',
            THROUGHPUT_APP_PATH,
            'esp32c2_xtal26m_notify',
            'y',
        ),
    ],
    indirect=True,
)
def test_c2_26mhz_xtal_ble50_notify_throughput(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_ble50_notify_throughput_func(dut)
