# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
from pathlib import Path

import pexpect
import pytest
from pytest_embedded_idf.dut import IdfDut
from pytest_embedded_idf.utils import idf_parametrize

CUR_DIR = Path(__file__).parent.resolve()


# Case 1: BLE power save test
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
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
    'target',
    ['esp32c6', 'esp32h2', 'esp32c3', 'esp32s3', 'esp32c5', 'esp32c61', 'esp32', 'esp32h4'],
    indirect=['target'],
)
def test_power_save_conn(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    peripheral = dut[0]
    central = dut[1]

    peripheral.expect_exact('NimBLE_BLE_PRPH: BLE Host Task Started', timeout=30)
    central.expect_exact('NimBLE_BLE_CENT: BLE Host Task Started', timeout=30)
    peripheral.expect_exact('Returned from app_main()', timeout=30)
    central.expect_exact('Returned from app_main()', timeout=30)
    central.expect_exact('Connection established', timeout=30)
    peripheral.expect_exact('connection established; status=0', timeout=30)
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

    peripheral.expect_exact('NimBLE_BLE_PRPH: BLE Host Task Started', timeout=5)
    central.expect_exact('NimBLE_BLE_CENT: BLE Host Task Started', timeout=5)
    peripheral.expect_exact('Returned from app_main()', timeout=5)
    central.expect_exact('Returned from app_main()', timeout=5)
    central.expect_exact('Connection established', timeout=30)
    peripheral.expect_exact('connection established; status=0', timeout=30)
    output = peripheral.expect(pexpect.TIMEOUT, timeout=30)
    assert 'rst:' not in str(output) and 'boot:' not in str(output)


# Case 2: BLE power save test for ESP32C2ECO4
@pytest.mark.two_duts
@pytest.mark.xtal_26mhz
@pytest.mark.esp32c2_rev2
@pytest.mark.parametrize(
    'config, count, app_path, baud',
    [
        (
            'esp32c2_rev2_xtal26m',
            2,
            f'{str(CUR_DIR / "power_save")}|{str(CUR_DIR / "blecent")}',
            '74880',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c2'], indirect=['target'])
def test_power_save_conn_esp32c2_rev2(dut: tuple[IdfDut, IdfDut]) -> None:
    peripheral = dut[0]
    central = dut[1]

    peripheral.expect_exact('NimBLE_BLE_PRPH: BLE Host Task Started', timeout=5)
    central.expect_exact('NimBLE_BLE_CENT: BLE Host Task Started', timeout=5)
    peripheral.expect_exact('Returned from app_main()', timeout=5)
    central.expect_exact('Returned from app_main()', timeout=5)
    central.expect_exact('Connection established', timeout=30)
    peripheral.expect_exact('connection established; status=0', timeout=30)
    output = peripheral.expect(pexpect.TIMEOUT, timeout=30)
    assert 'rst:' not in str(output) and 'boot:' not in str(output)


# Case 2: BLE power save test for ESP32C3ECO7
@pytest.mark.two_duts
@pytest.mark.esp32c3_rev1
@pytest.mark.parametrize(
    'config, count, app_path',
    [
        (
            'esp32c3_rev1',
            2,
            f'{str(CUR_DIR / "power_save")}|{str(CUR_DIR / "blecent")}',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_power_save_conn_esp32c3_rev1(dut: tuple[IdfDut, IdfDut]) -> None:
    peripheral = dut[0]
    central = dut[1]

    peripheral.expect_exact('NimBLE_BLE_PRPH: BLE Host Task Started', timeout=5)
    central.expect_exact('NimBLE_BLE_CENT: BLE Host Task Started', timeout=5)
    peripheral.expect_exact('Returned from app_main()', timeout=5)
    central.expect_exact('Returned from app_main()', timeout=5)
    central.expect_exact('Connection established', timeout=30)
    peripheral.expect_exact('connection established; status=0', timeout=30)
    output = peripheral.expect(pexpect.TIMEOUT, timeout=30)
    assert 'rst:' not in str(output) and 'boot:' not in str(output)


CTE_PERIODIC_ADV_SYNC_APP_PATH = (
    f'{str(CUR_DIR / "ble_cte" / "ble_periodic_adv_with_cte")}|'
    f'{str(CUR_DIR / "ble_cte" / "ble_periodic_sync_with_cte")}'
)
NIMBLE_CTE_TARGETS = ['esp32h2', 'esp32c5', 'esp32c61', 'esp32s31', 'esp32h4']


def _run_nimble_periodic_adv_sync_with_cte_func(dut: tuple[IdfDut, IdfDut]) -> None:
    adv = dut[0]
    sync = dut[1]

    adv.expect_exact('DIRECTION_FINDING Example Periodic Adv AOA Mode', timeout=30)
    adv.expect_exact('BLE Host Task Started', timeout=30)
    sync.expect_exact('BLE Host Task Started', timeout=30)

    adv_addr = adv.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    name = adv.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    adv.expect_exact('Instance 0 started (periodic)', timeout=30)

    sync.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30)
    peer_name = sync.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    assert peer_name == name, (
        'advertiser and synchronizer must use the same CI device name from '
        'esp_ble_cte_get_example_name(); mismatch means sdkconfig.ci or CI ID is wrong'
    )
    sync.expect_exact('Receive CTE Antenna Pattern Info:', timeout=30)

    sync.expect_exact('Started periodic sync with device', timeout=60)
    sync.expect_exact('Periodic Sync Established', timeout=60)

    found_addr = (
        sync.expect(
            r'Adv Addr: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})',
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert found_addr.lower() == adv_addr.lower(), (
        'periodic sync must lock to the advertiser board MAC, not another periodic advertiser'
    )

    sync.expect_exact('IQ Report | Sync Handle:', timeout=60)

    adv_output = adv.expect(pexpect.TIMEOUT, timeout=10)
    sync_output = sync.expect(pexpect.TIMEOUT, timeout=10)
    assert 'rst:' not in str(adv_output) and 'boot:' not in str(adv_output)
    assert 'rst:' not in str(sync_output) and 'boot:' not in str(sync_output)
    assert 'Sync Lost' not in str(sync_output)


# Case 4: nimble periodic adv/sync with CTE test
# EXAMPLE_CI_ID=25
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            CTE_PERIODIC_ADV_SYNC_APP_PATH,
            'name',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', NIMBLE_CTE_TARGETS, indirect=['target'])
def test_nimble_periodic_adv_sync_with_cte_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_periodic_adv_sync_with_cte_func(dut)


CTS_APP_PATH = f'{str(CUR_DIR / "ble_cts" / "cts_prph")}|{str(CUR_DIR / "ble_cts" / "cts_cent")}'
NIMBLE_CTS_TARGETS = [
    'esp32',
    'esp32c3',
    'esp32c6',
    'esp32c5',
    'esp32h2',
    'esp32s3',
    'esp32c61',
    'esp32h4',
    'esp32s31',
]


def _run_nimble_cts_func(dut: tuple[IdfDut, IdfDut]) -> None:
    peripheral = dut[0]
    central = dut[1]

    peripheral_addr = (
        peripheral.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )
    central_addr = (
        central.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )
    name = peripheral.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    peer_name = central.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    assert peer_name == name, (
        'central and peripheral must use the same CI device name from '
        'esp_ble_cts_get_example_name(); mismatch means sdkconfig.ci or CI ID is wrong'
    )

    peripheral.expect_exact('NimBLE_CTS_PRPH: BLE Host Task Started', timeout=30)
    central.expect_exact('NimBLE_CTS_CENT: BLE Host Task Started', timeout=30)

    found_addr = (
        central.expect(
            rf'Found device: (([0-9a-fA-F]{{2}}:){{5}}[0-9a-fA-F]{{2}}), name: {name}',
            timeout=60,
        )
        .group(1)
        .decode('utf8')
    )
    assert found_addr.lower() == peripheral_addr.lower(), (
        'scan must report the peripheral board MAC, not another advertiser with the same CI name'
    )

    connected_remote = (
        central.expect(
            r'Connected, conn_handle \d+, remote (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})',
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert connected_remote.lower() == peripheral_addr.lower(), (
        'central must connect to the peripheral board MAC, not a colliding device in RF environment'
    )
    peripheral_remote = (
        peripheral.expect(
            r'Connected, conn_handle \d+, remote (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})',
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert peripheral_remote.lower() == central_addr.lower(), 'peripheral must see central board MAC as connection peer'

    central.expect_exact('Service discovery complete; status=0', timeout=60)
    central.expect_exact('Read Current time complete; status=0', timeout=30)
    central.expect_exact('Date :', timeout=30)

    out_central = central.expect(pexpect.TIMEOUT, timeout=10)
    out_peripheral = peripheral.expect(pexpect.TIMEOUT, timeout=10)
    assert 'rst:' not in str(out_central) and 'boot:' not in str(out_central)
    assert 'rst:' not in str(out_peripheral) and 'boot:' not in str(out_peripheral)
    assert 'disconnect' not in str(out_central).lower()
    assert 'disconnect' not in str(out_peripheral).lower()


# Case 5: nimble CTS peripheral/central test
# EXAMPLE_CI_ID=26
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            CTS_APP_PATH,
            'name',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', NIMBLE_CTS_TARGETS, indirect=['target'])
def test_nimble_cts_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_cts_func(dut)


# Case 6: nimble CTS test for ESP32C2 26MHz
# EXAMPLE_CI_ID=26
@pytest.mark.two_duts
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, target, baud, app_path, config, erase_nvs',
    [
        (
            2,
            'esp32c2|esp32c2',
            '74880',
            CTS_APP_PATH,
            'esp32c2_xtal26m',
            'y',
        ),
    ],
    indirect=True,
)
def test_nimble_cts_func_esp32c2_26mhz(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_cts_func(dut)


HTP_APP_PATH = f'{str(CUR_DIR / "ble_htp" / "htp_prph")}|{str(CUR_DIR / "ble_htp" / "htp_cent")}'
NIMBLE_HTP_TARGETS = [
    'esp32',
    'esp32c3',
    'esp32c6',
    'esp32c5',
    'esp32h2',
    'esp32s3',
    'esp32c61',
    'esp32h4',
    'esp32s31',
]


def _run_nimble_htp_func(dut: tuple[IdfDut, IdfDut]) -> None:
    peripheral = dut[0]
    central = dut[1]

    peripheral_addr = (
        peripheral.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )
    central_addr = (
        central.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )
    name = peripheral.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    peer_name = central.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    assert peer_name == name, (
        'central and peripheral must use the same CI device name from '
        'esp_ble_htp_get_example_name(); mismatch means sdkconfig.ci or CI ID is wrong'
    )

    peripheral.expect_exact('NimBLE_HTP_PRPH: BLE Host Task Started', timeout=30)
    central.expect_exact('NimBLE_HTP_CENT: BLE Host Task Started', timeout=30)

    found_addr = (
        central.expect(
            rf'Found device: (([0-9a-fA-F]{{2}}:){{5}}[0-9a-fA-F]{{2}}), name: {name}',
            timeout=60,
        )
        .group(1)
        .decode('utf8')
    )
    assert found_addr.lower() == peripheral_addr.lower(), (
        'scan must report the peripheral board MAC, not another advertiser with the same CI name'
    )

    connected_remote = (
        central.expect(
            r'Connected, conn_handle \d+, remote (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})',
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert connected_remote.lower() == peripheral_addr.lower(), (
        'central must connect to the peripheral board MAC, not a colliding device in RF environment'
    )
    peripheral_remote = (
        peripheral.expect(
            r'Connected, conn_handle \d+, remote (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})',
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert peripheral_remote.lower() == central_addr.lower(), 'peripheral must see central board MAC as connection peer'

    central.expect_exact('Service discovery complete; status=0', timeout=60)
    central.expect_exact('Read temperature type char completed; status=0', timeout=30)
    central.expect_exact('Write to measurement interval char completed; status=0', timeout=30)
    central.expect_exact('Subscribe to temperature measurement char completed; status=0', timeout=30)
    central.expect_exact('Subscribe to intermediate temperature char completed; status=0', timeout=30)

    out_central = central.expect(pexpect.TIMEOUT, timeout=10)
    out_peripheral = peripheral.expect(pexpect.TIMEOUT, timeout=10)
    assert 'rst:' not in str(out_central) and 'boot:' not in str(out_central)
    assert 'rst:' not in str(out_peripheral) and 'boot:' not in str(out_peripheral)
    assert 'disconnect' not in str(out_central).lower()
    assert 'disconnect' not in str(out_peripheral).lower()


# Case 7: nimble HTP peripheral/central test
# EXAMPLE_CI_ID=27
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            HTP_APP_PATH,
            'name',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', NIMBLE_HTP_TARGETS, indirect=['target'])
def test_nimble_htp_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_htp_func(dut)


# Case 8: nimble HTP test for ESP32C2 26MHz
# EXAMPLE_CI_ID=27
@pytest.mark.two_duts
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, target, baud, app_path, config, erase_nvs',
    [
        (
            2,
            'esp32c2|esp32c2',
            '74880',
            HTP_APP_PATH,
            'esp32c2_xtal26m',
            'y',
        ),
    ],
    indirect=True,
)
def test_nimble_htp_func_esp32c2_26mhz(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_htp_func(dut)


SPP_APP_PATH = f'{str(CUR_DIR / "ble_spp" / "spp_server")}|{str(CUR_DIR / "ble_spp" / "spp_client")}'
NIMBLE_SPP_TARGETS = [
    'esp32',
    'esp32c3',
    'esp32c6',
    'esp32c5',
    'esp32h2',
    'esp32s3',
    'esp32c61',
    'esp32h4',
    'esp32s31',
]


def _run_nimble_spp_func(dut: tuple[IdfDut, IdfDut]) -> None:
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
        'client and server must use the same CI device name from '
        'esp_ble_spp_get_example_name(); mismatch means sdkconfig.ci or CI ID is wrong'
    )

    server.expect_exact('NimBLE_SPP_SERVER: BLE Host Task Started', timeout=30)
    client.expect_exact('NimBLE_SPP_CLIENT: BLE Host Task Started', timeout=30)

    found_addr = (
        client.expect(
            rf'Found device: (([0-9a-fA-F]{{2}}:){{5}}[0-9a-fA-F]{{2}}), name: {name}',
            timeout=60,
        )
        .group(1)
        .decode('utf8')
    )
    assert found_addr.lower() == server_addr.lower(), (
        'scan must report the server board MAC, not another advertiser with the same CI name'
    )

    connected_remote = (
        client.expect(
            r'Connected, conn_handle \d+, remote (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})',
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert connected_remote.lower() == server_addr.lower(), (
        'client must connect to the server board MAC, not a colliding device in RF environment'
    )
    server_remote = (
        server.expect(
            r'Connected, conn_handle \d+, remote (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})',
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert server_remote.lower() == client_addr.lower(), 'server must see client board MAC as connection peer'

    client.expect_exact('Service discovery complete; status=0', timeout=60)
    client.expect(r'attribute_handle [0-9a-fA-F]+', timeout=30)
    server.expect_exact('subscribe event; conn_handle=', timeout=30)

    out_client = client.expect(pexpect.TIMEOUT, timeout=10)
    out_server = server.expect(pexpect.TIMEOUT, timeout=10)
    assert 'rst:' not in str(out_client) and 'boot:' not in str(out_client)
    assert 'rst:' not in str(out_server) and 'boot:' not in str(out_server)
    assert 'disconnect' not in str(out_client).lower()
    assert 'disconnect' not in str(out_server).lower()


# Case 9: nimble SPP server/client test
# EXAMPLE_CI_ID=28
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            SPP_APP_PATH,
            'name',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', NIMBLE_SPP_TARGETS, indirect=['target'])
def test_nimble_spp_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_spp_func(dut)


# Case 10: nimble SPP test for ESP32C2 26MHz
# EXAMPLE_CI_ID=28
@pytest.mark.two_duts
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, target, baud, app_path, config, erase_nvs',
    [
        (
            2,
            'esp32c2|esp32c2',
            '74880',
            SPP_APP_PATH,
            'esp32c2_xtal26m',
            'y',
        ),
    ],
    indirect=True,
)
def test_nimble_spp_func_esp32c2_26mhz(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_spp_func(dut)


MAC_PATTERN = r'(([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})'
MULTI_CONN_APP_PATH = (
    f'{str(CUR_DIR / "ble_multi_conn" / "ble_multi_conn_prph")}|'
    f'{str(CUR_DIR / "ble_multi_conn" / "ble_multi_conn_cent")}'
)
NIMBLE_MULTI_CONN_TARGETS = ['esp32c5', 'esp32c6', 'esp32c61', 'esp32h2', 'esp32h21', 'esp32s31']


def _run_nimble_multi_conn_func(dut: tuple[IdfDut, IdfDut]) -> None:
    prph_dut, cent_dut = dut[0], dut[1]

    prph_dut.expect(r'Bluetooth MAC: ' + MAC_PATTERN, timeout=30)
    cent_dut.expect(r'Bluetooth MAC: ' + MAC_PATTERN, timeout=30)
    name = prph_dut.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    peer_name = cent_dut.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    assert peer_name == name, (
        'central and peripheral must use the same CI device name from '
        'esp_ble_multi_conn_get_example_name(); mismatch means sdkconfig.ci or CI ID is wrong'
    )

    max_cfg = int(
        prph_dut.expect(
            r'ESP_MULTI_CONN_PRPH: CONFIG_BT_NIMBLE_MAX_CONNECTIONS=(\d+)\.',
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert max_cfg >= 1, 'peripheral MAX_CONNECTIONS must be at least 1 for multi-conn test'

    prph_dut.expect_exact('ESP_MULTI_CONN_PRPH: BLE Host Task Started', timeout=30)
    cent_dut.expect_exact('ESP_MULTI_CONN_CENT: BLE Host Task Started', timeout=30)

    for i in range(1, max_cfg + 1):
        prph_addr = (
            prph_dut.expect(
                r'ESP_MULTI_CONN_PRPH: Started adv, Device Address ' + MAC_PATTERN + r'.',
                timeout=30,
            )
            .group(1)
            .decode('utf8')
        )

        found = (
            cent_dut.expect(
                r'ESP_MULTI_CONN_CENT: Found device: addr: ' + MAC_PATTERN + rf', name: {name}',
                timeout=90,
            )
            .group(1)
            .decode('utf8')
        )
        assert found.lower() == prph_addr.lower(), (
            f'Round {i}: scan must report the peripheral adv address, '
            f'cent found {found.lower()} != prph adv {prph_addr.lower()}'
        )

        create = cent_dut.expect(
            r'ESP_MULTI_CONN_CENT: Create connection\. -> peer addr ' + MAC_PATTERN,
            timeout=30,
        )
        assert create.group(1).decode('utf8').lower() == prph_addr.lower(), (
            f'Round {i}: central must connect to the peripheral adv address'
        )
        prph_dut.expect_exact(
            'ESP_MULTI_CONN_PRPH: advertisement completed. Reason=0.',
            timeout=30,
        )
        cent_dut.expect(
            rf'ESP_MULTI_CONN_CENT: Connection established\. Handle:\d+, Total:{i}\b',
            timeout=120,
        )
        prph_dut.expect(
            rf'ESP_MULTI_CONN_PRPH: Connection established\. Handle:\d+\. Total:{i}\b',
            timeout=60,
        )

    prph_output = prph_dut.expect(pexpect.TIMEOUT, timeout=5)
    cent_output = cent_dut.expect(pexpect.TIMEOUT, timeout=5)
    assert 'rst:' not in str(prph_output) and 'boot:' not in str(prph_output)
    assert 'rst:' not in str(cent_output) and 'boot:' not in str(cent_output)
    assert 'disconnect' not in str(prph_output).lower()
    assert 'disconnect' not in str(cent_output).lower()


# Case 11: NimBLE multi-conn peripheral and central
# EXAMPLE_CI_ID=29
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runner')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            MULTI_CONN_APP_PATH,
            'name',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', NIMBLE_MULTI_CONN_TARGETS, indirect=['target'])
def test_nimble_multi_conn_prph_cent(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_multi_conn_func(dut)


PHY_APP_PATH = f'{str(CUR_DIR / "ble_phy" / "phy_prph")}|{str(CUR_DIR / "ble_phy" / "phy_cent")}'
NIMBLE_PHY_TARGETS = [
    'esp32c3',
    'esp32c6',
    'esp32c5',
    'esp32h2',
    'esp32s3',
    'esp32c61',
    'esp32h4',
    'esp32s31',
]


def _run_nimble_phy_prph_cent_func(dut: tuple[IdfDut, IdfDut]) -> None:
    prph_dut, cent_dut = dut[0], dut[1]

    def _cent_gatt_read_write_terminate() -> None:
        cent_dut.expect_exact('NimBLE: GATT procedure initiated: discover all services', timeout=30)
        cent_dut.expect_exact('NimBLE: Service discovery complete; status=0', timeout=60)
        cent_dut.expect_exact('NimBLE: Read complete; status=', timeout=30)
        cent_dut.expect_exact('NimBLE: Write complete; status=', timeout=30)
        cent_dut.expect_exact('NimBLE: GAP procedure initiated: terminate connection', timeout=30)

    def _prph_adv_then_conn() -> None:
        prph_dut.expect_exact('NimBLE: advertise complete; reason=0', timeout=30)
        prph_dut.expect_exact('NimBLE: connection established; status=0', timeout=30)

    def _disconnect_then_prph_re_adv() -> None:
        prph_dut.expect_exact('NimBLE: disconnect;', timeout=30)
        cent_dut.expect_exact('NimBLE: disconnect;', timeout=30)
        prph_dut.expect_exact('NimBLE: GAP procedure initiated: extended advertise', timeout=30)

    prph_bt_mac = prph_dut.expect(r'Bluetooth MAC: ' + MAC_PATTERN, timeout=30).group(1).decode('utf8')
    cent_bt_mac = cent_dut.expect(r'Bluetooth MAC: ' + MAC_PATTERN, timeout=30).group(1).decode('utf8')
    assert prph_bt_mac.lower() != cent_bt_mac.lower(), (
        'peripheral and central boards must have different Bluetooth MACs'
    )

    name = prph_dut.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    peer_name = cent_dut.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    assert peer_name == name, (
        'central and peripheral must use the same CI device name from '
        'esp_ble_phy_get_example_name(); mismatch means sdkconfig.ci or CI ID is wrong'
    )

    prph_dut.expect_exact('NimBLE_BLE_PHY_PRPH: BLE Host Task Started', timeout=30)
    cent_dut.expect_exact('NimBLE_BLE_PHY_CENT: BLE Host Task Started', timeout=30)
    prph_dut.expect_exact('NimBLE: Default LE PHY set successfully', timeout=30)
    prph_dut.expect_exact('NimBLE: GAP procedure initiated: extended advertise', timeout=30)
    cent_dut.expect_exact('NimBLE: Default LE PHY set successfully', timeout=30)
    cent_dut.expect_exact('NimBLE: GAP procedure initiated: extended discovery', timeout=30)

    found = cent_dut.expect(
        r'NimBLE_BLE_PHY_CENT: Found device: addr: ' + MAC_PATTERN + rf', name: {name}',
        timeout=90,
    )
    found_addr = found.group(1).decode('utf8')
    assert found_addr.lower() == prph_bt_mac.lower(), (
        'scan must report the peripheral board identity address, not another advertiser'
    )

    # 1M
    cent_dut.expect_exact('NimBLE: Attempting to connect to :', timeout=30)
    cent_dut.expect_exact('NimBLE: GAP procedure initiated: extended connect', timeout=30)
    _prph_adv_then_conn()
    cent_dut.expect_exact('Connection established on 1M Phy', timeout=90)
    _cent_gatt_read_write_terminate()

    # 2M
    _disconnect_then_prph_re_adv()
    cent_dut.expect_exact('Attempting to initiate connection on 2M PHY', timeout=60)
    cent_dut.expect_exact('NimBLE: GAP procedure initiated: extended connect', timeout=30)
    _prph_adv_then_conn()
    cent_dut.expect_exact('Connection established on 2M Phy', timeout=90)
    _cent_gatt_read_write_terminate()

    # Coded
    _disconnect_then_prph_re_adv()
    cent_dut.expect_exact('Attempting to initiate connection on Coded PHY', timeout=60)
    cent_dut.expect_exact('NimBLE: GAP procedure initiated: extended connect', timeout=30)
    _prph_adv_then_conn()
    cent_dut.expect_exact('Connection established on Coded Phy', timeout=90)
    _cent_gatt_read_write_terminate()

    prph_dut.expect_exact('NimBLE: disconnect;', timeout=30)
    cent_dut.expect_exact('NimBLE: disconnect;', timeout=30)

    prph_output = prph_dut.expect(pexpect.TIMEOUT, timeout=5)
    cent_output = cent_dut.expect(pexpect.TIMEOUT, timeout=5)
    assert 'rst:' not in str(prph_output) and 'boot:' not in str(prph_output)
    assert 'rst:' not in str(cent_output) and 'boot:' not in str(cent_output)


# Case 12: NimBLE PHY peripheral/central test
# EXAMPLE_CI_ID=30
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            PHY_APP_PATH,
            'name',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', NIMBLE_PHY_TARGETS, indirect=['target'])
def test_nimble_phy_prph_cent(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_phy_prph_cent_func(dut)


# Case 13: NimBLE PHY test for ESP32C2 26MHz
# EXAMPLE_CI_ID=30
@pytest.mark.two_duts
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, target, baud, app_path, config, erase_nvs',
    [
        (
            2,
            'esp32c2|esp32c2',
            '74880',
            PHY_APP_PATH,
            'esp32c2_xtal26m',
            'y',
        ),
    ],
    indirect=True,
)
def test_esp32c2_26mhz_nimble_phy_prph_cent(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_phy_prph_cent_func(dut)


PAWR_CONN_APP_PATH = (
    f'{str(CUR_DIR / "ble_pawr_adv_conn" / "ble_pawr_adv_conn")}'
    f'|{str(CUR_DIR / "ble_pawr_adv_conn" / "ble_pawr_sync_conn")}'
)
NIMBLE_PAWR_CONN_TARGETS = [
    'esp32h2',
    'esp32c6',
    'esp32c5',
    'esp32c61',
    'esp32s31',
    'esp32h21',
    'esp32h4',
]


def _run_nimble_pawr_adv_conn_sync_func(dut: tuple[IdfDut, IdfDut]) -> None:
    adv_dut, sync_dut = dut[0], dut[1]

    adv_bt_mac = adv_dut.expect(r'Bluetooth MAC: ' + MAC_PATTERN, timeout=30).group(1).decode('utf8')
    sync_bt_mac = sync_dut.expect(r'Bluetooth MAC: ' + MAC_PATTERN, timeout=30).group(1).decode('utf8')
    assert adv_bt_mac.lower() != sync_bt_mac.lower(), (
        'advertiser and synchronizer boards must have different Bluetooth MACs'
    )

    name = adv_dut.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    peer_name = sync_dut.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    assert peer_name == name, (
        'advertiser and synchronizer must use the same CI device name from '
        'esp_ble_pawr_conn_get_example_name(); mismatch means sdkconfig.ci or CI ID is wrong'
    )

    adv_dut.expect_exact('NimBLE_BLE_PAwR_CONN: BLE Host Task Started', timeout=30)
    sync_dut.expect_exact('NimBLE_BLE_PAwR_CONN: BLE Host Task Started', timeout=30)
    adv_dut.expect_exact('NimBLE_BLE_PAwR_CONN: instance 0 started (periodic)', timeout=30)

    found = sync_dut.expect(
        r'NimBLE_BLE_PAwR_CONN: Found device: addr: ' + MAC_PATTERN + rf', name: {name}',
        timeout=90,
    )
    found_addr = found.group(1).decode('utf8')
    assert found_addr.lower() == adv_bt_mac.lower(), (
        'scan must report the advertiser board identity address, not another advertiser'
    )

    sync_dut.expect_exact('NimBLE_BLE_PAwR_CONN: Create sync', timeout=60)
    sync_dut.expect_exact('NimBLE_BLE_PAwR_CONN: [Periodic Sync Established] sync handle:', timeout=60)
    sync_dut.expect_exact('NimBLE_BLE_PAwR_CONN: [Subevent Sync OK] sync handle:', timeout=30)

    sync_dut.expect_exact('NimBLE_BLE_PAwR_CONN: [Periodic Adv Report] handle:', timeout=60)
    sync_dut.expect_exact('NimBLE_BLE_PAwR_CONN: [RSP Data Set] sync handle:', timeout=60)

    adv_dut.expect_exact('NimBLE_BLE_PAwR_CONN: [Response] subevent:', timeout=90)
    adv_dut.expect_exact('NimBLE_BLE_PAwR_CONN: Connection create sent, adv handle =', timeout=60)

    adv_dut.expect_exact('NimBLE_BLE_PAwR_CONN: [Connection established], conn_handle =', timeout=60)
    sync_dut.expect_exact('NimBLE_BLE_PAwR_CONN: Connection established, conn_handle =', timeout=60)

    adv_peer = adv_dut.expect(r'peer_id_addr_type=\d+ peer_id_addr=' + MAC_PATTERN, timeout=30).group(1).decode('utf8')
    assert adv_peer.lower() == sync_bt_mac.lower(), 'advertiser must see synchronizer board MAC as connection peer'
    sync_peer = (
        sync_dut.expect(r'peer_id_addr_type=\d+ peer_id_addr=' + MAC_PATTERN, timeout=30).group(1).decode('utf8')
    )
    assert sync_peer.lower() == adv_bt_mac.lower(), 'synchronizer must see advertiser board MAC as connection peer'

    adv_output = adv_dut.expect(pexpect.TIMEOUT, timeout=10)
    sync_output = sync_dut.expect(pexpect.TIMEOUT, timeout=10)
    assert 'rst:' not in str(adv_output) and 'boot:' not in str(adv_output)
    assert 'rst:' not in str(sync_output) and 'boot:' not in str(sync_output)
    assert 'Periodic Sync Lost' not in str(sync_output)
    assert 'Disconnected' not in str(adv_output)
    assert 'Disconnected' not in str(sync_output)


# Case 14: NimBLE PAwR advertiser_conn / synchronizer_conn test
# EXAMPLE_CI_ID=31
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runner')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            PAWR_CONN_APP_PATH,
            'name',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', NIMBLE_PAWR_CONN_TARGETS, indirect=['target'])
def test_nimble_pawr_adv_conn_sync_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_pawr_adv_conn_sync_func(dut)


PAWR_ADV_APP_PATH = (
    f'{str(CUR_DIR / "ble_pawr_adv" / "ble_pawr_adv")}|{str(CUR_DIR / "ble_pawr_adv" / "ble_pawr_sync")}'
)
NIMBLE_PAWR_ADV_TARGETS = [
    'esp32h2',
    'esp32c6',
    'esp32c5',
    'esp32c61',
    'esp32s31',
    'esp32h21',
    'esp32h4',
]


def _run_nimble_pawr_adv_sync_func(dut: tuple[IdfDut, IdfDut]) -> None:
    adv_dut, sync_dut = dut[0], dut[1]

    adv_bt_mac = adv_dut.expect(r'Bluetooth MAC: ' + MAC_PATTERN, timeout=30).group(1).decode('utf8')
    sync_bt_mac = sync_dut.expect(r'Bluetooth MAC: ' + MAC_PATTERN, timeout=30).group(1).decode('utf8')
    assert adv_bt_mac.lower() != sync_bt_mac.lower(), (
        'advertiser and synchronizer boards must have different Bluetooth MACs'
    )

    name = adv_dut.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    peer_name = sync_dut.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    assert peer_name == name, (
        'advertiser and synchronizer must use the same CI device name from '
        'esp_ble_pawr_get_example_name(); mismatch means sdkconfig.ci or CI ID is wrong'
    )

    adv_dut.expect_exact('NimBLE_BLE_PAwR: BLE Host Task Started', timeout=30)
    sync_dut.expect_exact('NimBLE_BLE_PAwR: BLE Host Task Started', timeout=30)
    adv_dut.expect_exact('NimBLE_BLE_PAwR: instance 0 started (periodic)', timeout=30)
    adv_dut.expect_exact('NimBLE_BLE_PAwR: [Request] data:', timeout=60)

    found = sync_dut.expect(
        r'NimBLE_BLE_PAwR: Found device: addr: ' + MAC_PATTERN + rf', name: {name}',
        timeout=90,
    )
    found_addr = found.group(1).decode('utf8')
    assert found_addr.lower() == adv_bt_mac.lower(), (
        'scan must report the advertiser board identity address, not another advertiser'
    )

    sync_dut.expect_exact('NimBLE_BLE_PAwR: Create sync', timeout=60)
    sync_dut.expect_exact('NimBLE_BLE_PAwR: [Periodic Sync Established] sync handle:', timeout=60)
    sync_dut.expect_exact('NimBLE_BLE_PAwR: [Subevent Sync OK] sync handle:', timeout=30)

    sync_dut.expect_exact('NimBLE_BLE_PAwR: [Periodic Adv Report] handle:', timeout=60)
    sync_dut.expect_exact('NimBLE_BLE_PAwR: [RSP Data Set] sync handle:', timeout=60)

    adv_dut.expect_exact('NimBLE_BLE_PAwR: [Response] subevent:', timeout=90)
    adv_dut.expect_exact('NimBLE_BLE_PAwR: data:', timeout=30)

    adv_output = adv_dut.expect(pexpect.TIMEOUT, timeout=10)
    sync_output = sync_dut.expect(pexpect.TIMEOUT, timeout=10)
    assert 'rst:' not in str(adv_output) and 'boot:' not in str(adv_output)
    assert 'rst:' not in str(sync_output) and 'boot:' not in str(sync_output)
    assert 'Periodic Sync Lost' not in str(sync_output)


# Case 15: NimBLE PAwR advertiser / synchronizer test
# EXAMPLE_CI_ID=32
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runner')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            PAWR_ADV_APP_PATH,
            'name',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', NIMBLE_PAWR_ADV_TARGETS, indirect=['target'])
def test_nimble_pawr_adv_sync_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_pawr_adv_sync_func(dut)


# Case 16: NimBLE proximity sensor peripheral / central
# EXAMPLE_CI_ID: name/esp32c2_xtal26m=33, conn_with_scan/esp32c2_xtal26m_conn_with_scan=34,
#   name_v4/esp32c2_xtal26m_name_v4=35, conn_with_scan_v4/esp32c2_xtal26m_conn_with_scan_v4=36,
#   esp32_name_v4=37
BLE_PROXIMITY_SENSOR_V5_CONFIGS = ['name', 'conn_with_scan']
BLE_PROXIMITY_SENSOR_APP_PATH = (
    f'{str(CUR_DIR / "ble_proximity_sensor" / "proximity_sensor_prph")}'
    f'|{str(CUR_DIR / "ble_proximity_sensor" / "proximity_sensor_cent")}'
)


def _run_nimble_proximity_sensor_prph_cent(dut: tuple[IdfDut, IdfDut]) -> None:
    prph_dut, cent_dut = dut[0], dut[1]

    prph_bt_mac = prph_dut.expect(r'Bluetooth MAC: ' + MAC_PATTERN, timeout=30).group(1).decode('utf8')
    cent_dut.expect(r'Bluetooth MAC: ' + MAC_PATTERN, timeout=30)

    name = prph_dut.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    peer_name = cent_dut.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    assert peer_name == name, (
        'central and peripheral must use the same CI device name from get_example_name(); '
        'mismatch means sdkconfig.ci or CI ID is wrong'
    )

    prph_dut.expect_exact('NimBLE_PROX_PRPH: BLE Host Task Started', timeout=30)
    cent_dut.expect_exact('NimBLE_PROX_CENT: BLE Host Task Started', timeout=30)

    found = cent_dut.expect(
        r'NimBLE_PROX_CENT: Found device: addr: ' + MAC_PATTERN + rf', name: {name}',
        timeout=90,
    )
    found_addr = found.group(1).decode('utf8')
    assert found_addr.lower() == prph_bt_mac.lower(), (
        'scan must report the peripheral board MAC, not another advertiser with the same CI name'
    )

    cent_dut.expect_exact('Connection established', timeout=90)
    prph_dut.expect_exact('connection established; status=0', timeout=30)

    cent_dut.expect_exact('Connection secured', timeout=45)
    cent_dut.expect_exact('encryption change event; status=', timeout=30)
    cent_dut.expect_exact('Service discovery complete; status=0', timeout=120)
    cent_dut.expect_exact('Write alert level char completed; status=', timeout=30)
    cent_dut.expect_exact('Read on tx power level char completed; status=0', timeout=30)
    cent_dut.expect_exact('Connection handle :', timeout=30)
    cent_dut.expect_exact('Current RSSI =', timeout=30)
    cent_dut.expect_exact('path loss =', timeout=30)

    prph_output = prph_dut.expect(pexpect.TIMEOUT, timeout=5)
    cent_output = cent_dut.expect(pexpect.TIMEOUT, timeout=5)
    assert 'rst:' not in str(prph_output) and 'boot:' not in str(prph_output)
    assert 'rst:' not in str(cent_output) and 'boot:' not in str(cent_output)
    assert 'disconnect' not in str(cent_output).lower()
    assert 'disconnect' not in str(prph_output).lower()


@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runner')
@pytest.mark.temp_skip_ci(targets=['esp32s31'], reason='ble bugs # TODO: BLEQABR25-2508')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [(2, BLE_PROXIMITY_SENSOR_APP_PATH, config, 'y') for config in BLE_PROXIMITY_SENSOR_V5_CONFIGS],
    indirect=True,
)
@idf_parametrize(
    'target',
    ['esp32c6', 'esp32h2', 'esp32c5', 'esp32c61', 'esp32c3', 'esp32s3', 'esp32h21', 'esp32s31'],
    indirect=['target'],
)
def test_nimble_proximity_sensor_prph_cent(dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_proximity_sensor_prph_cent(dut)


BLE_PROXIMITY_SENSOR_V4_CONFIGS = ['name_v4', 'conn_with_scan_v4']


# Case 17: NimBLE proximity sensor peripheral / central for V4
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runner')
@pytest.mark.temp_skip_ci(targets=['esp32s31'], reason='ble bugs # TODO: BLEQABR25-2508')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [(2, BLE_PROXIMITY_SENSOR_APP_PATH, config, 'y') for config in BLE_PROXIMITY_SENSOR_V4_CONFIGS],
    indirect=True,
)
@idf_parametrize(
    'target',
    ['esp32c6', 'esp32h2', 'esp32c5', 'esp32c61', 'esp32c3', 'esp32s3', 'esp32h21', 'esp32s31'],
    indirect=['target'],
)
def test_v4_nimble_proximity_sensor_prph_cent(dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_proximity_sensor_prph_cent(dut)


# Case 18: NimBLE proximity sensor peripheral / central for ESP32
BLE_PROXIMITY_SENSOR_32_CONFIGS = ['esp32_name_v4']


@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [(2, BLE_PROXIMITY_SENSOR_APP_PATH, config, 'y') for config in BLE_PROXIMITY_SENSOR_32_CONFIGS],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_esp32_nimble_proximity_sensor_prph_cent(dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_proximity_sensor_prph_cent(dut)


# Case 19: NimBLE proximity sensor for ESP32-C2 26MHz
# Must use configs with CONFIG_XTAL_FREQ_26=y; plain name/name_v4 builds assume 40MHz XTAL
# and BLE RF will not work on 26MHz boards (UART may still work at baud 74880).
BLE_PROXIMITY_SENSOR_C2_CONFIGS = [
    'esp32c2_xtal26m',
    'esp32c2_xtal26m_conn_with_scan',
    'esp32c2_xtal26m_name_v4',
    'esp32c2_xtal26m_conn_with_scan_v4',
]


@pytest.mark.two_duts
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'baud, count, app_path, config, erase_nvs',
    [('74880', 2, BLE_PROXIMITY_SENSOR_APP_PATH, config, 'y') for config in BLE_PROXIMITY_SENSOR_C2_CONFIGS],
    indirect=True,
)
@idf_parametrize('target', ['esp32c2'], indirect=['target'])
def test_esp32c2_26mhz_nimble_proximity_sensor_prph_cent(dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_proximity_sensor_prph_cent(dut)


PERIODIC_ADV_SYNC_APP_PATH = f'{str(CUR_DIR / "ble_periodic_adv")}|{str(CUR_DIR / "ble_periodic_sync")}'
NIMBLE_PERIODIC_ADV_SYNC_TARGETS = [
    'esp32c3',
    'esp32c6',
    'esp32c5',
    'esp32h2',
    'esp32s3',
    'esp32c61',
    'esp32h21',
    'esp32h4',
    'esp32s31',
]


def _run_nimble_periodic_adv_sync_func(dut: tuple[IdfDut, IdfDut]) -> None:
    adv_dut, sync_dut = dut[0], dut[1]

    adv_bt_mac = adv_dut.expect(r'Bluetooth MAC: ' + MAC_PATTERN, timeout=30).group(1).decode('utf8')
    sync_bt_mac = sync_dut.expect(r'Bluetooth MAC: ' + MAC_PATTERN, timeout=30).group(1).decode('utf8')
    assert adv_bt_mac.lower() != sync_bt_mac.lower(), (
        'advertiser and synchronizer boards must have different Bluetooth MACs'
    )

    name = adv_dut.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    peer_name = sync_dut.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    assert peer_name == name, (
        'advertiser and synchronizer must use the same CI device name from '
        'esp_ble_periodic_get_example_name(); mismatch means sdkconfig.ci or CI ID is wrong'
    )

    adv_dut.expect_exact('NimBLE_BLE_PERIODIC_ADV: BLE Host Task Started', timeout=30)
    sync_dut.expect_exact('NimBLE_BLE_PERIODIC_SYNC: BLE Host Task Started', timeout=30)
    adv_dut.expect_exact('instance 0 started (periodic)', timeout=30)

    found = sync_dut.expect(
        r'NimBLE_BLE_PERIODIC_SYNC: Found device: addr: ' + MAC_PATTERN + rf', name: {name}',
        timeout=90,
    )
    found_addr = found.group(1).decode('utf8')
    assert found_addr.lower() == adv_bt_mac.lower(), (
        'scan must report the advertiser board identity address, not another advertiser'
    )

    sync_dut.expect_exact('NimBLE_BLE_PERIODIC_SYNC: Create sync', timeout=60)
    sync_dut.expect_exact('NimBLE_BLE_PERIODIC_SYNC: Periodic Sync Established', timeout=60)

    synced_addr = (
        sync_dut.expect(r'NimBLE_BLE_PERIODIC_SYNC: Adv Addr: ' + MAC_PATTERN, timeout=30).group(1).decode('utf8')
    )
    assert synced_addr.lower() == adv_bt_mac.lower(), (
        'periodic sync must lock to the advertiser board MAC, not another periodic advertiser'
    )

    sync_dut.expect_exact('Periodic adv report event:', timeout=60)
    sync_dut.expect_exact('data_length : 16', timeout=30)

    adv_output = adv_dut.expect(pexpect.TIMEOUT, timeout=10)
    sync_output = sync_dut.expect(pexpect.TIMEOUT, timeout=10)
    assert 'rst:' not in str(adv_output) and 'boot:' not in str(adv_output)
    assert 'rst:' not in str(sync_output) and 'boot:' not in str(sync_output)
    assert 'Periodic sync lost' not in str(sync_output)


# Case 20: NimBLE periodic advertiser / synchronizer test
# EXAMPLE_CI_ID=38
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runner')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            PERIODIC_ADV_SYNC_APP_PATH,
            'name',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', NIMBLE_PERIODIC_ADV_SYNC_TARGETS, indirect=['target'])
def test_nimble_periodic_adv_sync_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_periodic_adv_sync_func(dut)


# Case 21: NimBLE periodic advertiser / synchronizer for ESP32-C2 26MHz
# EXAMPLE_CI_ID=38
@pytest.mark.two_duts
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, target, baud, app_path, config, erase_nvs',
    [
        (
            2,
            'esp32c2|esp32c2',
            '74880',
            PERIODIC_ADV_SYNC_APP_PATH,
            'esp32c2_xtal26m',
            'y',
        ),
    ],
    indirect=True,
)
def test_nimble_periodic_adv_sync_func_esp32c2_26mhz(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_periodic_adv_sync_func(dut)


BLECENT_BLEPRPH_APP_PATH = f'{str(CUR_DIR / "bleprph")}|{str(CUR_DIR / "blecent")}'
NIMBLE_BLECENT_BLEPRPH_TARGETS = [
    'esp32',
    'esp32c3',
    'esp32c6',
    'esp32c5',
    'esp32h2',
    'esp32s3',
    'esp32c61',
    'esp32h4',
    'esp32s31',
]


def _run_nimble_blecent_bleprph_func(dut: tuple[IdfDut, IdfDut]) -> None:
    peripheral, central = dut[0], dut[1]

    peripheral_addr = peripheral.expect(r'Bluetooth MAC: ' + MAC_PATTERN, timeout=30).group(1).decode('utf8')
    central_addr = central.expect(r'Bluetooth MAC: ' + MAC_PATTERN, timeout=30).group(1).decode('utf8')
    assert peripheral_addr.lower() != central_addr.lower(), (
        'peripheral and central boards must have different Bluetooth MACs'
    )

    name = peripheral.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    peer_name = central.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    assert peer_name == name, (
        'central and peripheral must use the same CI device name from the blecent/bleprph helpers; '
        'mismatch means sdkconfig.ci or CI ID is wrong'
    )

    peripheral.expect_exact('NimBLE_BLE_PRPH: BLE Host Task Started', timeout=30)
    central.expect_exact('NimBLE_BLE_CENT: BLE Host Task Started', timeout=30)
    peripheral.expect_exact('NimBLE: Device Address:', timeout=30)

    found_addr = (
        central.expect(
            rf'NimBLE_BLE_CENT: Found device: {MAC_PATTERN}, name: {name}',
            timeout=60,
        )
        .group(1)
        .decode('utf8')
    )
    assert found_addr.lower() == peripheral_addr.lower(), (
        'scan must report the peripheral board MAC, not another advertiser with the same CI name'
    )

    connected_remote = (
        central.expect(
            r'NimBLE_BLE_CENT: Connected, conn_handle \d+, remote ' + MAC_PATTERN,
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert connected_remote.lower() == peripheral_addr.lower(), (
        'central must connect to the peripheral board MAC, not a colliding device in RF environment'
    )
    peripheral_remote = (
        peripheral.expect(
            r'NimBLE_BLE_PRPH: Connected, conn_handle \d+, remote ' + MAC_PATTERN,
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert peripheral_remote.lower() == central_addr.lower(), (
        'peripheral must see the central board MAC as its connection peer'
    )

    central.expect_exact('NimBLE: Service discovery complete; status=0', timeout=60)
    central.expect(r'NimBLE: Read complete; status=0 conn_handle=\d+', timeout=30)
    central.expect(r'NimBLE: Write complete; status=\d+ conn_handle=\d+ attr_handle=\d+', timeout=30)
    central.expect(r'NimBLE: Subscribe complete; status=0 conn_handle=\d+ attr_handle=\d+', timeout=30)
    central.expect(
        r'NimBLE: Subscribe to the custom subscribable characteristic complete; '
        r'status=0 conn_handle=\d+',
        timeout=30,
    )

    peripheral.expect(
        r'NimBLE: subscribe event; conn_handle=\d+ attr_handle=\d+ reason=\d+ prevn=\d+ curn=1',
        timeout=30,
    )
    peripheral.expect(
        r'NimBLE: subscribe event; conn_handle=\d+ attr_handle=\d+ reason=\d+ prevn=\d+ curn=1',
        timeout=30,
    )
    peripheral.expect(r'NimBLE: Characteristic write; conn_handle=\d+ attr_handle=\d+', timeout=30)
    peripheral.expect_exact('NimBLE: Notification/Indication scheduled for all subscribed peers.', timeout=30)

    central.expect(
        r'NimBLE: Write to the custom subscribable characteristic complete; '
        r'status=0 conn_handle=\d+ attr_handle=\d+',
        timeout=30,
    )
    central.expect(r'NimBLE: received notification; conn_handle=\d+ attr_handle=\d+ attr_len=1', timeout=30)
    peripheral.expect(
        r'NimBLE: notify_tx event; conn_handle=\d+ attr_handle=\d+ status=0 is_indication=0',
        timeout=30,
    )
    central.expect(
        r'NimBLE: Read complete for the subscribable characteristic; '
        r'status=0 conn_handle=\d+',
        timeout=30,
    )
    peripheral.expect(r'NimBLE: Characteristic read; conn_handle=\d+ attr_handle=\d+', timeout=30)

    out_central = central.expect(pexpect.TIMEOUT, timeout=10)
    out_peripheral = peripheral.expect(pexpect.TIMEOUT, timeout=10)
    assert 'rst:' not in str(out_central) and 'boot:' not in str(out_central)
    assert 'rst:' not in str(out_peripheral) and 'boot:' not in str(out_peripheral)
    assert 'disconnect' not in str(out_central).lower()
    assert 'disconnect' not in str(out_peripheral).lower()


# Case 22: NimBLE bleprph / blecent test
# EXAMPLE_CI_ID=39
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [(2, BLECENT_BLEPRPH_APP_PATH, 'name', 'y')],
    indirect=True,
)
@idf_parametrize('target', NIMBLE_BLECENT_BLEPRPH_TARGETS, indirect=['target'])
def test_nimble_blecent_bleprph_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_blecent_bleprph_func(dut)


# Case 23: NimBLE bleprph / blecent test for ESP32-C2 26MHz
# EXAMPLE_CI_ID=39
@pytest.mark.two_duts
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, target, baud, app_path, config, erase_nvs',
    [(2, 'esp32c2|esp32c2', '74880', BLECENT_BLEPRPH_APP_PATH, 'bleprph_esp32c2_xtal26m', 'y')],
    indirect=True,
)
def test_nimble_blecent_bleprph_func_esp32c2_26mhz(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_blecent_bleprph_func(dut)


def _run_nimble_multi_adv_func(dut: IdfDut) -> None:
    dut.expect_exact('NimBLE_MULTI_ADV: BLE Host Task Started', timeout=30)
    for instance in range(4):
        dut.expect_exact(f'NimBLE_MULTI_ADV: Instance {instance} started', timeout=30)
    dut.expect_exact('main_task: Returned from app_main()', timeout=30)

    # Instance 3 has a five-second duration and must restart after completion.
    dut.expect_exact('NimBLE: advertise complete; reason=13', timeout=10)
    dut.expect_exact('NimBLE_MULTI_ADV: Instance 3 started', timeout=30)

    output = dut.expect(pexpect.TIMEOUT, timeout=10)
    assert 'rst:' not in str(output) and 'boot:' not in str(output)
    assert 'assert failed' not in str(output).lower()
    assert 'Resetting state' not in str(output)


NIMBLE_MULTI_ADV_TARGETS = [
    'esp32c3',
    'esp32c5',
    'esp32c6',
    'esp32c61',
    'esp32h2',
    'esp32h21',
    'esp32h4',
    'esp32s3',
    'esp32s31',
]


# Case 24: NimBLE multi-adv single-DUT smoke test
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runner')
@pytest.mark.generic
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [(1, str(CUR_DIR / 'ble_multi_adv'), 'name', 'y')],
    indirect=True,
)
@idf_parametrize('target', NIMBLE_MULTI_ADV_TARGETS, indirect=['target'])
def test_nimble_multi_adv_func(dut: IdfDut) -> None:
    _run_nimble_multi_adv_func(dut)


# Case 25: NimBLE multi-adv single-DUT smoke test for ESP32-C2 26MHz
@pytest.mark.generic
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, target, baud, app_path, config, erase_nvs',
    [(1, 'esp32c2', '74880', str(CUR_DIR / 'ble_multi_adv'), 'esp32c2_xtal26m', 'y')],
    indirect=True,
)
def test_nimble_multi_adv_func_esp32c2_26mhz(dut: IdfDut) -> None:
    _run_nimble_multi_adv_func(dut)


NIMBLE_GATT_THROUGHPUT_APP_PATH = (
    f'{str(CUR_DIR / "throughput_app" / "gatt" / "bleprph_throughput")}'
    f'|{str(CUR_DIR / "throughput_app" / "gatt" / "blecent_throughput")}'
)
NIMBLE_GATT_THROUGHPUT_TARGETS = [
    'esp32',
    'esp32c3',
    'esp32c5',
    'esp32c6',
    'esp32c61',
    'esp32h2',
    'esp32h21',
    'esp32h4',
    'esp32s3',
    'esp32s31',
]


def _run_nimble_gatt_throughput_func(dut: tuple[IdfDut, IdfDut], throughput_mode: str) -> None:
    peripheral, central = dut

    peripheral_addr = peripheral.expect(r'Bluetooth MAC: ' + MAC_PATTERN, timeout=30).group(1).decode('utf8')
    central_addr = central.expect(r'Bluetooth MAC: ' + MAC_PATTERN, timeout=30).group(1).decode('utf8')
    assert peripheral_addr.lower() != central_addr.lower(), (
        'peripheral and central boards must have different Bluetooth MACs'
    )
    name = peripheral.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    peer_name = central.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    assert peer_name == name, (
        'central and peripheral must use the same CI device name from get_example_name(); '
        'mismatch means sdkconfig.ci or CI ID is wrong'
    )

    peripheral.expect_exact('bleprph_throughput: BLE Host Task Started', timeout=30)
    central.expect_exact('blecent_throughput: BLE Host Task Started', timeout=30)
    peripheral.expect_exact('bleprph_throughput: Device Address:', timeout=30)

    central.expect_exact('If yes then enter in this format: `Insert Yes`', timeout=30)
    central.write('Insert No')
    central.expect_exact('User entered: Insert No', timeout=10)

    # Boards of the same pipeline, chip and example share the CI device name, so
    # pin the central to the MAC of the board it was paired with for this run.
    central.expect_exact('Enter peer address in this format: `peer xx:xx:xx:xx:xx:xx`', timeout=10)
    central.write(f'peer {peripheral_addr}')
    central.expect_exact(f'User entered: peer {peripheral_addr}', timeout=10)
    central.expect_exact(
        f'blecent_throughput: Peer address filter set to {peripheral_addr.lower()}',
        timeout=10,
    )

    found_addr = (
        central.expect(
            r'blecent_throughput: Found device: addr: ' + MAC_PATTERN + rf', name: {name}',
            timeout=60,
        )
        .group(1)
        .decode('utf8')
    )
    assert found_addr.lower() == peripheral_addr.lower(), (
        'scan must report the peripheral board MAC, not another advertiser with the same CI name'
    )

    central.expect_exact('blecent_throughput: Connection established', timeout=30)
    peripheral.expect_exact('bleprph_throughput: connection established; status = 0', timeout=30)
    connected_remote = (
        central.expect(
            r'blecent_throughput: Connected, conn_handle \d+, remote ' + MAC_PATTERN,
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert connected_remote.lower() == peripheral_addr.lower(), (
        'central must connect to the peripheral board MAC, not another device in the RF environment'
    )
    peripheral_remote = (
        peripheral.expect(
            r'bleprph_throughput: Connected, conn_handle \d+, remote ' + MAC_PATTERN,
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert peripheral_remote.lower() == central_addr.lower(), (
        'peripheral must see the central board MAC as its connection peer'
    )

    central.expect(
        r'blecent_throughput: mtu update event; conn_handle = \d+ cid = \d+ mtu = 512',
        timeout=30,
    )
    peripheral.expect(
        r'bleprph_throughput: mtu update event; conn_handle = \d+ mtu = 512',
        timeout=30,
    )
    central.expect(
        r'blecent_throughput: Service discovery complete; status=0 conn_handle=\d+',
        timeout=60,
    )
    central.expect_exact('blecent_throughput: Format for throughput demo:: throughput read 100', timeout=10)
    central.write(f'throughput {throughput_mode} 10 0')
    central.expect_exact(f'Throughput demo handler: throughput {throughput_mode} 10 0', timeout=10)
    central.expect_exact('blecent_throughput: Requested connection parameter update', timeout=30)

    if throughput_mode == 'read':
        result = central.expect(
            r'blecent_throughput: Application Read throughput = (\d+) bps, Read op counter = (\d+)',
            timeout=30,
        )
        assert int(result.group(1)) > 0 and int(result.group(2)) > 0, (
            'read throughput and completed read count must both be positive'
        )
        central.expect_exact('blecent_throughput: Restoring WRITE-optimized conn params', timeout=10)
    elif throughput_mode == 'write':
        result = central.expect(
            r'blecent_throughput: Application Write throughput = (\d+) bps, write count = (\d+),'
            r'failure count = (\d+)',
            timeout=30,
        )
        write_count = int(result.group(2))
        assert int(result.group(1)) > 0 and write_count > 0, (
            'write throughput and attempted write count must both be positive'
        )
        assert int(result.group(3)) < write_count, 'the write test must complete at least one write successfully'
    else:
        central.expect_exact(
            'blecent_throughput: Subscribed to notifications. '
            'Throughput number can be seen on peripheral terminal after 10 seconds',
            timeout=30,
        )
        peripheral.expect(r'bleprph_throughput: subscribe event; cur_notify=1; value handle;', timeout=30)
        peripheral.expect_exact('bleprph_throughput: Notifications enabled, test time = 3600 sec', timeout=30)
        central.expect_exact('blecent_throughput: Unsubscribed from notifications', timeout=30)
        peripheral.expect_exact('bleprph_throughput: Notifications disabled', timeout=60)
        result = peripheral.expect(
            r'bleprph_throughput: Notify throughput = (\d+) bps, count = (\d+)',
            timeout=30,
        )
        assert int(result.group(1)) > 0 and int(result.group(2)) > 0, (
            'notification throughput and sent notification count must both be positive'
        )
        peripheral.expect(r'bleprph_throughput:  Notification test complete \(\d+ sec elapsed\)', timeout=10)

    out_central = central.expect(pexpect.TIMEOUT, timeout=10)
    out_peripheral = peripheral.expect(pexpect.TIMEOUT, timeout=10)
    assert 'rst:' not in str(out_central) and 'boot:' not in str(out_central)
    assert 'rst:' not in str(out_peripheral) and 'boot:' not in str(out_peripheral)
    assert 'disconnect' not in str(out_central).lower()
    assert 'disconnect' not in str(out_peripheral).lower()


# Case 26: NimBLE GATT throughput read/write/notify (EXAMPLE_CI_ID=40)
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runner')
@pytest.mark.temp_skip_ci(targets=['esp32s31'], reason='ble bugs # TODO: BLEQABR25-2509')
@pytest.mark.two_duts
@pytest.mark.parametrize('throughput_mode', ['read', 'write', 'notify'])
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [(2, NIMBLE_GATT_THROUGHPUT_APP_PATH, 'name', 'y')],
    indirect=True,
)
@idf_parametrize('target', NIMBLE_GATT_THROUGHPUT_TARGETS, indirect=['target'])
def test_nimble_gatt_throughput_func(throughput_mode: str, app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_gatt_throughput_func(dut, throughput_mode)


# Case 27: NimBLE GATT throughput for ESP32-C2 26MHz (EXAMPLE_CI_ID=40)
@pytest.mark.two_duts
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize('throughput_mode', ['read', 'write', 'notify'])
@pytest.mark.parametrize(
    'count, target, baud, app_path, config, erase_nvs',
    [
        (
            2,
            'esp32c2|esp32c2',
            '74880',
            NIMBLE_GATT_THROUGHPUT_APP_PATH,
            'esp32c2_xtal26m',
            'y',
        )
    ],
    indirect=True,
)
def test_nimble_gatt_throughput_func_esp32c2_26mhz(
    throughput_mode: str, app_path: str, dut: tuple[IdfDut, IdfDut]
) -> None:
    _run_nimble_gatt_throughput_func(dut, throughput_mode)


L2CAP_COC_APP_PATH = (
    f'{str(CUR_DIR / "ble_l2cap_coc" / "coc_bleprph")}|{str(CUR_DIR / "ble_l2cap_coc" / "coc_blecent")}'
)
NIMBLE_L2CAP_COC_TARGETS = [
    'esp32',
    'esp32c3',
    'esp32c6',
    'esp32c5',
    'esp32h2',
    'esp32s3',
    'esp32c61',
    'esp32h4',
    'esp32s31',
]


def _run_nimble_l2cap_coc_func(dut: tuple[IdfDut, IdfDut]) -> None:
    peripheral = dut[0]
    central = dut[1]

    peripheral_addr = (
        peripheral.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )
    central_addr = (
        central.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})', timeout=30).group(1).decode('utf8')
    )
    name = peripheral.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    peer_name = central.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    assert peer_name == name, (
        'central and peripheral must use the same CI device name from '
        'esp_ble_l2cap_coc_get_example_name(); mismatch means sdkconfig.ci or CI ID is wrong'
    )

    peripheral.expect_exact('NimBLE_BLE_PRPH_L2CAP_COC: BLE Host Task Started', timeout=30)
    central.expect_exact('NimBLE_BLE_CENT_L2CAP_COC: BLE Host Task Started', timeout=30)

    found_addr = (
        central.expect(
            rf'Found device: (([0-9a-fA-F]{{2}}:){{5}}[0-9a-fA-F]{{2}}), name: {name}',
            timeout=60,
        )
        .group(1)
        .decode('utf8')
    )
    assert found_addr.lower() == peripheral_addr.lower(), (
        'scan must report the peripheral board MAC, not another advertiser with the same CI name'
    )

    connected_remote = (
        central.expect(
            r'Connected, conn_handle \d+, remote (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})',
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert connected_remote.lower() == peripheral_addr.lower(), (
        'central must connect to the peripheral board MAC, not a colliding device in RF environment'
    )
    peripheral_remote = (
        peripheral.expect(
            r'Connected, conn_handle \d+, remote (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})',
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert peripheral_remote.lower() == central_addr.lower(), 'peripheral must see central board MAC as connection peer'

    central.expect_exact('LE COC connected, conn:', timeout=60)
    peripheral.expect_exact('LE COC connected, conn:', timeout=30)
    central.expect_exact('Data sent successfully', timeout=30)
    peripheral.expect_exact('Data received :', timeout=30)

    out_central = central.expect(pexpect.TIMEOUT, timeout=10)
    out_peripheral = peripheral.expect(pexpect.TIMEOUT, timeout=10)
    assert 'rst:' not in str(out_central) and 'boot:' not in str(out_central)
    assert 'rst:' not in str(out_peripheral) and 'boot:' not in str(out_peripheral)
    assert 'disconnect' not in str(out_central).lower()
    assert 'disconnect' not in str(out_peripheral).lower()


# Case 28: NimBLE L2CAP COC peripheral/central test
# EXAMPLE_CI_ID=41
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [
        (
            2,
            L2CAP_COC_APP_PATH,
            'name',
            'y',
        ),
    ],
    indirect=True,
)
@idf_parametrize('target', NIMBLE_L2CAP_COC_TARGETS, indirect=['target'])
def test_nimble_l2cap_coc_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_l2cap_coc_func(dut)


# Case 29: NimBLE L2CAP COC test for ESP32C2 26MHz
# EXAMPLE_CI_ID=41
@pytest.mark.two_duts
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'count, target, baud, app_path, config, erase_nvs',
    [
        (
            2,
            'esp32c2|esp32c2',
            '74880',
            L2CAP_COC_APP_PATH,
            'esp32c2_xtal26m',
            'y',
        ),
    ],
    indirect=True,
)
def test_nimble_l2cap_coc_func_esp32c2_26mhz(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_l2cap_coc_func(dut)


NIMBLE_L2CAP_COC_TP_APP_PATH = (
    f'{str(CUR_DIR / "throughput_app" / "l2cap_coc" / "l2cap_coc_prph")}'
    f'|{str(CUR_DIR / "throughput_app" / "l2cap_coc" / "l2cap_coc_cent")}'
)
NIMBLE_L2CAP_COC_TP_TARGETS = [
    'esp32',
    'esp32c3',
    'esp32c5',
    'esp32c6',
    'esp32c61',
    'esp32h2',
    'esp32h21',
    'esp32h4',
    'esp32s3',
    'esp32s31',
]


def _run_nimble_l2cap_coc_throughput_func(dut: tuple[IdfDut, IdfDut]) -> None:
    peripheral, central = dut

    peripheral_addr = peripheral.expect(r'Bluetooth MAC: ' + MAC_PATTERN, timeout=30).group(1).decode('utf8')
    central_addr = central.expect(r'Bluetooth MAC: ' + MAC_PATTERN, timeout=30).group(1).decode('utf8')
    assert peripheral_addr.lower() != central_addr.lower(), (
        'peripheral and central boards must have different Bluetooth MACs'
    )
    name = peripheral.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    peer_name = central.expect(r'DeviceName:(.+?)\,', timeout=30).group(1).decode('utf8')
    assert peer_name == name, (
        'central and peripheral must use the same CI device name from '
        'esp_ble_l2cap_coc_tp_get_example_name(); mismatch means sdkconfig.ci or CI ID is wrong'
    )

    peripheral.expect_exact('l2cap_coc_prph: BLE Host Task started', timeout=30)
    central.expect_exact('l2cap_coc_cent: BLE Host Task started', timeout=30)
    peripheral.expect(r'l2cap_coc_prph: Device Address: ' + MAC_PATTERN, timeout=30)
    central.expect(r'l2cap_coc_cent: Device Address: ' + MAC_PATTERN, timeout=30)

    found_addr = (
        central.expect(
            r'l2cap_coc_cent: Found device: ' + MAC_PATTERN + rf', name: {name}',
            timeout=60,
        )
        .group(1)
        .decode('utf8')
    )
    assert found_addr.lower() == peripheral_addr.lower(), (
        'scan must report the peripheral board MAC, not another advertiser with the same CI name'
    )

    connected_remote = (
        central.expect(
            r'l2cap_coc_cent: Connected, conn_handle \d+, remote ' + MAC_PATTERN,
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert connected_remote.lower() == peripheral_addr.lower(), (
        'central must connect to the peripheral board MAC, not a colliding device in RF environment'
    )
    peripheral_remote = (
        peripheral.expect(
            r'l2cap_coc_prph: Connected, conn_handle \d+, remote ' + MAC_PATTERN,
            timeout=30,
        )
        .group(1)
        .decode('utf8')
    )
    assert peripheral_remote.lower() == central_addr.lower(), (
        'peripheral must see the central board MAC as its connection peer'
    )

    central.expect_exact('l2cap_coc_cent: L2CAP COC connected, chan=', timeout=60)
    peripheral.expect_exact('l2cap_coc_prph: L2CAP COC connected, chan=', timeout=30)
    # The banner contains a non-ASCII em dash, which pexpect cannot encode in expect_exact.
    central.expect(
        r'l2cap_coc_cent: L2CAP COC Throughput .+ TX side \(central sends to peripheral\)',
        timeout=30,
    )
    phy_count = central.expect(r'l2cap_coc_cent: Number of enabled PHYs: (\d+)', timeout=30)
    assert int(phy_count.group(1)) > 0, 'at least one EXAMPLE_TEST_PHY_* must be enabled for the throughput test'
    # Classic ESP32 has no BLE 5.0 PHY update; the example applies 1M without logging PHY updated.
    if central.target != 'esp32':
        central.expect(r'l2cap_coc_cent: PHY updated: tx=\d+ rx=\d+ status=0', timeout=30)
    central.expect(r'l2cap_coc_cent: \[.+ PHY\] Sending for \d+ s', timeout=60)

    rx_result = peripheral.expect(
        r'l2cap_coc_prph: \| RX : (\d+)\s+kbps \|',
        timeout=60,
    )
    assert int(rx_result.group(1)) > 0, 'peripheral RX throughput must be positive while central is sending'

    central.expect(r'l2cap_coc_cent: \| PHY    : .+\|', timeout=60)
    tx_result = central.expect(
        r'l2cap_coc_cent: \| TX     : (\d+)\s+kbps',
        timeout=30,
    )
    assert int(tx_result.group(1)) > 0, 'central TX throughput must be positive after the measurement interval'
    bytes_result = central.expect(
        r'l2cap_coc_cent: \| Bytes  : (\d+)',
        timeout=10,
    )
    assert int(bytes_result.group(1)) > 0, 'central must report a positive number of bytes sent'

    out_central = central.expect(pexpect.TIMEOUT, timeout=10)
    out_peripheral = peripheral.expect(pexpect.TIMEOUT, timeout=10)
    assert 'rst:' not in str(out_central) and 'boot:' not in str(out_central)
    assert 'rst:' not in str(out_peripheral) and 'boot:' not in str(out_peripheral)
    assert 'disconnect' not in str(out_central).lower()
    assert 'disconnect' not in str(out_peripheral).lower()


# Case 30: NimBLE L2CAP COC throughput (EXAMPLE_CI_ID=42)
@pytest.mark.temp_skip_ci(targets=['esp32h4'], reason='lack of runner # TODO: IDFCI-11112')
@pytest.mark.temp_skip_ci(targets=['esp32h21'], reason='lack of runner')
@pytest.mark.two_duts
@pytest.mark.parametrize(
    'count, app_path, config, erase_nvs',
    [(2, NIMBLE_L2CAP_COC_TP_APP_PATH, 'name', 'y')],
    indirect=True,
)
@idf_parametrize('target', NIMBLE_L2CAP_COC_TP_TARGETS, indirect=['target'])
def test_nimble_l2cap_coc_throughput_func(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_l2cap_coc_throughput_func(dut)


# Case 31: NimBLE L2CAP COC throughput for ESP32-C2 26MHz (EXAMPLE_CI_ID=42)
@pytest.mark.two_duts
@pytest.mark.xtal_26mhz
@pytest.mark.temp_skip_ci(targets=['esp32c2'], reason='ble bug # TODO: BLEQABR25-2507')
@pytest.mark.parametrize(
    'count, target, baud, app_path, config, erase_nvs',
    [
        (
            2,
            'esp32c2|esp32c2',
            '74880',
            NIMBLE_L2CAP_COC_TP_APP_PATH,
            'esp32c2_xtal26m',
            'y',
        )
    ],
    indirect=True,
)
def test_nimble_l2cap_coc_throughput_func_esp32c2_26mhz(app_path: str, dut: tuple[IdfDut, IdfDut]) -> None:
    _run_nimble_l2cap_coc_throughput_func(dut)
