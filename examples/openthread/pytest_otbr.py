# SPDX-FileCopyrightText: 2022-2026 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
# !/usr/bin/env python3
import copy
import logging
import os.path
import random
import re
import secrets
import subprocess
import sys
import threading
import time

sys.path.append(os.path.dirname(os.path.abspath(__file__)))
import ot_ci_function as ocf
import pexpect
import pytest
from pytest_embedded_idf.dut import IdfDut

# This file contains the test scripts for Thread:

# Case 1: Thread network formation and attaching
#         A Thread Border Router forms a Thread network, Thread devices attach to it, then test ping
#         connection between them.

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

# Case 10: Sleepy device test
#         Start a Thread sleepy device, wait it join the Thread network and check related flags.

# Case 11: Basic startup Test of BR
#         Test the basic startup and network formation of a Thread device.

# Case 12: Curl a website via DNS and NAT64
#         A border router joins a Wi-Fi network and forms a Thread network, a Thread devices attached to it and curl
#         a website.

# Case 13: Meshcop discovery of Border Router
#         A border router joins a Wi-Fi network, forms a Thread network and publish a meshcop service. Linux Host device
#         discover the mescop service.

# Case 14: Curl a website over HTTPS via DNS and NAT64
#         A border router joins a Wi-Fi network and forms a Thread network, a Thread devices attached to it and curl
#         a https website.

# Case 15: Thread network formation and attaching with TREL
#         A TREL device forms a Thread network, other TREL devices attach to it, then test ping connection between them.

# Case 16: Thread network BR lib check
#         Check BR library compatibility

# Case 17: Synchronized sleepy end device (SSED) test
#         Start a Thread ssed device, wait it join the Thread network and check related flags.


@pytest.fixture(scope='module', name='Init_avahi')
def fixture_Init_avahi() -> bool:
    logging.info('Init Avahi')
    ocf.start_avahi()
    time.sleep(10)
    return True


@pytest.fixture(name='Init_interface')
def fixture_Init_interface() -> bool:
    logging.info('Init interface')
    ocf.flush_ipv6_addr_by_interface()
    # The sleep time is set based on experience; reducing it might cause the host to be unready.
    time.sleep(30)
    ocf.set_interface_sysctl_options()
    return True


default_br_ot_para = ocf.thread_parameter('leader', '', '12', '7766554433221100', True)
default_br_wifi_para = ocf.wifi_parameter('OTCITE', 'otcitest888', 10)
default_cli_ot_para = ocf.thread_parameter('router', '', '', '', False)
ESPPORT1 = os.getenv('ESPPORT1')
ESPPORT2 = os.getenv('ESPPORT2')
ESPPORT3 = os.getenv('ESPPORT3')
ESPPORT4 = os.getenv('ESPPORT4')

PORT_MAPPING = {'ESPPORT1': 'esp32h2', 'ESPPORT2': 'esp32s3', 'ESPPORT3': 'esp32c6', 'ESPPORT4': 'esp32c5'}


# Case 1: Thread network formation and attaching
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=5)
@pytest.mark.parametrize(
    'config, count, app_path, target, port',
    [
        pytest.param(
            'rcp_uart|cli|br',
            3,
            f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
            'esp32c6|esp32h2|esp32s3',
            f'{ESPPORT3}|{ESPPORT1}|{ESPPORT2}',
            id='c6-h2-s3',
        ),
        pytest.param(
            'rcp_spi|cli|br_spi',
            3,
            f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
            'esp32h2|esp32c6|esp32s3',
            f'{ESPPORT1}|{ESPPORT3}|{ESPPORT2}',
            id='h2-c6-s3',
        ),
    ],
    indirect=True,
)
def test_thread_connect(dut: tuple[IdfDut, IdfDut, IdfDut]) -> None:
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
            rx_nums = ocf.ot_ping(cli, br_mleid_addr, count=5)[1]
            assert rx_nums == 5
            rx_nums = ocf.ot_ping(br, cli_mleid_addr, count=5)[1]
            assert rx_nums == 5
    finally:
        for cli in cli_list:
            ocf.stop_thread(cli)
        ocf.stop_thread(br)
        time.sleep(3)


# Form a Wi-Fi/Thread network with a Wi-Fi host, a border router and a Thread end device
# Topology:
#     Border_Router
#         /  \
#        /    \
# Wi-FI_Host   Thread_End_Device
def formBasicWiFiThreadNetwork(br: IdfDut, cli: IdfDut) -> None:
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
    ocf.wait(cli, 10)


# Case 2: Bidirectional IPv6 connectivity
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=5)
@pytest.mark.parametrize(
    'config, count, app_path, target, port',
    [
        pytest.param(
            'rcp_uart|cli|br',
            3,
            f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
            'esp32c6|esp32h2|esp32s3',
            f'{ESPPORT3}|{ESPPORT1}|{ESPPORT2}',
            id='c6-h2-s3',
        ),
    ],
    indirect=True,
)
def test_Bidirectional_IPv6_connectivity(Init_interface: bool, dut: tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli = dut[1]
    assert Init_interface
    dut[0].serial.stop_redirect_thread()

    formBasicWiFiThreadNetwork(br, cli)
    try:
        assert ocf.is_joined_wifi_network(br)
        cli_global_unicast_addr = ocf.get_global_unicast_addr(cli, br)
        logging.info(f'cli_global_unicast_addr {cli_global_unicast_addr}')
        command = 'ping ' + str(cli_global_unicast_addr) + ' -c 10'
        out_str = subprocess.getoutput(command)
        logging.info(f'ping result:\n{out_str}')
        role = re.findall(r' (\d+)%', str(out_str))[0]
        assert role != '100'
        interface_name = ocf.get_host_interface_name()
        command = 'ifconfig ' + interface_name + ' | grep inet6 | grep global'
        out_bytes = subprocess.check_output(command, shell=True, timeout=5)
        out_str = out_bytes.decode('utf-8')
        onlinkprefix = ocf.get_onlinkprefix(br)
        pattern = rf'\W+({onlinkprefix}(?:\w+:){{3}}\w+)\W+'
        host_global_unicast_addr = re.findall(pattern, out_str)
        logging.info(f'host_global_unicast_addr: {host_global_unicast_addr}')
        if host_global_unicast_addr is None:
            raise Exception(f'onlinkprefix: {onlinkprefix}, host_global_unicast_addr: {host_global_unicast_addr}')
        rx_nums = 0
        for ip_addr in host_global_unicast_addr:
            txrx_nums = ocf.ot_ping(cli, str(ip_addr), count=10)
            rx_nums = rx_nums + int(txrx_nums[1])
        logging.info(f'rx_nums: {rx_nums}')
        assert rx_nums != 0
    finally:
        ocf.stop_thread(cli)
        ocf.stop_thread(br)
        time.sleep(3)


# Case 3: Multicast forwarding from Wi-Fi to Thread network
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=5)
@pytest.mark.parametrize(
    'config, count, app_path, target, port',
    [
        pytest.param(
            'rcp_uart|cli|br',
            3,
            f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
            'esp32c6|esp32h2|esp32s3',
            f'{ESPPORT3}|{ESPPORT1}|{ESPPORT2}',
            id='c6-h2-s3',
        ),
    ],
    indirect=True,
)
def test_multicast_forwarding_A(Init_interface: bool, dut: tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli = dut[1]
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
        logging.info(f'ping result:\n{out_str}')
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
        ocf.stop_thread(cli)
        ocf.stop_thread(br)
        time.sleep(3)


# Case 4: Multicast forwarding from Thread to Wi-Fi network
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=5)
@pytest.mark.parametrize(
    'config, count, app_path, target, port',
    [
        pytest.param(
            'rcp_uart|cli|br',
            3,
            f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
            'esp32c6|esp32h2|esp32s3',
            f'{ESPPORT3}|{ESPPORT1}|{ESPPORT2}',
            id='c6-h2-s3',
        ),
    ],
    indirect=True,
)
def test_multicast_forwarding_B(Init_interface: bool, dut: tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli = dut[1]
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
        udp_mission = threading.Thread(target=ocf.create_host_udp_server, args=(myudp,))
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
        ocf.stop_thread(cli)
        ocf.stop_thread(br)
        time.sleep(3)
    assert b'hello' in myudp.udp_bytes


# Case 5: discover dervice published by Thread device
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=5)
@pytest.mark.parametrize(
    'config, count, app_path, target, port',
    [
        pytest.param(
            'rcp_uart|cli|br',
            3,
            f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
            'esp32c6|esp32h2|esp32s3',
            f'{ESPPORT3}|{ESPPORT1}|{ESPPORT2}',
            id='c6-h2-s3',
        ),
    ],
    indirect=True,
)
def test_service_discovery_of_Thread_device(
    Init_interface: bool, Init_avahi: bool, dut: tuple[IdfDut, IdfDut, IdfDut]
) -> None:
    br = dut[2]
    cli = dut[1]
    assert Init_interface
    assert Init_avahi
    dut[0].serial.stop_redirect_thread()

    formBasicWiFiThreadNetwork(br, cli)
    try:
        assert ocf.is_joined_wifi_network(br)
        command = 'avahi-browse -rt _testyyy._udp'
        out_str = subprocess.getoutput(command)
        logging.info(f'avahi-browse:\n{out_str}')
        assert 'myTest' not in str(out_str)
        hostname = 'myTest'
        command = 'srp client host name ' + hostname
        ocf.execute_command(cli, command)
        cli.expect('Done', timeout=5)
        cli_global_unicast_addr = ocf.get_global_unicast_addr(cli, br)
        logging.info(f'cli_global_unicast_addr {cli_global_unicast_addr}')
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
        logging.info(f'avahi-browse:\n {out_str}')
        assert 'myTest' in str(out_str)
    finally:
        ocf.stop_thread(cli)
        ocf.stop_thread(br)
        time.sleep(3)


# Case 6: discover dervice published by Wi-Fi device
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=5)
@pytest.mark.parametrize(
    'config, count, app_path, target, port',
    [
        pytest.param(
            'rcp_uart|cli|br',
            3,
            f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
            'esp32c6|esp32h2|esp32s3',
            f'{ESPPORT3}|{ESPPORT1}|{ESPPORT2}',
            id='c6-h2-s3',
        ),
    ],
    indirect=True,
)
def test_service_discovery_of_WiFi_device(
    Init_interface: bool, Init_avahi: bool, dut: tuple[IdfDut, IdfDut, IdfDut]
) -> None:
    br = dut[2]
    cli = dut[1]
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
        logging.info(f'domain name is: {domain_name}')
        command = 'dns resolve ' + domain_name + '.default.service.arpa.'

        ocf.execute_command(cli, command)
        cli.expect('TTL', timeout=10)
        cli.expect('Done', timeout=10)

        command = 'dns browse _testxxx._udp.default.service.arpa'
        tmp = ocf.get_ouput_string(cli, command, 10)
        assert 'Port:12347' not in str(tmp)
        ocf.restart_avahi()
        command = 'avahi-publish-service testxxx _testxxx._udp 12347 test=1235 dn="for_ci_br_test"'
        sp = subprocess.Popen(command, shell=True)
        time.sleep(2)
        ocf.wait(cli, 5)

        command = 'dns browse _testxxx._udp.default.service.arpa'
        tmp = ocf.get_ouput_string(cli, command, 10)
        assert 'response for _testxxx' in str(tmp)
        assert 'Port:12347' in str(tmp)

        command = 'dns service testxxx _testxxx._udp.default.service.arpa.'
        tmp = ocf.get_ouput_string(cli, command, 10)
        assert 'response for testxxx' in str(tmp)
        assert 'Port:12347' in str(tmp)
    finally:
        ocf.host_close_service()
        sp.terminate()
        ocf.stop_thread(cli)
        ocf.stop_thread(br)
        time.sleep(3)


# Case 7: ICMP communication via NAT64
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=5)
@pytest.mark.parametrize(
    'config, count, app_path, target, port',
    [
        pytest.param(
            'rcp_uart|cli|br',
            3,
            f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
            'esp32c6|esp32h2|esp32s3',
            f'{ESPPORT3}|{ESPPORT1}|{ESPPORT2}',
            id='c6-h2-s3',
        ),
    ],
    indirect=True,
)
def test_ICMP_NAT64(Init_interface: bool, dut: tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli = dut[1]
    assert Init_interface
    dut[0].serial.stop_redirect_thread()

    formBasicWiFiThreadNetwork(br, cli)
    try:
        assert ocf.is_joined_wifi_network(br)
        host_ipv4_address = ocf.get_host_ipv4_address()
        logging.info(f'host_ipv4_address: {host_ipv4_address}')
        rx_nums = ocf.ot_ping(cli, str(host_ipv4_address), count=5)[1]
        assert rx_nums != 0
    finally:
        ocf.stop_thread(cli)
        ocf.stop_thread(br)
        time.sleep(3)


# Case 8: UDP communication via NAT64
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=5)
@pytest.mark.parametrize(
    'config, count, app_path, target, port',
    [
        pytest.param(
            'rcp_uart|cli|br',
            3,
            f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
            'esp32c6|esp32h2|esp32s3',
            f'{ESPPORT3}|{ESPPORT1}|{ESPPORT2}',
            id='c6-h2-s3',
        ),
    ],
    indirect=True,
)
def test_UDP_NAT64(Init_interface: bool, dut: tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli = dut[1]
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
        logging.info(f'host_ipv4_address: {host_ipv4_address}')
        myudp = ocf.udp_parameter('INET4', host_ipv4_address, 5090, '', False, 15.0, b'')
        udp_mission = threading.Thread(target=ocf.create_host_udp_server, args=(myudp,))
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
        ocf.stop_thread(cli)
        ocf.stop_thread(br)
        time.sleep(3)
    assert b'hello' in myudp.udp_bytes


# Case 9: TCP communication via NAT64
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=5)
@pytest.mark.parametrize(
    'config, count, app_path, target, port',
    [
        pytest.param(
            'rcp_uart|cli|br',
            3,
            f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
            'esp32c6|esp32h2|esp32s3',
            f'{ESPPORT3}|{ESPPORT1}|{ESPPORT2}',
            id='c6-h2-s3',
        ),
    ],
    indirect=True,
)
def test_TCP_NAT64(Init_interface: bool, dut: tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli = dut[1]
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
        logging.info(f'connect_address is: {connect_address}')
        mytcp = ocf.tcp_parameter('INET4', host_ipv4_address, 12345, False, False, 15.0, b'')
        tcp_mission = threading.Thread(target=ocf.create_host_tcp_server, args=(mytcp,))
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
        ocf.stop_thread(cli)
        ocf.stop_thread(br)
        time.sleep(3)
    assert b'hello' in mytcp.tcp_bytes


# Case 10: Sleepy device test
@pytest.mark.openthread_sleep
@pytest.mark.flaky(reruns=1, reruns_delay=5)
@pytest.mark.parametrize(
    'config, count, app_path, target, port',
    [
        pytest.param(
            'cli|sleepy',
            2,
            f'{os.path.join(os.path.dirname(__file__), "ot_cli")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_sleepy_device/light_sleep")}',
            'esp32h2|esp32c6',
            f'{ESPPORT1}|{ESPPORT3}',
            id='h2-c6',
        ),
        pytest.param(
            'cli|sleepy',
            2,
            f'{os.path.join(os.path.dirname(__file__), "ot_cli")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_sleepy_device/light_sleep")}',
            'esp32c6|esp32h2',
            f'{ESPPORT3}|{ESPPORT1}',
            id='c6-h2',
        ),
        pytest.param(
            'cli|sleepy',
            2,
            f'{os.path.join(os.path.dirname(__file__), "ot_cli")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_sleepy_device/light_sleep")}',
            'esp32h2|esp32c5',
            f'{ESPPORT1}|{ESPPORT4}',
            id='h2-c5',
        ),
    ],
    indirect=True,
)
def test_ot_sleepy_device(dut: tuple[IdfDut, IdfDut]) -> None:
    leader = dut[0]
    sleepy_device = dut[1]
    ocf.hardreset_dut(sleepy_device)
    fail_info = re.compile(r'Core\W*?\d\W*?register dump')
    try:
        ocf.init_thread(leader)
        time.sleep(3)
        leader_para = ocf.thread_parameter('leader', '', '12', '7766554433221100', False)
        ocf.joinThreadNetwork(leader, leader_para)
        ocf.wait(leader, 5)
        dataset = ocf.getDataset(leader)
        ocf.execute_command(sleepy_device, 'mode -')
        sleepy_device.expect('Done', timeout=5)
        ocf.execute_command(sleepy_device, 'pollperiod 3000')
        sleepy_device.expect('Done', timeout=5)
        ocf.execute_command(sleepy_device, 'dataset set active ' + dataset)
        sleepy_device.expect('Done', timeout=5)
        ocf.execute_command(sleepy_device, 'ifconfig up')
        sleepy_device.expect('Done', timeout=5)
        ocf.execute_command(sleepy_device, 'thread start')
        sleepy_device.expect('Done', timeout=5)
        info = sleepy_device.expect(r'(.+)detached -> child', timeout=20)[1].decode(errors='replace')
        assert not bool(fail_info.search(str(info)))
        info = sleepy_device.expect(r'(.+)PMU_SLEEP_PD_TOP: True', timeout=10)[1].decode(errors='replace')
        assert not bool(fail_info.search(str(info)))
        info = sleepy_device.expect(r'(.+)PMU_SLEEP_PD_MODEM: True', timeout=20)[1].decode(errors='replace')
        assert not bool(fail_info.search(str(info)))
        output = sleepy_device.expect(pexpect.TIMEOUT, timeout=20)
        assert not bool(fail_info.search(str(output)))
        ocf.execute_command(leader, 'factoryreset')
        output = sleepy_device.expect(pexpect.TIMEOUT, timeout=5)
        assert not bool(fail_info.search(str(output)))
    finally:
        logging.info('Cleaning up...')
        ocf.execute_command(leader, 'factoryreset')
        leader.expect('OpenThread attached to netif', timeout=20)
        ocf.hardreset_dut(sleepy_device)
        time.sleep(3)


# Case 11: Basic startup Test of BR
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=5)
@pytest.mark.parametrize(
    'config, count, app_path, target, port',
    [
        pytest.param(
            'rcp_uart|br',
            2,
            f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}|{os.path.join(os.path.dirname(__file__), "ot_br")}',
            'esp32c6|esp32s3',
            f'{ESPPORT3}|{ESPPORT2}',
            id='c6-s3',
        ),
    ],
    indirect=True,
)
def test_basic_startup(dut: tuple[IdfDut, IdfDut]) -> None:
    br = dut[1]
    dut[0].serial.stop_redirect_thread()
    try:
        ocf.init_thread(br)
        time.sleep(3)
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
        ocf.stop_thread(br)
        time.sleep(3)


# Case 12: Curl a website via DNS and NAT64
@pytest.mark.openthread_bbr
@pytest.mark.flaky(reruns=1, reruns_delay=5)
@pytest.mark.parametrize(
    'config, count, app_path, target, port',
    [
        pytest.param(
            'rcp_uart|cli|br',
            3,
            f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
            'esp32c6|esp32h2|esp32s3',
            f'{ESPPORT3}|{ESPPORT1}|{ESPPORT2}',
            id='c6-h2-s3',
        ),
    ],
    indirect=True,
)
def test_NAT64_DNS(Init_interface: bool, dut: tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli = dut[1]
    assert Init_interface
    dut[0].serial.stop_redirect_thread()

    formBasicWiFiThreadNetwork(br, cli)
    try:
        ocf.wait_for_host_network()
        ocf.execute_command(br, 'bbr')
        br.expect('server16', timeout=5)
        ocf.execute_command(cli, 'dns64server 8.8.8.8')
        cli.expect('Done', timeout=5)
        command = 'curl http://www.espressif.com'
        message = ocf.get_ouput_string(cli, command, 10)
        assert 'html' in str(message)
        assert '301 Moved Permanently' in str(message)
    finally:
        ocf.stop_thread(cli)
        ocf.stop_thread(br)
        time.sleep(3)


# Case 13: Meshcop discovery of Border Router
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=5)
@pytest.mark.parametrize(
    'config, count, app_path, target, port',
    [
        pytest.param(
            'rcp_uart|br',
            2,
            f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}|{os.path.join(os.path.dirname(__file__), "ot_br")}',
            'esp32c6|esp32s3',
            f'{ESPPORT3}|{ESPPORT2}',
            id='c6-s3',
        ),
    ],
    indirect=True,
)
def test_br_meshcop(Init_interface: bool, Init_avahi: bool, dut: tuple[IdfDut, IdfDut]) -> None:
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
            logging.info(f'out_bytes: {output_bytes!r}')
            output_str = str(output_bytes)
            logging.info(f'out_str: {output_str}')

            assert 'hostname = [esp-ot-br.local]' in str(output_str)
            assert ('address = [' + ipv4_address + ']') in str(output_str)
            assert 'dn=DefaultDomain' in str(output_str)
            assert 'tv=1.4.0' in str(output_str)
            assert ('nn=' + networkname) in str(output_str)
            assert 'mn=BorderRouter' in str(output_str)
            assert 'vn=OpenThread' in str(output_str)
            assert 'rv=1' in str(output_str)
    finally:
        ocf.stop_thread(br)
        time.sleep(3)


# Case 14: Curl a website over HTTPS via DNS and NAT64
@pytest.mark.openthread_bbr
@pytest.mark.flaky(reruns=1, reruns_delay=5)
@pytest.mark.parametrize(
    'config, count, app_path, target, port',
    [
        pytest.param(
            'rcp_uart|cli|br',
            3,
            f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_cli")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_br")}',
            'esp32c6|esp32h2|esp32s3',
            f'{ESPPORT3}|{ESPPORT1}|{ESPPORT2}',
            id='c6-h2-s3',
        ),
    ],
    indirect=True,
)
def test_https_NAT64_DNS(Init_interface: bool, dut: tuple[IdfDut, IdfDut, IdfDut]) -> None:
    br = dut[2]
    cli = dut[1]
    assert Init_interface
    dut[0].serial.stop_redirect_thread()

    formBasicWiFiThreadNetwork(br, cli)
    try:
        ocf.wait_for_host_network()
        ocf.execute_command(cli, 'dns64server 8.8.8.8')
        cli.expect('Done', timeout=5)
        command = 'curl https://www.example.com/'
        message = ocf.get_ouput_string(cli, command, 20)
        assert 'html' in str(message)
        assert 'This domain is for use in' in str(message)
    finally:
        ocf.stop_thread(cli)
        ocf.stop_thread(br)
        time.sleep(3)


# Case 15: Thread network formation and attaching with TREL
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=5)
@pytest.mark.parametrize(
    'config, count, app_path, target, port',
    [
        pytest.param(
            'trel|trel',
            2,
            f'{os.path.join(os.path.dirname(__file__), "ot_trel")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_trel")}',
            'esp32c6|esp32s3',
            f'{ESPPORT3}|{ESPPORT2}',
            id='c6-s3',
        ),
    ],
    indirect=True,
)
def test_trel_connect(dut: tuple[IdfDut, IdfDut]) -> None:
    trel_s3 = dut[1]
    trel_c6 = dut[0]
    trel_list = [trel_c6]
    router_extaddr_list = ['7766554433221101']

    trel_s3.expect('IPv4 address:', timeout=10)
    trel_c6.expect('IPv4 address:', timeout=10)
    ocf.init_thread(trel_s3)
    for trel in trel_list:
        ocf.init_thread(trel)
    trel_leader_para = copy.copy(default_br_ot_para)
    trel_leader_para.bbr = False
    ocf.joinThreadNetwork(trel_s3, trel_leader_para)
    trel_para = copy.copy(default_cli_ot_para)
    trel_para.dataset = ocf.getDataset(trel_s3)
    try:
        order = 0
        for trel in trel_list:
            trel_para.exaddr = router_extaddr_list[order]
            order = order + 1
            ocf.joinThreadNetwork(trel, trel_para)
        for trel in trel_list:
            trel_mleid_addr = ocf.get_mleid_addr(trel)
            trel_s3_mleid_addr = ocf.get_mleid_addr(trel_s3)
            rx_nums = ocf.ot_ping(trel, trel_s3_mleid_addr, count=10)[1]
            assert rx_nums > 5
            rx_nums = ocf.ot_ping(trel_s3, trel_mleid_addr, count=10)[1]
            assert rx_nums > 5
    finally:
        for trel in trel_list:
            ocf.stop_thread(trel)
        ocf.stop_thread(trel_s3)
        time.sleep(3)


# Case 16: Thread network BR lib check
@pytest.mark.openthread_br
@pytest.mark.flaky(reruns=1, reruns_delay=5)
@pytest.mark.parametrize(
    'config, count, app_path, target, port',
    [
        pytest.param(
            'rcp_uart|br_libcheck',
            2,
            f'{os.path.join(os.path.dirname(__file__), "ot_rcp")}|{os.path.join(os.path.dirname(__file__), "ot_br")}',
            'esp32c6|esp32s3',
            f'{ESPPORT3}|{ESPPORT2}',
            id='c6-s3',
        ),
    ],
    indirect=True,
)
def test_br_lib_check(dut: tuple[IdfDut, IdfDut]) -> None:
    br = dut[1]
    dut[0].serial.stop_redirect_thread()
    try:
        time.sleep(3)
        ocf.execute_command(br, 'brlibcheck')
        br.expect('The br library compatibility checking passed', timeout=10)
    finally:
        ocf.execute_command(br, 'factoryreset')
        time.sleep(3)


# Case 17: SSED test
@pytest.mark.openthread_sleep
@pytest.mark.flaky(reruns=1, reruns_delay=5)
@pytest.mark.parametrize(
    'config, count, app_path, target, port',
    [
        pytest.param(
            'cli|ssed',
            2,
            f'{os.path.join(os.path.dirname(__file__), "ot_cli")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_sleepy_device/light_sleep")}',
            'esp32h2|esp32c6',
            f'{ESPPORT1}|{ESPPORT3}',
            id='h2-c6',
        ),
        pytest.param(
            'cli|ssed',
            2,
            f'{os.path.join(os.path.dirname(__file__), "ot_cli")}'
            f'|{os.path.join(os.path.dirname(__file__), "ot_sleepy_device/light_sleep")}',
            'esp32c6|esp32h2',
            f'{ESPPORT3}|{ESPPORT1}',
            id='c6-h2',
        ),
    ],
    indirect=True,
)
def test_ot_ssed_device(dut: tuple[IdfDut, IdfDut]) -> None:
    leader = dut[0]
    ssed_device = dut[1]
    try:
        ocf.clean_buffer(ssed_device)
        ssed_device.serial.hard_reset()
        # CI device must have external XTAL to run SSED case, we will check this here first
        ssed_device.expect('32k XTAL in use', timeout=20)
        ocf.init_thread(leader)
        time.sleep(3)
        leader_para = ocf.thread_parameter('leader', '', '12', '7766554433221100', False)
        ocf.joinThreadNetwork(leader, leader_para)
        ocf.wait(leader, 5)
        ocf.execute_command(leader, 'networkkey')
        dataset = ocf.getDataset(leader)
        ocf.execute_command(ssed_device, 'dataset set active ' + dataset)
        ssed_device.expect('Done', timeout=5)
        ocf.execute_command(ssed_device, 'mode -')
        ssed_device.expect('Done', timeout=5)
        ocf.execute_command(ssed_device, 'csl period 3000000')
        ssed_device.expect('Done', timeout=5)
        ocf.execute_command(ssed_device, 'csl channel 12')
        ssed_device.expect('Done', timeout=5)
        ocf.execute_command(ssed_device, 'ifconfig up')
        ssed_device.expect('Done', timeout=5)
        ocf.execute_command(ssed_device, 'thread start')
        ssed_device.expect(r'(.+)detached -> child', timeout=20)
        # add a sleep to wait ssed ready
        time.sleep(3)
        ssed_device.expect('PMU_SLEEP_PD_TOP: True', timeout=5)
        ssed_device.expect('PMU_SLEEP_PD_MODEM: True', timeout=5)

        ocf.execute_command(leader, 'child table')
        pattern = r'\|\s+\d+\s+\|\s+(0x\w{4})\s+\|.*\|\s+(\w{16})\s+\|'
        result = leader.expect(pattern)

        rloc16_decode_from_leader = result[1].decode()[2:]
        cli_rloc_addr = ':'.join(ocf.get_rloc_addr(leader).split(':')[:-1])
        ssed_address = f'{cli_rloc_addr}:{rloc16_decode_from_leader}'

        ocf.ping_and_check(dut=leader, target=ssed_address, tx_total=10, timeout=6)
        time.sleep(random.randint(5, 20))
        ocf.ping_and_check(dut=leader, target=ssed_address, tx_total=10, timeout=6)
    finally:
        ocf.execute_command(leader, 'factoryreset')
        ocf.hardreset_dut(ssed_device)
        time.sleep(3)
