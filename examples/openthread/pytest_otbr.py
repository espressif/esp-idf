# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
# !/usr/bin/env python3


import os.path
import re
import subprocess
import threading
import time
from typing import Tuple

import ot_ci_function as ocf
import pytest
from pytest_embedded_idf.dut import IdfDut

# This file contains the test scripts for Thread:

# Case 1: Thread network formation and attaching
#         A Thread Border Router forms a Thread network, a Thread device attaches to it, then test ping connection between them.

# Case 2: Bidirectional IPv6 connectivity
#         Test IPv6 ping connection between Thread device and Linux Host (via Thread Border Router).

# Case 3: Multicast forwarding from Wi-Fi to Thread network
#         Thread device joins the multicast group, then test group communication from Wi-Fi to Thread network.

# Case 4: Multicast forwarding from Thread to Wi-Fi network
#         Linux Host joins the multicast group, test group communication from Thread to Wi-Fi network.


@pytest.fixture(name='Init_interface')
def fixture_Init_interface() -> bool:
    ocf.init_interface_ipv6_address()
    ocf.reset_host_interface()
    time.sleep(30)
    ocf.set_interface_sysctl_options()
    return True


wifi_ssid = 'OTCITE'
wifi_psk = 'otcitest888'


# Case 1: Thread network formation and attaching
@pytest.mark.esp32s3
@pytest.mark.esp32h4
@pytest.mark.timeout(40 * 60)
@pytest.mark.i154_multi_dut
@pytest.mark.parametrize(
    'port, config, count, app_path, beta_target, target', [
        ('/dev/USB_RCP|/dev/USB_CLI|/dev/USB_BR', 'rcp|cli|br', 3,
         f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
         'esp32h2beta2|esp32h2beta2|esp32s3', 'esp32h4|esp32h4|esp32s3'),       # No need to rename beta_target as it is still called h2 in esptool
    ],
    indirect=True,
)
def test_thread_connect(dut:Tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli  = dut[1]
    dut[0].close()

    dataset = '-1'
    ocf.config_network(br, cli, 'br', 'random', dataset, br, wifi_ssid, '0000')
    flag = False
    try:
        cli_mleid_addr = ocf.get_mleid_addr(cli)
        br_mleid_addr = ocf.get_mleid_addr(br)
        rx_nums = ocf.ot_ping(cli, br_mleid_addr, 5)[1]
        assert rx_nums != 0
        rx_nums = ocf.ot_ping(br, cli_mleid_addr, 5)[1]
        assert rx_nums != 0
        flag = True
    finally:
        br.write('factoryreset')
        cli.write('factoryreset')
        time.sleep(3)
        assert flag


# Case 2: Bidirectional IPv6 connectivity
@pytest.mark.esp32s3
@pytest.mark.esp32h4
@pytest.mark.timeout(40 * 60)
@pytest.mark.i154_multi_dut
@pytest.mark.parametrize(
    'port, config, count, app_path, beta_target, target', [
        ('/dev/USB_RCP|/dev/USB_CLI|/dev/USB_BR', 'rcp|cli|br', 3,
         f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
         'esp32h2beta2|esp32h2beta2|esp32s3', 'esp32h4|esp32h4|esp32s3'),
    ],
    indirect=True,
)
def test_Bidirectional_IPv6_connectivity(Init_interface:bool, dut: Tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli  = dut[1]
    assert Init_interface
    dut[0].close()

    dataset = '-1'
    ocf.config_network(br, cli, 'br', 'random', dataset, br, wifi_ssid, wifi_psk)
    flag = False
    try:
        assert ocf.is_joined_wifi_network(br)
        cli_global_unicast_addr = ocf.get_global_unicast_addr(cli, br)
        print('cli_global_unicast_addr', cli_global_unicast_addr)
        command = 'ping ' + str(cli_global_unicast_addr) + ' -c 10'
        out_str = subprocess.getoutput(command)
        print('ping result:\n', str(out_str))
        role = re.findall(r' (\d+)%', str(out_str))[0]
        assert role != '100'
        interface_name = ocf.get_host_interface_name()
        command = 'ifconfig ' + interface_name
        out_bytes = subprocess.check_output(command, shell=True, timeout=5)
        out_str = out_bytes.decode('utf-8')
        host_global_unicast_addr = re.findall(r'inet6 ((?:\w+:){7}\w+)  prefixlen 64  scopeid 0x0<global>', str(out_str))
        rx_nums = 0
        for ip_addr in host_global_unicast_addr:
            txrx_nums = ocf.ot_ping(cli, str(ip_addr), 5)
            rx_nums = rx_nums + int(txrx_nums[1])
        assert rx_nums != 0
        flag = True
    finally:
        br.write('factoryreset')
        cli.write('factoryreset')
        time.sleep(3)
        assert flag


# Case 3: Multicast forwarding from Wi-Fi to Thread network
@pytest.mark.esp32s3
@pytest.mark.esp32h4
@pytest.mark.timeout(40 * 60)
@pytest.mark.i154_multi_dut
@pytest.mark.parametrize(
    'port, config, count, app_path, beta_target, target', [
        ('/dev/USB_RCP|/dev/USB_CLI|/dev/USB_BR', 'rcp|cli|br', 3,
         f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
         'esp32h2beta2|esp32h2beta2|esp32s3', 'esp32h4|esp32h4|esp32s3'),
    ],
    indirect=True,
)
def test_multicast_forwarding_A(Init_interface:bool, dut: Tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli  = dut[1]
    assert Init_interface
    dut[0].close()

    dataset = '-1'
    ocf.config_network(br, cli, 'br', 'random', dataset, br, wifi_ssid, wifi_psk)
    flag = False
    try:
        assert ocf.is_joined_wifi_network(br)
        br.write('bbr')
        br.expect('server16', timeout=2)
        assert ocf.thread_is_joined_group(cli)
        interface_name = ocf.get_host_interface_name()
        command = 'ping -I ' + str(interface_name) + ' -t 64 ff04::125 -c 10'
        out_str = subprocess.getoutput(command)
        print('ping result:\n', str(out_str))
        role = re.findall(r' (\d+)%', str(out_str))[0]
        assert role != '100'
        flag = True
    finally:
        br.write('factoryreset')
        cli.write('factoryreset')
        time.sleep(3)
        assert flag


# Case 4: Multicast forwarding from Thread to Wi-Fi network
@pytest.mark.esp32s3
@pytest.mark.esp32h4
@pytest.mark.timeout(40 * 60)
@pytest.mark.i154_multi_dut
@pytest.mark.parametrize(
    'port, config, count, app_path, beta_target, target', [
        ('/dev/USB_RCP|/dev/USB_CLI|/dev/USB_BR', 'rcp|cli|br', 3,
         f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
         'esp32h2beta2|esp32h2beta2|esp32s3', 'esp32h4|esp32h4|esp32s3'),
    ],
    indirect=True,
)
def test_multicast_forwarding_B(Init_interface:bool, dut: Tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli  = dut[1]
    assert Init_interface
    dut[0].close()

    dataset = '-1'
    ocf.config_network(br, cli, 'br', 'random', dataset, br, wifi_ssid, wifi_psk)
    try:
        assert ocf.is_joined_wifi_network(br)
        br.write('bbr')
        br.expect('server16', timeout=2)
        cli.write('udp open')
        cli.expect('Done', timeout=2)
        ocf.wait(cli, 1)
        cli.write('udp open')
        cli.expect('Already', timeout=2)
        myudp = ocf.udp_parameter('ff04::125', False, 15.0, b'')
        udp_mission = threading.Thread(target=ocf.create_host_udp_server, args=(myudp, ))
        udp_mission.start()
        start_time = time.time()
        while not myudp.try_join_udp_group:
            if (time.time() - start_time) > 10:
                assert False
        assert ocf.host_joined_group()
        for num in range(0, 3):
            command = 'udp send ff04::125 5090 hello' + str(num)
            cli.write(command)
            cli.expect('Done', timeout=2)
            ocf.wait(cli, 0.5)
        while udp_mission.is_alive():
            time.sleep(1)
    finally:
        br.write('factoryreset')
        cli.write('factoryreset')
        time.sleep(3)
    assert b'hello' in myudp.udp_bytes
