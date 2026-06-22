# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
from pathlib import Path

import pexpect
import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize

CUR_DIR = Path(__file__).parent.resolve()


# Case 1: ble50 security client and ble50 security server test (config: name, min_bin, log_off)
# EXAMPLE_CI_ID=6
BLE50_SECURITY_APP_PATH = f'{str(CUR_DIR / "ble50_security_server")}|{str(CUR_DIR / "ble50_security_client")}'
BLE50_SECURITY_CI_CONFIGS = ['name']


@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [(2, BLE50_SECURITY_APP_PATH, c, 'y') for c in BLE50_SECURITY_CI_CONFIGS],
    indirect=True,
)
@idf_parametrize(
    'target',
    ['esp32c3', 'esp32c6', 'esp32c5', 'esp32h2', 'esp32s3', 'esp32c61', 'esp32h4', 'esp32s31'],
    indirect=['target'],
)
def test_ble50_security_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
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
@pytest.mark.two_duts
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, target, baud, app_path, config, erase_nvs',
    [
        (
            2,
            'esp32c2|esp32c2',
            '74880',
            f'{str(CUR_DIR / "ble50_security_server")}|{str(CUR_DIR / "ble50_security_client")}',
            'esp32c2_xtal26m',
            'y',
        ),
    ],
    indirect=True,
)
def test_c2_26mhz_xtal_ble50_security_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
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
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            f'{str(CUR_DIR / "periodic_adv")}|{str(CUR_DIR / "periodic_sync")}',
            'name',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    ['esp32c3', 'esp32c6', 'esp32c5', 'esp32h2', 'esp32s3', 'esp32c61', 'esp32h4', 'esp32s31'],
    indirect=['target'],
)
def test_period_adv_sync_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
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
    sync_dut.expect_exact('Create sync with the peer device BE', timeout=30)
    sync_dut.expect_exact('Periodic advertising sync establish, status 0', timeout=30)
    sync_dut.expect_exact('Periodic adv report, sync handle ', timeout=30)


# Case 4: period_adv and period_sync test for ESP32C2 26mhz xtal
# EXAMPLE_CI_ID=8
@pytest.mark.two_duts
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, target, baud, app_path, config, erase_nvs',
    [
        (
            2,
            'esp32c2|esp32c2',
            '74880',
            f'{str(CUR_DIR / "periodic_adv")}|{str(CUR_DIR / "periodic_sync")}',
            'esp32c2_xtal26m',
            'y',
        ),
    ],
    indirect=True,
)
def test_c2_26mhz_xtal_period_adv_sync_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
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
    sync_dut.expect_exact('Create sync with the peer device BE', timeout=30)
    sync_dut.expect_exact('Periodic advertising sync establish, status 0', timeout=30)
    sync_dut.expect_exact('Periodic adv report, sync handle ', timeout=30)


# Case 5: ble50 security client and ble50 security server config test
# EXAMPLE_CI_ID=7
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            f'{str(CUR_DIR / "ble50_security_server")}|{str(CUR_DIR / "ble50_security_client")}',
            'cfg_test',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize(
    'target',
    ['esp32c3', 'esp32c6', 'esp32c5', 'esp32h2', 'esp32s3', 'esp32c61', 'esp32h4', 'esp32s31'],
    indirect=['target'],
)
def test_ble50_security_config_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
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
@pytest.mark.two_duts
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, target, baud, app_path, config, erase_nvs',
    [
        (
            2,
            'esp32c2|esp32c2',
            '74880',
            f'{str(CUR_DIR / "ble50_security_server")}|{str(CUR_DIR / "ble50_security_client")}',
            'esp32c2_cfg_test',
            'y',
        ),
    ],
    indirect=True,
)
def test_c2_26mhz_xtal_ble50_security_config_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
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


def _run_ble_power_control_func(dut: tuple[IdfDut, IdfDut]) -> None:
    peripheral = dut[0]
    central = dut[1]

    central_addr = (
        central.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )
    peripheral_addr = (
        peripheral.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )

    peer_name = central.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    name = peripheral.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    assert peer_name == name

    peripheral.expect_exact('GATT server register, status 0,', timeout=30)
    peripheral.expect_exact('Service start, status 0', timeout=30)
    peripheral.expect_exact('Extended advertising params set, status 0, instance 0', timeout=30)
    peripheral.expect_exact('Extended advertising data set, status 0, instance 0', timeout=30)
    peripheral.expect_exact('Extended advertising start successfully, instance num 1', timeout=30)

    central.expect_exact('GATT client register, status 0', timeout=30)
    central.expect_exact('Extended scan started successfully', timeout=30)
    central.expect_exact(f'Found device: {peripheral_addr}, name: {name}', timeout=30)
    central.expect_exact(f'Connected, conn_id 0, hci_conn_handle 0, remote {peripheral_addr}', timeout=30)
    peripheral.expect_exact(f'Connected, conn_id 0, remote {central_addr}', timeout=30)

    central.expect_exact('Read local transmit power level request sent', timeout=30)
    central.expect_exact('Read remote transmit power level request sent', timeout=30)
    central.expect_exact('Transmit power reporting enabled', timeout=30)
    central.expect_exact('Path loss reporting parameters set', timeout=30)
    central.expect_exact('Path loss reporting enabled', timeout=30)
    central.expect_exact('Enhanced read transmit power level: conn_handle=0, status=0', timeout=30)
    central.expect_exact('Read remote transmit power level complete, status 0', timeout=30)
    central.expect_exact('Set transmit power reporting enable complete, status 0', timeout=30)
    central.expect_exact('Transmit power reporting event: conn_handle=0, status=0', timeout=30)
    central.expect_exact('Set path loss reporting parameters complete, status 0', timeout=30)
    central.expect_exact('Set path loss reporting enable complete, status 0', timeout=30)
    central.expect_exact('Path loss threshold event', timeout=30)

    peripheral.expect_exact('Read remote transmit power level request sent', timeout=30)
    peripheral.expect_exact('Transmit power reporting enabled', timeout=30)
    peripheral.expect_exact('Extended advertising stop successfully', timeout=30)
    peripheral.expect_exact('Read remote transmit power level complete, status 0', timeout=30)
    peripheral.expect_exact('Set transmit power reporting enable complete, status 0', timeout=30)
    peripheral.expect_exact('Transmit power reporting event', timeout=30)
    peripheral.expect_exact('Connection parameters updated', timeout=30)
    peripheral.expect_exact('Transmit power reporting event', timeout=30)

    central_output = central.expect(pexpect.TIMEOUT, timeout=10)
    peripheral_output = peripheral.expect(pexpect.TIMEOUT, timeout=10)
    assert 'rst:' not in str(central_output) and 'boot:' not in str(central_output)
    assert 'rst:' not in str(peripheral_output) and 'boot:' not in str(peripheral_output)
    assert 'Disconnected' not in str(central_output)
    assert 'Disconnected' not in str(peripheral_output)


# Case 7: ble power control peripheral and central test
# EXAMPLE_CI_ID=17
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.temp_skip_ci(targets=['esp32s31'], reason='ble bug # TODO: BLEQABR25-2374')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            f'{str(CUR_DIR / "ble_power_control_peripheral")}|{str(CUR_DIR / "ble_power_control_central")}',
            'name',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c6', 'esp32c5', 'esp32h2', 'esp32c61', 'esp32s31', 'esp32h4'], indirect=['target'])
def test_ble_power_control_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_ble_power_control_func(dut)


def _run_ble_conn_subrating_func(dut: tuple[IdfDut, IdfDut]) -> None:
    peripheral = dut[0]
    central = dut[1]

    central_addr = (
        central.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )
    peripheral_addr = (
        peripheral.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )

    name = peripheral.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    peer_name = central.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    assert peer_name == name, (
        'central and peripheral must use the same CI device name from '
        'esp_bluedroid_get_example_name(); mismatch means sdkconfig.ci or CI ID is wrong'
    )

    peripheral.expect_exact('GATT server register, status 0,', timeout=30)
    peripheral.expect_exact('create attribute table successfully', timeout=30)
    peripheral.expect_exact('Extended advertising params set, status 0, instance 0', timeout=30)
    peripheral.expect_exact('Service start, status 0', timeout=30)
    peripheral.expect_exact('Extended advertising data set, status 0, instance 0', timeout=30)
    peripheral.expect_exact('Extended advertising start successfully, instance num 1', timeout=30)

    central.expect_exact('GATT client register, status 0', timeout=30)
    central.expect_exact('Set default subrate complete, status 0', timeout=30)
    central.expect_exact('Extended scan parameters set, starting scan...', timeout=30)
    central.expect_exact('Extended scan started successfully', timeout=30)

    found_addr = (
        central.expect(
            rf'Found device: (([0-9a-fA-F]{{2}}:){{5}}[0-9a-fA-F]{{2}}), name: {name}',
            timeout=60,
        )
        .group(1)
        .decode('utf8')
    )
    assert found_addr == peripheral_addr, (
        'scan must report the peripheral board MAC, not another advertiser with the same CI name'
    )

    central.expect_exact('Target device found, connecting...', timeout=30)
    central.expect_exact('Extended scan stopped', timeout=30)

    connected_remote = (
        central.expect(
            r'Connected, conn_id \d+, remote (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})',
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert connected_remote == peripheral_addr, (
        'central must connect to the peripheral board MAC, not a colliding device in RF environment'
    )
    central.expect_exact('Subrate request sent successfully', timeout=30)

    peripheral_remote = (
        peripheral.expect(
            r'Connected, conn_id \d+, remote (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})',
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert peripheral_remote == central_addr, 'peripheral must see central board MAC as connection peer'

    central.expect_exact('GATT open OK, conn_id', timeout=30)
    peripheral.expect_exact('Extended advertising stop successfully', timeout=30)
    central.expect_exact('Subrate request complete, status 0', timeout=30)
    central.expect_exact('Connection subrating update, status 0', timeout=30)
    peripheral.expect_exact('Connection subrating update, status 0', timeout=30)

    central_output = central.expect(pexpect.TIMEOUT, timeout=10)
    peripheral_output = peripheral.expect(pexpect.TIMEOUT, timeout=10)
    assert 'rst:' not in str(central_output) and 'boot:' not in str(central_output)
    assert 'rst:' not in str(peripheral_output) and 'boot:' not in str(peripheral_output)
    assert 'Disconnected' not in str(central_output)
    assert 'Disconnected' not in str(peripheral_output)


# Case 8: ble connection subrating peripheral and central test
# EXAMPLE_CI_ID=18
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.temp_skip_ci(targets=['esp32s31'], reason='ble bug # TODO: BLEQABR25-2377')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            f'{str(CUR_DIR / "ble_conn_subrating_peripheral")}|{str(CUR_DIR / "ble_conn_subrating_central")}',
            'name',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32h2', 'esp32c6', 'esp32c5', 'esp32c61', 'esp32h4', 'esp32s31'], indirect=['target'])
def test_ble_conn_subrating_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_ble_conn_subrating_func(dut)


def _run_ble_periodic_adv_sync_with_cte_func(dut: tuple[IdfDut, IdfDut]) -> None:
    adv_dut = dut[0]
    sync_dut = dut[1]

    adv_dut.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30)
    sync_dut.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30)

    name = adv_dut.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    peer_name = sync_dut.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    assert peer_name == name, (
        'sync and adv must use the same CI device name from '
        'esp_bluedroid_get_example_name(); mismatch means sdkconfig.ci or CI ID is wrong'
    )

    adv_dut.expect_exact('ESP_GAP_BLE_EXT_ADV_SET_PARAMS_COMPLETE_EVT, status 0', timeout=30)
    adv_dut.expect_exact('ESP_GAP_BLE_EXT_ADV_SET_RAND_ADDR_COMPLETE_EVT, status 0', timeout=30)
    adv_dut.expect_exact('ESP_GAP_BLE_EXT_ADV_DATA_SET_COMPLETE_EVT, status 0', timeout=30)
    adv_dut.expect_exact('ESP_GAP_BLE_EXT_ADV_START_COMPLETE_EVT, status 0', timeout=30)
    adv_dut.expect_exact('ESP_GAP_BLE_PERIODIC_ADV_SET_PARAMS_COMPLETE_EVT, status 0', timeout=30)
    adv_dut.expect_exact('ESP_BLE_CTE_SET_CONNLESS_TRANS_PARAMS_CMPL_EVT, status 0', timeout=30)
    adv_dut.expect_exact('ESP_BLE_CTE_SET_CONNLESS_TRANS_ENABLE_CMPL_EVT, status 0', timeout=30)
    adv_dut.expect_exact('ESP_GAP_BLE_PERIODIC_ADV_DATA_SET_COMPLETE_EVT, status 0', timeout=30)
    adv_dut.expect_exact('ESP_GAP_BLE_PERIODIC_ADV_START_COMPLETE_EVT, status 0', timeout=30)

    sync_dut.expect_exact('Extended scanning params set, status 0', timeout=30)
    sync_dut.expect_exact('Extended scanning start, status 0', timeout=30)
    sync_dut.expect_exact(f'Create sync with the peer device {name}', timeout=60)
    sync_dut.expect_exact('Periodic advertising create sync, status 0', timeout=30)
    sync_dut.expect_exact('Periodic advertising sync establish, status 0', timeout=30)

    synced_adv_addr = (
        sync_dut.expect(r'address (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )
    assert synced_adv_addr == 'c0:de:52:00:00:02', (
        'periodic sync must establish with the advertiser static random address programmed in ble_periodic_adv_with_cte'
    )

    sync_dut.expect_exact('sync handle ', timeout=30)
    sync_dut.expect_exact('Periodic adv report, sync handle ', timeout=30)

    sync_dut.expect_exact('CTE IQ sampling enable, status 0', timeout=30)
    sync_dut.expect_exact('i_sample', timeout=30)
    sync_dut.expect_exact('q_sample', timeout=30)

    adv_output = adv_dut.expect(pexpect.TIMEOUT, timeout=10)
    sync_output = sync_dut.expect(pexpect.TIMEOUT, timeout=10)
    assert 'rst:' not in str(adv_output) and 'boot:' not in str(adv_output)
    assert 'rst:' not in str(sync_output) and 'boot:' not in str(sync_output)


# Case 9: ble periodic adv/sync with CTE test
# EXAMPLE_CI_ID=19
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runner')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            f'{str(CUR_DIR / "ble_periodic_adv_with_cte")}|{str(CUR_DIR / "ble_periodic_sync_with_cte")}',
            'name',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32h2', 'esp32c5', 'esp32c61', 'esp32s31', 'esp32h21', 'esp32h4'], indirect=['target'])
def test_ble_periodic_adv_sync_with_cte_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_ble_periodic_adv_sync_with_cte_func(dut)


def _run_ble_connection_with_cte_func(dut: tuple[IdfDut, IdfDut]) -> None:
    peripheral = dut[0]
    central = dut[1]

    peripheral.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30)
    central.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30)

    name = peripheral.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    peer_name = central.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    assert peer_name == name, (
        'central and peripheral must use the same CI device name from '
        'esp_bluedroid_get_example_name(); mismatch means sdkconfig.ci or CI ID is wrong'
    )

    peripheral.expect_exact('GATT server register, status 0', timeout=30)
    central.expect_exact('GATT client register, status 0', timeout=30)
    peripheral.expect_exact('Attribute table create successfully', timeout=30)
    peripheral.expect_exact('Local privacy config, status 0', timeout=30)
    peripheral.expect_exact('Extended advertising params set, status 0', timeout=30)
    peripheral.expect_exact('Extended advertising data set, status 0', timeout=30)
    peripheral.expect_exact('Extended advertising start, status 0', timeout=30)

    central.expect_exact('Extended scanning start successfully', timeout=60)
    central.expect_exact('Device found', timeout=60)
    central.expect_exact(name, timeout=30)
    central.expect_exact('Scanning stop successfully', timeout=30)

    central.expect_exact('Connected, cur_conn_hdl', timeout=30)
    peripheral.expect_exact('Connected, conn_id', timeout=30)
    central.expect_exact('Open successfully', timeout=30)

    central.expect_exact('Pairing successfully', timeout=30)
    peripheral.expect_exact('Pairing successfully', timeout=30)

    peripheral.expect_exact('Set CTE connection transmit params, conn_handle', timeout=30)
    peripheral.expect_exact('CTE set connection transmit params, status 0', timeout=30)
    peripheral.expect_exact('Setting CTE connection response enable', timeout=30)
    peripheral.expect_exact('CTE set connection response enable, status 0', timeout=30)

    central.expect_exact('Set CTE connection receive params after encryption', timeout=30)
    central.expect_exact('CTE set connection receive params, status 0', timeout=30)
    central.expect_exact('Enable CTE request', timeout=30)
    central.expect_exact('CTE set connection request enable, status 0', timeout=30)

    central.expect_exact('conn_handle ', timeout=30)
    central.expect_exact('i_sample', timeout=30)
    central.expect_exact('q_sample', timeout=30)

    central_output = central.expect(pexpect.TIMEOUT, timeout=10)
    peripheral_output = peripheral.expect(pexpect.TIMEOUT, timeout=10)
    assert 'rst:' not in str(central_output) and 'boot:' not in str(central_output)
    assert 'rst:' not in str(peripheral_output) and 'boot:' not in str(peripheral_output)
    assert 'Disconnected' not in str(central_output)
    assert 'Disconnected' not in str(peripheral_output)


# Case 10: ble connection peripheral/central with CTE test
# EXAMPLE_CI_ID=20
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runner')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            f'{str(CUR_DIR / "ble_connection_peripheral_with_cte")}|{str(CUR_DIR / "ble_connection_central_with_cte")}',
            'name',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32h2', 'esp32c5', 'esp32c61', 'esp32s31', 'esp32h21', 'esp32h4'], indirect=['target'])
def test_ble_connection_with_cte_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_ble_connection_with_cte_func(dut)


PAWR_TARGETS = ['esp32h2', 'esp32c6', 'esp32c5', 'esp32c61', 'esp32s31', 'esp32h21', 'esp32h4']


def _run_ble_pawr_adv_sync_common(adv: IdfDut, sync: IdfDut, adv_rand_addr_log: str) -> None:
    adv_name = adv.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    sync_name = sync.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    assert sync_name == adv_name, (
        'advertiser and synchronizer must use the same CI device name from '
        'esp_bluedroid_get_example_name(); mismatch means sdkconfig.ci or CI ID is wrong'
    )

    sync.expect_exact('GATTS register app_id', timeout=30)
    sync.expect_exact('create attribute table successfully', timeout=30)

    adv.expect_exact('Extended advertising params set, status 0, instance 0', timeout=30)
    adv.expect_exact(adv_rand_addr_log, timeout=30)
    adv.expect_exact('Extended advertising data set, status 0, instance 0', timeout=30)
    adv.expect_exact('Extended advertising start, status 0, instance num 1', timeout=30)
    adv.expect_exact('Periodic advertising params set, status 0, instance 0', timeout=30)
    adv.expect_exact('Periodic advertising start, status 0, instance 0', timeout=30)
    adv.expect_exact('PAwR advertising started successfully', timeout=30)

    sync.expect_exact('Extended scanning params set, status 0', timeout=30)
    sync.expect_exact('Extended scanning start, status 0', timeout=30)
    sync.expect_exact(f'Create sync with the peer device {adv_name}', timeout=60)
    sync.expect_exact('Periodic advertising sync establish, status 0', timeout=60)
    sync.expect_exact('sync handle ', timeout=30)
    sync.expect_exact('[Periodic Adv Report] handle:', timeout=30)
    sync.expect_exact('PA sync subevent, status 0', timeout=60)
    sync.expect_exact('[Periodic Adv Report] handle:', timeout=60)
    adv.expect_exact('[Response] adv_handle:', timeout=60)


def _run_ble_pawr_adv_sync_func(dut: tuple[IdfDut, IdfDut]) -> None:
    adv = dut[0]
    sync = dut[1]
    _run_ble_pawr_adv_sync_common(adv, sync, 'Extended advertising set random address, status 0')

    adv_output = adv.expect(pexpect.TIMEOUT, timeout=10)
    sync_output = sync.expect(pexpect.TIMEOUT, timeout=10)
    assert 'rst:' not in str(adv_output) and 'boot:' not in str(adv_output)
    assert 'rst:' not in str(sync_output) and 'boot:' not in str(sync_output)
    assert 'Periodic advertising sync lost' not in str(sync_output)


# Case 11: ble pawr advertiser and synchronizer test
# EXAMPLE_CI_ID=21
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runner')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            f'{str(CUR_DIR / "ble_pawr_advertiser")}|{str(CUR_DIR / "ble_pawr_synchronizer")}',
            'name',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', PAWR_TARGETS, indirect=['target'])
def test_ble_pawr_adv_sync_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_ble_pawr_adv_sync_func(dut)


def _run_ble_pawr_adv_conn_sync_func(dut: tuple[IdfDut, IdfDut]) -> None:
    adv = dut[0]
    sync = dut[1]
    _run_ble_pawr_adv_sync_common(adv, sync, 'Extended advertising set random address complete, status 0')

    adv.expect_exact('Connection create sent', timeout=60)
    adv.expect_exact('[Connection established]', timeout=30)
    sync.expect_exact('[Connection established]', timeout=30)
    adv.expect_exact('GATTC open successfully', timeout=30)
    adv.expect_exact('PAwR Sync Service found', timeout=30)
    adv.expect_exact('Notification register successfully', timeout=30)
    adv.expect_exact('Descriptor write successfully, notify enabled', timeout=30)

    adv_output = adv.expect(pexpect.TIMEOUT, timeout=10)
    sync_output = sync.expect(pexpect.TIMEOUT, timeout=10)
    assert 'rst:' not in str(adv_output) and 'boot:' not in str(adv_output)
    assert 'rst:' not in str(sync_output) and 'boot:' not in str(sync_output)
    assert 'Periodic advertising sync lost' not in str(sync_output)
    assert 'Disconnected' not in str(adv_output)
    assert 'Disconnected' not in str(sync_output)


# # Case 12: ble pawr advertiser_conn and synchronizer test
# # EXAMPLE_CI_ID=22
# @pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
# @pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runner')
# @pytest.mark.two_duts
# @pytest.mark.parametrize(
#     'count, app_path, config, erase_nvs',
#     [
#         (
#             2,
#             f'{str(CUR_DIR / "ble_pawr_advertiser_conn")}|{str(CUR_DIR / "ble_pawr_synchronizer")}',
#             'conn_name',
#             'y',
#         ),
#     ],
#     indirect=True,
# )
# @idf_parametrize('target', PAWR_TARGETS, indirect=['target'])
# def test_ble_pawr_adv_conn_sync_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
#     _run_ble_pawr_adv_conn_sync_func(dut)


def _run_ble_multi_adv_func(dut: IdfDut) -> None:
    dut.expect_exact('Extended advertising params set, status 0, instance 0', timeout=30)
    dut.expect_exact('Extended advertising random address set, status 0, instance 0', timeout=30)
    dut.expect_exact('Extended advertising data set, status 0, instance 0', timeout=30)
    dut.expect_exact('Extended advertising params set, status 0, instance 1', timeout=30)
    dut.expect_exact('Extended advertising random address set, status 0, instance 1', timeout=30)
    dut.expect_exact('Extended advertising scan response data set, status 0, instance 1', timeout=30)
    dut.expect_exact('Extended advertising params set, status 0, instance 2', timeout=30)
    dut.expect_exact('Extended advertising random address set, status 0, instance 2', timeout=30)
    dut.expect_exact('Extended advertising data set, status 0, instance 2', timeout=30)
    dut.expect_exact('Extended advertising scan response data set, status 0, instance 2', timeout=30)
    dut.expect_exact('Extended advertising params set, status 0, instance 3', timeout=30)
    dut.expect_exact('Extended advertising random address set, status 0, instance 3', timeout=30)
    dut.expect_exact('Extended advertising scan response data set, status 0, instance 3', timeout=30)
    dut.expect_exact('Extended advertising start, status 0, instance_num 4', timeout=30)

    output = dut.expect(pexpect.TIMEOUT, timeout=10)
    assert 'rst:' not in str(output) and 'boot:' not in str(output)


# Case 13: multi-adv test (single DUT)
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.generic
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (1, str(CUR_DIR / 'multi-adv'), 'name', 'y'),
    ],
    indirect=True,
)
@idf_parametrize(
    'target', ['esp32c3', 'esp32c6', 'esp32c5', 'esp32h2', 'esp32s3', 'esp32c61', 'esp32s31'], indirect=['target']
)
def test_ble_multi_adv_func(dut: IdfDut) -> None:
    _run_ble_multi_adv_func(dut)


@pytest.mark.generic
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, target, baud, app_path, config, erase_nvs',
    [
        (1, 'esp32c2', '74880', str(CUR_DIR / 'multi-adv'), 'esp32c2_xtal26m', 'y'),
    ],
    indirect=True,
)
def test_c2_26m_xtal_ble_multi_adv_func(dut: IdfDut) -> None:
    _run_ble_multi_adv_func(dut)
