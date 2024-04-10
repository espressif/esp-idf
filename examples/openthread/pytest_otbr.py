# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
# !/usr/bin/env python3
import copy
import os.path
import re
import secrets
import subprocess
import threading
import time
from typing import Tuple

import ot_ci_function as ocf
import pexpect
import pytest
from pytest_embedded_idf.dut import IdfDut

# This file contains the test scripts for Thread:

# Case 1: Thread network formation and attaching
#         A Thread Border Router forms a Thread network, Thread devices attach to it, then test ping connection between them.

# Case 2: Bidirectional IPv6 connectivity
#         Test IPv6 ping connection between Thread device and Linux Host (via Thread Border Router).

# Case 3: Multicast forwarding from Wi-Fi to Thread network
#         Thread device joins the multicast group, then test group communication from Wi-Fi to Thread network.

# Case 4: Multicast forwarding from Thread to Wi-Fi network
#         Linux Host joins the multicast group, test group communication from Thread to Wi-Fi network.

# Case 5: discover Service published by Thread device
#         Thread device publishes the service, Linux Host discovers the service on Wi-Fi network.

# Case 6: discover Service published by W-Fi device
#         Linux Host device publishes the service on Wi-Fi network, Thread device discovers the service.

# Case 7: ICMP communication via NAT64
#         Thread device (IPV6) ping the host device (IPV4) via NAT64.

# Case 8: UDP communication via NAT64
#         Thread device (IPV6) send udp message to the host device (IPV4) via NAT64.

# Case 9: TCP communication via NAT64
#         Thread device (IPV6) send tcp message to the host device (IPV4) via NAT64.


@pytest.fixture(scope='module', name='Init_avahi')
def fixture_Init_avahi() -> bool:
    print('Init Avahi')
    ocf.start_avahi()
    time.sleep(10)
    return True


@pytest.fixture(name='Init_interface')
def fixture_Init_interface() -> bool:
    print('Init interface')
    ocf.init_interface_ipv6_address()
    ocf.reset_host_interface()
    time.sleep(30)
    ocf.set_interface_sysctl_options()
    return True


default_br_ot_para = ocf.thread_parameter('leader', '', '12', '7766554433221100', True)
default_br_wifi_para = ocf.wifi_parameter('OTCITE', 'otcitest888', 10)
default_cli_ot_para = ocf.thread_parameter('router', '', '', '', False)


# Case 1: Thread network formation and attaching
@pytest.mark.supported_targets
@pytest.mark.esp32h2
@pytest.mark.esp32c6
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=1)
@pytest.mark.parametrize(
    'config, count, app_path, target', [
        ('rcp|cli_h2|br', 3,
         f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
         'esp32c6|esp32h2|esp32s3'),
    ],
    indirect=True,
)
def test_thread_connect(dut:Tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli_h2 = dut[1]
    dut[0].serial.stop_redirect_thread()
    cli_list = [cli_h2]
    router_extaddr_list = ['7766554433221101']

    ocf.init_thread(br)
    for cli in cli_list:
        ocf.init_thread(cli)
    br_ot_para = copy.copy(default_br_ot_para)
    ocf.joinThreadNetwork(br, br_ot_para)
    cli_ot_para = copy.copy(default_cli_ot_para)
    cli_ot_para.dataset = ocf.getDataset(br)
    try:
        order = 0
        for cli in cli_list:
            cli_ot_para.exaddr = router_extaddr_list[order]
            order = order + 1
            ocf.joinThreadNetwork(cli, cli_ot_para)
        for cli in cli_list:
            cli_mleid_addr = ocf.get_mleid_addr(cli)
            br_mleid_addr = ocf.get_mleid_addr(br)
            rx_nums = ocf.ot_ping(cli, br_mleid_addr, 5)[1]
            assert rx_nums == 5
            rx_nums = ocf.ot_ping(br, cli_mleid_addr, 5)[1]
            assert rx_nums == 5
    finally:
        ocf.execute_command(br, 'factoryreset')
        for cli in cli_list:
            ocf.execute_command(cli, 'factoryreset')
        time.sleep(3)


# Form a Wi-Fi/Thread network with a Wi-Fi host, a border router and a Thread end device
# Topology:
#     Border_Router
#         /  \
#        /    \
# Wi-FI_Host   Thread_End_Device
def formBasicWiFiThreadNetwork(br:IdfDut, cli:IdfDut) -> None:
    ocf.init_thread(br)
    ocf.init_thread(cli)
    otbr_wifi_para = copy.copy(default_br_wifi_para)
    ocf.joinWiFiNetwork(br, otbr_wifi_para)
    otbr_thread_para = copy.copy(default_br_ot_para)
    ocf.joinThreadNetwork(br, otbr_thread_para)
    otcli_thread_para = copy.copy(default_cli_ot_para)
    otcli_thread_para.dataset = ocf.getDataset(br)
    otcli_thread_para.exaddr = '7766554433221101'
    ocf.joinThreadNetwork(cli, otcli_thread_para)
    ocf.wait(cli,10)


# Case 2: Bidirectional IPv6 connectivity
@pytest.mark.supported_targets
@pytest.mark.esp32h2
@pytest.mark.esp32c6
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=1)
@pytest.mark.parametrize(
    'config, count, app_path, target', [
        ('rcp|cli_h2|br', 3,
         f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
         'esp32c6|esp32h2|esp32s3'),
    ],
    indirect=True,
)
def test_Bidirectional_IPv6_connectivity(Init_interface:bool, dut: Tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli  = dut[1]
    assert Init_interface
    dut[0].serial.stop_redirect_thread()

    formBasicWiFiThreadNetwork(br, cli)
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
        command = 'ifconfig ' + interface_name + ' | grep inet6 | grep global'
        out_bytes = subprocess.check_output(command, shell=True, timeout=5)
        out_str = out_bytes.decode('utf-8')
        onlinkprefix = ocf.get_onlinkprefix(br)
        host_global_unicast_addr = re.findall(r'\W+(%s(?:\w+:){3}\w+)\W+' % onlinkprefix, str(out_str))
        rx_nums = 0
        for ip_addr in host_global_unicast_addr:
            txrx_nums = ocf.ot_ping(cli, str(ip_addr), 5)
            rx_nums = rx_nums + int(txrx_nums[1])
        assert rx_nums != 0
    finally:
        ocf.execute_command(br, 'factoryreset')
        ocf.execute_command(cli, 'factoryreset')
        time.sleep(3)


# Case 3: Multicast forwarding from Wi-Fi to Thread network
@pytest.mark.supported_targets
@pytest.mark.esp32h2
@pytest.mark.esp32c6
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=1)
@pytest.mark.parametrize(
    'config, count, app_path, target', [
        ('rcp|cli_h2|br', 3,
         f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
         'esp32c6|esp32h2|esp32s3'),
    ],
    indirect=True,
)
def test_multicast_forwarding_A(Init_interface:bool, dut: Tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli  = dut[1]
    assert Init_interface
    dut[0].serial.stop_redirect_thread()

    formBasicWiFiThreadNetwork(br, cli)
    try:
        assert ocf.is_joined_wifi_network(br)
        ocf.execute_command(br, 'bbr')
        br.expect('server16', timeout=5)
        assert ocf.thread_is_joined_group(cli)
        interface_name = ocf.get_host_interface_name()
        command = 'ping -I ' + str(interface_name) + ' -t 64 ff04::125 -c 10'
        out_str = subprocess.getoutput(command)
        print('ping result:\n', str(out_str))
        role = re.findall(r' (\d+)%', str(out_str))[0]
        assert role != '100'
        ocf.execute_command(cli, 'udp open')
        cli.expect('Done', timeout=5)
        ocf.execute_command(cli, 'udp bind :: 12350')
        cli.expect('Done', timeout=5)
        ocf.clean_buffer(cli)
        target_udp = ocf.udp_parameter('INET6', 'ff04::125', 12350, '', False, 15.0, b'hello')
        ocf.host_udp_send_message(target_udp)
        cli.expect('hello', timeout=5)
        ocf.execute_command(cli, 'udp close')
        cli.expect('Done', timeout=5)
    finally:
        ocf.execute_command(br, 'factoryreset')
        ocf.execute_command(cli, 'factoryreset')
        time.sleep(3)


# Case 4: Multicast forwarding from Thread to Wi-Fi network
@pytest.mark.supported_targets
@pytest.mark.esp32h2
@pytest.mark.esp32c6
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=1)
@pytest.mark.parametrize(
    'config, count, app_path, target', [
        ('rcp|cli_h2|br', 3,
         f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
         'esp32c6|esp32h2|esp32s3'),
    ],
    indirect=True,
)
def test_multicast_forwarding_B(Init_interface:bool, dut: Tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli  = dut[1]
    assert Init_interface
    dut[0].serial.stop_redirect_thread()

    formBasicWiFiThreadNetwork(br, cli)
    try:
        assert ocf.is_joined_wifi_network(br)
        ocf.execute_command(br, 'bbr')
        br.expect('server16', timeout=5)
        ocf.execute_command(cli, 'udp open')
        cli.expect('Done', timeout=5)
        ocf.wait(cli, 3)
        myudp = ocf.udp_parameter('INET6', '::', 5090, 'ff04::125', False, 15.0, b'')
        udp_mission = threading.Thread(target=ocf.create_host_udp_server, args=(myudp, ))
        udp_mission.start()
        start_time = time.time()
        while not myudp.init_flag:
            if (time.time() - start_time) > 10:
                assert False
        for num in range(0, 3):
            command = 'udp send ff04::125 5090 hello' + str(num)
            ocf.execute_command(cli, command)
            cli.expect('Done', timeout=5)
            ocf.wait(cli, 0.5)
        while udp_mission.is_alive():
            time.sleep(1)
    finally:
        ocf.execute_command(br, 'factoryreset')
        ocf.execute_command(cli, 'factoryreset')
        time.sleep(3)
    assert b'hello' in myudp.udp_bytes


# Case 5: discover dervice published by Thread device
@pytest.mark.supported_targets
@pytest.mark.esp32h2
@pytest.mark.esp32c6
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=1)
@pytest.mark.parametrize(
    'config, count, app_path, target', [
        ('rcp|cli_h2|br', 3,
         f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
         'esp32c6|esp32h2|esp32s3'),
    ],
    indirect=True,
)
def test_service_discovery_of_Thread_device(Init_interface:bool, Init_avahi:bool, dut: Tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli  = dut[1]
    assert Init_interface
    assert Init_avahi
    dut[0].serial.stop_redirect_thread()

    formBasicWiFiThreadNetwork(br, cli)
    try:
        assert ocf.is_joined_wifi_network(br)
        command = 'avahi-browse -rt _testyyy._udp'
        out_str = subprocess.getoutput(command)
        print('avahi-browse:\n', str(out_str))
        assert 'myTest' not in str(out_str)
        hostname = 'myTest'
        command = 'srp client host name ' + hostname
        ocf.execute_command(cli, command)
        cli.expect('Done', timeout=5)
        cli_global_unicast_addr = ocf.get_global_unicast_addr(cli, br)
        print('cli_global_unicast_addr', cli_global_unicast_addr)
        command = 'srp client host address ' + str(cli_global_unicast_addr)
        ocf.execute_command(cli, command)
        cli.expect('Done', timeout=5)
        port = '12348'
        command = 'srp client service add my-service _testyyy._udp ' + port
        ocf.execute_command(cli, command)
        cli.expect('Done', timeout=5)
        ocf.execute_command(cli, 'srp client autostart enable')
        cli.expect('Done', timeout=5)
        ocf.wait(cli, 3)
        command = 'avahi-browse -rt _testyyy._udp'
        out_str = subprocess.getoutput(command)
        print('avahi-browse:\n', str(out_str))
        assert 'myTest' in str(out_str)
    finally:
        ocf.execute_command(br, 'factoryreset')
        ocf.execute_command(cli, 'factoryreset')
        time.sleep(3)


# Case 6: discover dervice published by Wi-Fi device
@pytest.mark.supported_targets
@pytest.mark.esp32h2
@pytest.mark.esp32c6
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=1)
@pytest.mark.parametrize(
    'config, count, app_path, target', [
        ('rcp|cli_h2|br', 3,
         f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
         'esp32c6|esp32h2|esp32s3'),
    ],
    indirect=True,
)
def test_service_discovery_of_WiFi_device(Init_interface:bool, Init_avahi:bool, dut: Tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli  = dut[1]
    assert Init_interface
    assert Init_avahi
    dut[0].serial.stop_redirect_thread()

    formBasicWiFiThreadNetwork(br, cli)
    try:
        assert ocf.is_joined_wifi_network(br)
        br_global_unicast_addr = ocf.get_global_unicast_addr(br, br)
        command = 'dns config ' + br_global_unicast_addr
        ocf.execute_command(cli, command)
        cli.expect('Done', timeout=5)
        ocf.wait(cli, 1)
        domain_name = ocf.get_domain()
        print('domain name is: ', domain_name)
        command = 'dns resolve ' + domain_name + '.default.service.arpa.'

        ocf.execute_command(cli, command)
        cli.expect('TTL', timeout=10)
        cli.expect('Done', timeout=10)

        command = 'dns browse _testxxx._udp.default.service.arpa'
        tmp = ocf.get_ouput_string(cli, command, 5)
        assert 'Port:12347' not in str(tmp)
        ocf.host_publish_service()
        ocf.wait(cli, 5)

        command = 'dns browse _testxxx._udp.default.service.arpa'
        tmp = ocf.get_ouput_string(cli, command, 5)
        assert 'response for _testxxx' in str(tmp)
        assert 'Port:12347' in str(tmp)

        command = 'dns browse _testxxx._udp.default.service.arpa'
        tmp = ocf.get_ouput_string(cli, command, 5)
        ocf.execute_command(cli, 'dns service testxxx _testxxx._udp.default.service.arpa.')
        tmp = cli.expect(pexpect.TIMEOUT, timeout=5)
        assert 'response for testxxx' in str(tmp)
        assert 'Port:12347' in str(tmp)
    finally:
        ocf.host_close_service()
        ocf.execute_command(br, 'factoryreset')
        ocf.execute_command(cli, 'factoryreset')
        time.sleep(3)


# Case 7: ICMP communication via NAT64
@pytest.mark.supported_targets
@pytest.mark.esp32h2
@pytest.mark.esp32c6
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=1)
@pytest.mark.parametrize(
    'config, count, app_path, target', [
        ('rcp|cli_h2|br', 3,
         f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
         'esp32c6|esp32h2|esp32s3'),
    ],
    indirect=True,
)
def test_ICMP_NAT64(Init_interface:bool, dut: Tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli  = dut[1]
    assert Init_interface
    dut[0].serial.stop_redirect_thread()

    formBasicWiFiThreadNetwork(br, cli)
    try:
        assert ocf.is_joined_wifi_network(br)
        host_ipv4_address = ocf.get_host_ipv4_address()
        print('host_ipv4_address: ', host_ipv4_address)
        rx_nums = ocf.ot_ping(cli, str(host_ipv4_address), 5)[1]
        assert rx_nums != 0
    finally:
        ocf.execute_command(br, 'factoryreset')
        ocf.execute_command(cli, 'factoryreset')
        time.sleep(3)


# Case 8: UDP communication via NAT64
@pytest.mark.supported_targets
@pytest.mark.esp32h2
@pytest.mark.esp32c6
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=1)
@pytest.mark.parametrize(
    'config, count, app_path, target', [
        ('rcp|cli_h2|br', 3,
         f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
         'esp32c6|esp32h2|esp32s3'),
    ],
    indirect=True,
)
def test_UDP_NAT64(Init_interface:bool, dut: Tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli  = dut[1]
    assert Init_interface
    dut[0].serial.stop_redirect_thread()

    formBasicWiFiThreadNetwork(br, cli)
    try:
        assert ocf.is_joined_wifi_network(br)
        ocf.execute_command(br, 'bbr')
        br.expect('server16', timeout=5)
        ocf.execute_command(cli, 'udp open')
        cli.expect('Done', timeout=5)
        ocf.wait(cli, 3)
        host_ipv4_address = ocf.get_host_ipv4_address()
        print('host_ipv4_address: ', host_ipv4_address)
        myudp = ocf.udp_parameter('INET4', host_ipv4_address, 5090, '', False, 15.0, b'')
        udp_mission = threading.Thread(target=ocf.create_host_udp_server, args=(myudp, ))
        udp_mission.start()
        start_time = time.time()
        while not myudp.init_flag:
            if (time.time() - start_time) > 10:
                assert False
        for num in range(0, 3):
            command = 'udp send ' + host_ipv4_address + ' 5090 hello' + str(num)
            ocf.execute_command(cli, command)
            cli.expect('Done', timeout=5)
            ocf.wait(cli, 0.5)
        while udp_mission.is_alive():
            time.sleep(1)
    finally:
        ocf.execute_command(br, 'factoryreset')
        ocf.execute_command(cli, 'factoryreset')
        time.sleep(3)
    assert b'hello' in myudp.udp_bytes


# Case 9: TCP communication via NAT64
@pytest.mark.supported_targets
@pytest.mark.esp32h2
@pytest.mark.esp32c6
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=1)
@pytest.mark.parametrize(
    'config, count, app_path, target', [
        ('rcp|cli_h2|br', 3,
         f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
         'esp32c6|esp32h2|esp32s3'),
    ],
    indirect=True,
)
def test_TCP_NAT64(Init_interface:bool, dut: Tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli  = dut[1]
    assert Init_interface
    dut[0].serial.stop_redirect_thread()

    formBasicWiFiThreadNetwork(br, cli)
    try:
        assert ocf.is_joined_wifi_network(br)
        ocf.execute_command(br, 'bbr')
        br.expect('server16', timeout=5)
        ocf.execute_command(cli, 'tcpsockclient open')
        cli.expect('Done', timeout=5)
        ocf.wait(cli, 3)
        host_ipv4_address = ocf.get_host_ipv4_address()
        connect_address = ocf.get_ipv6_from_ipv4(host_ipv4_address, br)
        print('connect_address is: ', connect_address)
        mytcp = ocf.tcp_parameter('INET4', host_ipv4_address, 12345, False, False, 15.0, b'')
        tcp_mission = threading.Thread(target=ocf.create_host_tcp_server, args=(mytcp, ))
        tcp_mission.start()
        start_time = time.time()
        while not mytcp.listen_flag:
            if (time.time() - start_time) > 10:
                assert False
        command = 'tcpsockclient connect ' + connect_address + ' 12345'
        ocf.execute_command(cli, command)
        cli.expect('Successfully connected', timeout=10)
        start_time = time.time()
        while not mytcp.recv_flag:
            if (time.time() - start_time) > 10:
                assert False
        command = 'tcpsockclient send hello'
        ocf.execute_command(cli, command)
        cli.expect('Done', timeout=5)
        while tcp_mission.is_alive():
            time.sleep(1)
    finally:
        ocf.execute_command(br, 'factoryreset')
        ocf.execute_command(cli, 'factoryreset')
        time.sleep(3)
    assert b'hello' in mytcp.tcp_bytes


# Case 10: Sleepy device test
@pytest.mark.esp32h2
@pytest.mark.esp32c6
@pytest.mark.openthread_sleep
@pytest.mark.parametrize(
    'config, count, app_path, target', [
        ('cli_h2|sleepy_c6', 2,
         f'{os.path.join(os.path.dirname(__file__), "ot_cli")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_sleepy_device/light_sleep")}',
         'esp32h2|esp32c6'),
        ('cli_c6|sleepy_h2', 2,
         f'{os.path.join(os.path.dirname(__file__), "ot_cli")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_sleepy_device/light_sleep")}',
         'esp32c6|esp32h2'),
    ],
    indirect=True,
)
def test_ot_sleepy_device(dut: Tuple[IdfDut, IdfDut]) -> None:
    leader = dut[0]
    sleepy_device = dut[1]
    fail_info = re.compile(r'Core\W*?\d\W*?register dump')
    try:
        ocf.init_thread(leader)
        time.sleep(3)
        leader_para = ocf.thread_parameter('leader', '', '12', '7766554433221100', False)
        ocf.joinThreadNetwork(leader, leader_para)
        ocf.wait(leader, 5)
        dataset = ocf.getDataset(leader)
        ocf.execute_command(sleepy_device, 'mode -')
        ocf.execute_command(sleepy_device, 'pollperiod 3000')
        ocf.execute_command(sleepy_device, 'dataset set active ' + dataset)
        ocf.execute_command(sleepy_device, 'ifconfig up')
        ocf.execute_command(sleepy_device, 'thread start')
        info = sleepy_device.expect(r'(.+)detached -> child', timeout=20)[1].decode(errors='replace')
        assert not bool(fail_info.search(str(info)))
        info = sleepy_device.expect(r'(.+)PMU_SLEEP_PD_TOP: True', timeout=10)[1].decode(errors='replace')
        assert not bool(fail_info.search(str(info)))
        info = sleepy_device.expect(r'(.+)PMU_SLEEP_PD_MODEM: True', timeout=20)[1].decode(errors='replace')
        assert not bool(fail_info.search(str(info)))
        output = sleepy_device.expect(pexpect.TIMEOUT, timeout=20)
        assert not bool(fail_info.search(str(output)))
        ocf.clean_buffer(sleepy_device)
        ocf.execute_command(leader, 'factoryreset')
        output = sleepy_device.expect(pexpect.TIMEOUT, timeout=5)
        assert not bool(fail_info.search(str(output)))
    finally:
        ocf.execute_command(leader, 'factoryreset')
        time.sleep(3)


# Case 11: Basic startup Test of BR
@pytest.mark.supported_targets
@pytest.mark.esp32h2
@pytest.mark.esp32c6
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=1)
@pytest.mark.parametrize(
    'config, count, app_path, target', [
        ('rcp|br', 2,
         f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
         'esp32c6|esp32s3'),
    ],
    indirect=True,
)
def test_basic_startup(dut: Tuple[IdfDut, IdfDut]) -> None:
    br = dut[1]
    dut[0].serial.stop_redirect_thread()
    try:
        ocf.init_thread(br)
        time.sleep(3)
        ocf.clean_buffer(br)
        ocf.execute_command(br, 'ifconfig up')
        br.expect('Done', timeout=5)
        ocf.execute_command(br, 'thread start')
        br.expect('Done', timeout=5)
        assert ocf.wait_for_join(br, 'leader')
        ocf.reset_thread(br)
        ocf.joinWiFiNetwork(br, default_br_wifi_para)
        ocf.execute_command(br, 'ifconfig up')
        br.expect('Done', timeout=5)
        ocf.execute_command(br, 'thread start')
        br.expect('Done', timeout=5)
        assert ocf.wait_for_join(br, 'leader')
    finally:
        ocf.execute_command(br, 'factoryreset')
        time.sleep(3)


# Case 12: Curl a website via DNS and NAT64
@pytest.mark.supported_targets
@pytest.mark.esp32h2
@pytest.mark.esp32c6
@pytest.mark.openthread_bbr
@pytest.mark.flaky(reruns=1, reruns_delay=1)
@pytest.mark.parametrize(
    'config, count, app_path, target', [
        ('rcp|cli_h2|br', 3,
         f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
         'esp32c6|esp32h2|esp32s3'),
    ],
    indirect=True,
)
def test_NAT64_DNS(Init_interface:bool, dut: Tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli  = dut[1]
    assert Init_interface
    dut[0].serial.stop_redirect_thread()

    formBasicWiFiThreadNetwork(br, cli)
    try:
        ocf.execute_command(br, 'bbr')
        br.expect('server16', timeout=5)
        ocf.execute_command(cli, 'dns64server 8.8.8.8')
        cli.expect('Done', timeout=5)
        command = 'curl http://www.espressif.com'
        message = ocf.get_ouput_string(cli, command, 10)
        assert '<html>' in str(message)
        assert '301 Moved Permanently' in str(message)
    finally:
        ocf.execute_command(br, 'factoryreset')
        ocf.execute_command(cli, 'factoryreset')
        time.sleep(3)


# Case 13: Meshcop discovery of Border Router
@pytest.mark.supported_targets
@pytest.mark.esp32c6
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=1)
@pytest.mark.parametrize(
    'config, count, app_path, target', [
        ('rcp|br', 2,
         f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
         f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
         'esp32c6|esp32s3'),
    ],
    indirect=True,
)
def test_br_meshcop(Init_interface:bool, Init_avahi:bool, dut: Tuple[IdfDut, IdfDut]) -> None:
    br = dut[1]
    assert Init_interface
    assert Init_avahi
    dut[0].serial.stop_redirect_thread()

    result = None
    output_bytes = b''
    try:
        ocf.init_thread(br)
        br_wifi_para = copy.copy(default_br_wifi_para)
        ipv4_address = ocf.joinWiFiNetwork(br, br_wifi_para)[0]
        br_thread_para = copy.copy(default_br_ot_para)
        networkname = 'OTCI-' + str(secrets.token_hex(1))
        br_thread_para.setnetworkname(networkname)
        ocf.joinThreadNetwork(br, br_thread_para)
        ocf.wait(br, 10)
        assert ocf.is_joined_wifi_network(br)
        command = 'timeout 3 avahi-browse -r _meshcop._udp'
        try:
            result = subprocess.run(command, capture_output=True, check=True, shell=True)
            if result:
                output_bytes = result.stdout
        except subprocess.CalledProcessError as e:
            output_bytes = e.stdout
        finally:
            print('out_bytes: ', output_bytes)
            output_str = str(output_bytes)
            print('out_str: ', output_str)

            assert 'hostname = [esp-ot-br.local]' in str(output_str)
            assert ('address = [' + ipv4_address + ']') in str(output_str)
            assert 'dn=DefaultDomain' in str(output_str)
            assert 'tv=1.4.0' in str(output_str)
            assert ('nn=' + networkname) in str(output_str)
            assert 'mn=BorderRouter' in str(output_str)
            assert 'vn=OpenThread' in str(output_str)
            assert 'rv=1' in str(output_str)
    finally:
        ocf.execute_command(br, 'factoryreset')
        time.sleep(3)
