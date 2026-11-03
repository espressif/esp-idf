# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import ipaddress
import socket
import time

import pytest
from pytest_embedded_idf import IdfDut
from pytest_embedded_idf.utils import idf_parametrize


# ----------- IP101 -----------
@pytest.mark.eth_ip101
@pytest.mark.parametrize(
    'config',
    [
        'default_generic',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_esp_emac(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='esp_emac', timeout=240)


@pytest.mark.eth_ip101
@pytest.mark.parametrize(
    'config',
    [
        'rmii_clko_esp32',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_esp32_emac_clko(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='esp_emac_clk_out')


# ----------- IP101 ESP32P4 -----------
@pytest.mark.parametrize(
    'config, target',
    [
        pytest.param('default_generic_esp32p4', 'esp32p4', marks=[pytest.mark.eth_ip101]),
        pytest.param('default_generic_esp32p4v1', 'esp32p4', marks=[pytest.mark.eth_ip101, pytest.mark.esp32p4_rev1]),
    ],
    indirect=['target'],
)
def test_esp32p4_emac(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='esp_emac', timeout=240)


@pytest.mark.parametrize(
    'config, target',
    [
        pytest.param('rmii_clko_esp32p4', 'esp32p4', marks=[pytest.mark.eth_ip101]),
        pytest.param('rmii_clko_esp32p4v1', 'esp32p4', marks=[pytest.mark.eth_ip101, pytest.mark.esp32p4_rev1]),
    ],
    indirect=['target'],
)
def test_esp32p4_emac_clko(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='esp_emac_clk_out')


# ----------- EMAC Sleep Modes -----------
@pytest.mark.parametrize(
    'config, target',
    [
        pytest.param('default_generic', 'esp32', marks=[pytest.mark.eth_ip101]),
        pytest.param('default_generic_esp32p4', 'esp32p4', marks=[pytest.mark.eth_ip101]),
        pytest.param('default_generic_esp32s31', 'esp32s31', marks=[pytest.mark.eth_yt8531]),
    ],
    indirect=['target'],
)
def test_emac_sleep_modes(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='sleep_mode', timeout=120)


# ----------- EMAC Sleep Retention -----------
@pytest.mark.parametrize(
    'config, target',
    [
        pytest.param('emac_sleep_no_sleep', 'esp32p4', marks=[pytest.mark.eth_ip101]),
        # Note: ESP32P4 does not support wake on lan
        pytest.param('emac_sleep_retention', 'esp32p4', marks=[pytest.mark.eth_ip101]),
        pytest.param('emac_sleep_no_sleep', 'esp32s31', marks=[pytest.mark.eth_yt8531]),
        pytest.param('emac_sleep_wake_on_lan', 'esp32s31', marks=[pytest.mark.eth_yt8531]),
        # ALLOW_PD needs RMII with an external reference clock. YT8531 runners are RGMII.
        pytest.param(
            'emac_sleep_retention',
            'esp32s31',
            marks=[
                pytest.mark.eth_yt8531,
                pytest.mark.temp_skip_ci(
                    targets=['esp32s31'],
                    reason='no runner with RMII and an external reference clock',
                ),
            ],
        ),
    ],
    indirect=['target'],
)
def test_emac_sleep_retention(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='sleep_retention', timeout=120)


# ----------- EMAC Wake on LAN -----------
@pytest.mark.parametrize(
    'config, target',
    [
        pytest.param('emac_sleep_wake_on_lan', 'esp32s31', marks=[pytest.mark.eth_yt8531]),
    ],
    indirect=['target'],
)
def test_emac_sleep_wol(dut: IdfDut) -> None:
    """Ping, wake from a magic packet, ping, then wake from the timer with no second WoL packet."""
    _WOL_CASE_NAME = 'internal emac sleep retention - wake on lan'

    case = next((item for item in dut.test_menu if item.name == _WOL_CASE_NAME and item.type == 'normal'), None)
    if case is None:
        raise AssertionError(f'{_WOL_CASE_NAME!r} is not in the test menu')

    # test_menu resets the DUT. _get_ready waits for the menu, including when reset is unsupported.
    dut._get_ready(timeout=20)
    dut.confirm_write(case.index, expect_str=f'Running {_WOL_CASE_NAME}...', timeout=10)

    # GOT_IP log and the MAC line are printed from different tasks; accept either order.
    dut_ip = dut.expect(r'esp_netif_handlers: .+ ip: (\d+\.\d+\.\d+\.\d+)').group(1).decode()
    netmask = dut.expect(r'mask: (\d+\.\d+\.\d+\.\d+)').group(1).decode()
    mac = dut.expect(r'WoL MAC ([0-9a-fA-F]{2}(?::[0-9a-fA-F]{2}){5})').group(1).decode()

    broadcast = str(ipaddress.IPv4Network(f'{dut_ip}/{netmask}', strict=False).broadcast_address)

    # First sleep: the host delivers the magic packet once the CPU is stopped.
    dut.expect('Entering light sleep for', timeout=60)
    time.sleep(0.5)
    # send magic packet
    mac_bytes = bytes(int(octet, 16) for octet in mac.split(':'))
    packet = b'\xff' * 6 + mac_bytes * 16
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_BROADCAST, 1)
        sock.sendto(packet, (broadcast, 9))

    # Second sleep: send nothing and let the timer wake the DUT.
    dut.expect('Entering light sleep for', timeout=60)

    dut.expect_unity_test_output()
