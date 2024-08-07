# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import os.path
from typing import Tuple

import pexpect
import pytest
from pytest_embedded_idf.dut import IdfDut


# Case 1: SPP
@pytest.mark.esp32
@pytest.mark.wifi_two_dut
@pytest.mark.parametrize(
    'count, app_path, target, erase_all', [
        (2,
         f'{os.path.join(os.path.dirname(__file__), "bt_spp_acceptor")}|{os.path.join(os.path.dirname(__file__), "bt_spp_initiator")}',
         'esp32|esp32', 'y'),
    ],
    indirect=True,
)
def test_bt_spp_only(app_path: str, dut: Tuple[IdfDut, IdfDut]) -> None:
    acceptor = dut[0]
    initiator = dut[1]

    acceptor.expect_exact('ESP_SPP_INIT_EVT', timeout=30)
    acceptor.expect_exact('ESP_SPP_START_EVT', timeout=30)
    initiator.expect_exact('ESP_SPP_INIT_EVT', timeout=30)
    initiator.expect_exact('ESP_SPP_DISCOVERY_COMP_EVT', timeout=30)
    initiator_output = initiator.expect(pexpect.TIMEOUT, timeout=5)

    if 'ESP_BT_GAP_KEY_REQ_EVT Please enter passkey!' in str(initiator_output):
        passkey = acceptor.expect(r'ESP_BT_GAP_KEY_NOTIF_EVT passkey:(\d+)').group(1).decode('utf8')
        initiator.write(f'spp key {passkey};')
        acceptor.expect_exact('authentication success', timeout=30)
        initiator.expect_exact('authentication success', timeout=30)
    acceptor.expect_exact('ESP_SPP_SRV_OPEN_EVT status:0', timeout=30)
    initiator.expect_exact('ESP_SPP_OPEN_EVT', timeout=30)


# Case 2: SPP_VFS
@pytest.mark.esp32
@pytest.mark.wifi_two_dut
@pytest.mark.parametrize(
    'count, app_path, target', [
        (2,
         f'{os.path.join(os.path.dirname(__file__), "bt_spp_vfs_acceptor")}|{os.path.join(os.path.dirname(__file__), "bt_spp_vfs_initiator")}',
         'esp32|esp32'),
    ],
    indirect=True,
)
def test_bt_spp_vfs(app_path: str, dut: Tuple[IdfDut, IdfDut]) -> None:
    acceptor = dut[0]
    initiator = dut[1]

    acceptor.expect_exact('ESP_SPP_INIT_EVT', timeout=30)
    acceptor.expect_exact('ESP_SPP_VFS_REGISTER_EVT', timeout=30)
    acceptor.expect_exact('ESP_SPP_START_EVT', timeout=30)
    initiator.expect_exact('ESP_SPP_INIT_EVT', timeout=30)
    initiator.expect_exact('ESP_SPP_VFS_REGISTER_EVT', timeout=30)
    initiator.expect_exact('ESP_SPP_DISCOVERY_COMP_EVT', timeout=30)
    initiator.expect_exact('ESP_SPP_OPEN_EVT', timeout=30)
    acceptor.expect_exact('ESP_SPP_SRV_OPEN_EVT status:0', timeout=30)


# Case 3: A2DP
@pytest.mark.esp32
@pytest.mark.wifi_two_dut
@pytest.mark.parametrize(
    'count, app_path, target', [
        (2,
         f'{os.path.join(os.path.dirname(__file__), "a2dp_sink")}|{os.path.join(os.path.dirname(__file__), "a2dp_source")}',
         'esp32|esp32'),
    ],
    indirect=True,
)
def test_bt_a2dp(app_path: str, dut: Tuple[IdfDut, IdfDut]) -> None:
    sink_dut = dut[0]
    source_dut = dut[1]
    source_dut_mac = source_dut.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})').group(1).decode('utf8')
    sink_dut.expect_exact('A2DP PROF STATE: Init Complete', timeout=30)
    source_dut.expect_exact('a2dp connecting to peer', timeout=30)
    source_dut.expect_exact('a2dp connected', timeout=30)
    source_dut.expect_exact('a2dp media start successfully', timeout=30)
    sink_dut.expect_exact(f'A2DP connection state: Connected, [{source_dut_mac}]', timeout=30)
    sink_dut.expect_exact('start volume change simulation', timeout=30)


# Case 4: HFP
@pytest.mark.esp32
@pytest.mark.wifi_two_dut
@pytest.mark.parametrize(
    'count, app_path, target, config', [
        (2,
         f'{os.path.join(os.path.dirname(__file__), "hfp_ag")}|{os.path.join(os.path.dirname(__file__), "hfp_hf")}',
         'esp32|esp32', 'all'),
    ],
    indirect=True,
)
def test_bt_hfp(app_path: str, dut: Tuple[IdfDut, IdfDut]) -> None:
    hfp_ag = dut[0]
    hfp_hf = dut[1]

    hfp_hf.expect_exact('Found a target device name', timeout=30)
    hfp_hf.expect_exact('connection state connected', timeout=30)
    hfp_hf.expect_exact('connection state slc_connected', timeout=30)
    hfp_ag.expect_exact('connection state CONNECTED', timeout=30)
    hfp_ag.expect_exact('connection state SLC_CONNECTED', timeout=30)


# # Case 5: HID
@pytest.mark.esp32
@pytest.mark.wifi_two_dut
@pytest.mark.parametrize(
    'count, app_path, target', [
        (2,
         f'{os.path.join(os.path.dirname(__file__), "bt_hid_mouse_device")}| \
           {os.path.join(os.path.abspath(os.path.join(os.path.dirname(__file__), "../..")), "esp_hid_host")}',
         'esp32|esp32'),
    ],
    indirect=True,
)
def test_bt_hid(app_path: str, dut: Tuple[IdfDut, IdfDut]) -> None:
    hid_device = dut[0]
    hid_host = dut[1]

    hid_host_mac = hid_host.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})').group(1).decode('utf8')
    hid_device_mac = hid_device.expect(r'Bluetooth MAC: (([0-9a-fA-F]{2}:){5}[0-9a-fA-F]{2})').group(1).decode('utf8')

    hid_device.expect_exact('setting hid parameters', timeout=30)
    hid_device.expect_exact('setting to connectable, discoverable', timeout=30)
    hid_device.expect_exact(f'connected to {hid_host_mac}', timeout=30)
    hid_device.expect_exact('making self non-discoverable and non-connectable', timeout=30)
    hid_host.expect_exact(f'ESP_HIDH_DEMO: {hid_device_mac} OPEN', timeout=30)


# Case 6: L2CAP
@pytest.mark.esp32
@pytest.mark.wifi_two_dut
@pytest.mark.parametrize(
    'count, app_path, target', [
        (2,
         f'{os.path.join(os.path.dirname(__file__), "bt_l2cap_server")}|{os.path.join(os.path.dirname(__file__), "bt_l2cap_client")}',
         'esp32|esp32'),
    ],
    indirect=True,
)
def test_bt_l2cap(app_path: str, dut: Tuple[IdfDut, IdfDut]) -> None:
    server = dut[0]
    client = dut[1]

    server.expect_exact('ESP_BT_L2CAP_INIT_EVT: status:0', timeout=30)
    server.expect_exact('ESP_BT_L2CAP_START_EVT: status:0', timeout=30)
    server.expect_exact('ESP_SDP_CREATE_RECORD_COMP_EVT: status:0', timeout=30)
    client.expect_exact('ESP_BT_L2CAP_INIT_EVT: status:0', timeout=30)
    client.expect_exact('ESP_SDP_SEARCH_COMP_EVT: status:0', timeout=30)
    client.expect_exact('ESP_BT_L2CAP_OPEN_EVT: status:0', timeout=30)
    server.expect_exact('ESP_BT_L2CAP_OPEN_EVT: status:0', timeout=30)
