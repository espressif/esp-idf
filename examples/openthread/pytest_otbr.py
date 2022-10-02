# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
# !/usr/bin/env python3


import os.path
import re
import socket
import struct
import subprocess
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
    ocf.set_interface_sysctl_options()
    return True


# Case 1: Thread network formation and attaching
@pytest.mark.esp32s3
@pytest.mark.esp32h2
@pytest.mark.i154_multi_dut
@pytest.mark.flaky(reruns=2, reruns_delay=10)
@pytest.mark.parametrize(
    'port, config, count, app_path, beta_target, target', [
        ('/dev/USB_BR|/dev/USB_CLI|/dev/USB_RCP', 'br|cli|rcp', 3,
         f'{os.path.join(os.path.dirname(__file__), "ot_br")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_rcp")}',
         'esp32s3|esp32h2beta2|esp32h2beta2', 'esp32s3|esp32h2|esp32h2'),
    ],
    indirect=True,
)
def test_thread_connect(dut:Tuple[IdfDut, IdfDut]) -> None:
    br = dut[0]
    cli  = dut[1]

    dataset = '-1'
    ocf.form_network_using_manual_configuration(br, cli, 'br', 'random', dataset, br, 'OTCITE', '0000')
    time.sleep(1)
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
@pytest.mark.esp32h2
@pytest.mark.i154_multi_dut
@pytest.mark.flaky(reruns=5, reruns_delay=10)
@pytest.mark.parametrize(
    'port, config, count, app_path, beta_target, target', [
        ('/dev/USB_BR|/dev/USB_CLI|/dev/USB_RCP', 'br|cli|rcp', 3,
         f'{os.path.join(os.path.dirname(__file__), "ot_br")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_rcp")}',
         'esp32s3|esp32h2beta2|esp32h2beta2', 'esp32s3|esp32h2|esp32h2'),
    ],
    indirect=True,
)
def test_Bidirectional_IPv6_connectivity(Init_interface:bool, dut: Tuple[IdfDut, IdfDut]) -> None:
    br = dut[0]
    cli  = dut[1]
    assert Init_interface

    dataset = '-1'
    ocf.form_network_using_manual_configuration(br, cli, 'br', 'random', dataset, br, 'OTCITE', 'otcitest888')
    time.sleep(5)
    cli_global_unicast_addr = ocf.get_global_unicast_addr(cli, br)
    flag = False
    try:
        command = 'ping ' + str(cli_global_unicast_addr) + ' -c 10'
        out_bytes = subprocess.check_output(command, shell=True, timeout=60)
        out_str = out_bytes.decode('utf-8')
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
@pytest.mark.esp32h2
@pytest.mark.i154_multi_dut
@pytest.mark.flaky(reruns=5, reruns_delay=10)
@pytest.mark.parametrize(
    'port, config, count, app_path, beta_target, target', [
        ('/dev/USB_BR|/dev/USB_CLI|/dev/USB_RCP', 'br|cli|rcp', 3,
         f'{os.path.join(os.path.dirname(__file__), "ot_br")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_rcp")}',
         'esp32s3|esp32h2beta2|esp32h2beta2', 'esp32s3|esp32h2|esp32h2'),
    ],
    indirect=True,
)
def test_multicast_forwarding_A(Init_interface:bool, dut: Tuple[IdfDut, IdfDut]) -> None:
    br = dut[0]
    cli  = dut[1]
    assert Init_interface

    dataset = '-1'
    ocf.form_network_using_manual_configuration(br, cli, 'br', 'random', dataset, br, 'OTCITE', 'otcitest888')
    time.sleep(5)
    flag = False
    try:
        br.write('bbr')
        br.expect('server16', timeout=2)
        cli.write('mcast join ff04::125')
        cli.expect('Done', timeout=2)
        time.sleep(1)
        interface_name = ocf.get_host_interface_name()
        command = 'ping -I ' + str(interface_name) + ' -t 64 ff04::125 -c 10'
        out_bytes = subprocess.check_output(command, shell=True, timeout=60)
        out_str = out_bytes.decode('utf-8')
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
@pytest.mark.esp32h2
@pytest.mark.i154_multi_dut
@pytest.mark.flaky(reruns=5, reruns_delay=5)
@pytest.mark.parametrize(
    'port, config, count, app_path, beta_target, target', [
        ('/dev/USB_BR|/dev/USB_CLI|/dev/USB_RCP', 'br|cli|rcp', 3,
         f'{os.path.join(os.path.dirname(__file__), "ot_br")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_rcp")}',
         'esp32s3|esp32h2beta2|esp32h2beta2', 'esp32s3|esp32h2|esp32h2'),
    ],
    indirect=True,
)
def test_multicast_forwarding_B(Init_interface:bool, dut: Tuple[IdfDut, IdfDut]) -> None:
    br = dut[0]
    cli  = dut[1]
    assert Init_interface

    dataset = '-1'
    ocf.form_network_using_manual_configuration(br, cli, 'br', 'random', dataset, br, 'OTCITE', 'otcitest888')
    time.sleep(5)
    br.write('bbr')
    br.expect('server16', timeout=2)
    interface_name = ocf.get_host_interface_name()
    if_index = socket.if_nametoindex(interface_name)
    sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
    sock.bind(('::', 5090))
    sock.setsockopt(
        socket.IPPROTO_IPV6, socket.IPV6_JOIN_GROUP,
        struct.pack('16si', socket.inet_pton(socket.AF_INET6, 'ff04::125'),
                    if_index))
    time.sleep(1)
    cli.write('udp open')
    cli.expect('Done', timeout=2)
    cli.write('udp send ff04::125 5090 hello')
    cli.expect('Done', timeout=2)
    data = b''
    try:
        print('The host start to receive message!')
        sock.settimeout(5)
        data = (sock.recvfrom(1024))[0]
        print('The host has received message!')
    except socket.error:
        print('The host did not received message!')
    finally:
        sock.close()
        br.write('factoryreset')
        cli.write('factoryreset')
        time.sleep(3)
    assert data == b'hello'
