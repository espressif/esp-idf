# SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import ipaddress
import subprocess
import threading
import time
from typing import Dict, Union

import pytest
from pytest_embedded import Dut
from scapy import layers
from scapy.all import ICMP, IP, TCP, UDP, AsyncSniffer

udp_port = 1234
tcp_port = 4321


def run_cmd(command: str, secure: bool=False) -> str:
    if secure is False:
        print(f'Running: {command}')

    cmd = command.strip().split(' ')

    # Run the command
    proc = subprocess.Popen(cmd, stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    # Send su password if secured
    if secure is True:
        proc.communicate(input=b'Esp@32')

    # Get the output
    stdout, stderr = proc.communicate()

    # Print the output
    if secure is False:
        if len(stdout.decode('utf-8')) > 0:
            print('Output: ', stdout.decode('utf-8'))

    if len(stderr.decode('utf-8')) > 0:
        print('Error: ', stderr.decode('utf-8'))

    return stdout.decode('utf-8')


def run_cmd_sec(command: str) -> str:
    print(f'Running secured: {command}')
    return run_cmd(f'sudo -S -k {command}', secure=True)


def clear_network(config: dict) -> None:
    # delete route
    for each_cmd in config['delete_route_cmd_l']:
        run_cmd_sec(each_cmd)

    # destroy Vlan interfaces
    for each_cmd in config['vlan_destroy_cmd_l']:
        run_cmd_sec(each_cmd)


def setup_network(config: dict) -> None:
    # Clear network before setting it up
    clear_network(config)

    # Create Vlan interfaces
    for each_cmd in config['vlan_create_cmd_l']:
        run_cmd_sec(each_cmd)

    # set route
    for each_cmd in config['set_route_cmd_l']:
        run_cmd_sec(each_cmd)


def create_config(dut: Dut) -> dict:
    pc_iface = dut.app.sdkconfig.get('EXAMPLE_VLAN_PYTEST_PC_IFACE')
    vlanClient_conf = {'id': str(dut.app.sdkconfig.get('EXAMPLE_ETHERNET_VLAN_ID')),
                       'name': 'vlanClient',
                       'ip': dut.app.sdkconfig.get('EXAMPLE_VLAN_STATIC_ADDR_DEF_GW')}
    vlanServer_conf = {'id': str(dut.app.sdkconfig.get('EXAMPLE_EXTRA_ETHERNET_VLAN_ID')),
                       'name': 'vlanServer',
                       'ip': dut.app.sdkconfig.get('EXAMPLE_EXTRA_VLAN_STATIC_ADDR_DEF_GW')}
    esp_vlanClient_ip = dut.app.sdkconfig.get('EXAMPLE_VLAN_STATIC_IPV4_ADDR')
    esp_vlanServer_ip = dut.app.sdkconfig.get('EXAMPLE_EXTRA_VLAN_STATIC_IPV4_ADDR')

    subnet_mask = ipaddress.IPv4Address('255.255.255.0')
    tmp_ip = ipaddress.IPv4Address(vlanServer_conf['ip'])
    vlanServer_net_addr = ipaddress.IPv4Address(int(tmp_ip) & int(subnet_mask))

    config: Dict[str, Union[str, dict, dict, str, str, list, list, list, list]] = {
        # Basic Configurations
        'pc_iface': pc_iface,

        'vlanClient': vlanClient_conf,
        'vlanServer': vlanServer_conf,

        'esp_vlanClient_ip': esp_vlanClient_ip,
        'esp_vlanServer_ip': esp_vlanServer_ip,

        'vlan_create_cmd_l': [f'ip netns add ns_vlanClient',
                              f"ip link add link {pc_iface} name {vlanClient_conf['name']} type vlan id {vlanClient_conf['id']}",
                              f"ip link set {vlanClient_conf['name']} netns ns_vlanClient",
                              f"ip netns exec ns_vlanClient ip addr add {vlanClient_conf['ip']}/255.255.255.0 dev {vlanClient_conf['name']}",
                              f"ip netns exec ns_vlanClient ip link set dev {vlanClient_conf['name']} up",
                              f"ip link add link {pc_iface} name {vlanServer_conf['name']} type vlan id {vlanServer_conf['id']}",
                              f"ip addr add {vlanServer_conf['ip']}/255.255.255.0 dev {vlanServer_conf['name']}",
                              f"ip link set dev {vlanServer_conf['name']} up"],

        'vlan_destroy_cmd_l': [f"ip netns exec ns_vlanClient ip link set dev {vlanClient_conf['name']} down",
                               f"ip netns exec ns_vlanClient ip link delete {vlanClient_conf['name']}",
                               f"ip link set dev {vlanServer_conf['name']} down",
                               f"ip link delete {vlanServer_conf['name']}",
                               f'ip netns delete ns_vlanClient'],

        'set_route_cmd_l': [f'ip netns exec ns_vlanClient ip route add {vlanServer_net_addr}/24 via {esp_vlanClient_ip}'],

        'delete_route_cmd_l': [f'ip netns exec ns_vlanClient ip route delete {vlanServer_net_addr}/24 via {esp_vlanClient_ip}'],
    }

    return config


# Ping Test
def ping_test(config: dict) -> None:

    setup_network(config)

    capture = AsyncSniffer(iface=config['vlanServer']['name'], filter='icmp', count=10)

    # Start sniffing
    capture.start()

    time.sleep(1)
    # Run network test commands here
    print(run_cmd_sec(f"ip netns exec ns_vlanClient ping -I {config['vlanClient']['ip']} {config['vlanServer']['ip']} -c 10"))

    # Stop sniffing
    capture.join(timeout=20)
    vlanServer_pkt_list = capture.results

    clear_network(config)

    # Test Validation
    vlanServer_forward_flag = False
    vlanServer_return_flag = False

    if vlanServer_pkt_list is None:
        print('Failure: No packets captured')
        assert False

    print(f"Captured: {len(vlanServer_pkt_list)} packets on interface {config['vlanServer']['name']}")
    for pkt in vlanServer_pkt_list:
        print('Summary: ', pkt.summary())
        if pkt[ICMP].type == 8 and pkt[IP].src == config['esp_vlanServer_ip'] and pkt[IP].dst == config['vlanServer']['ip']:
            vlanServer_forward_flag = True
        if pkt[ICMP].type == 0 and pkt[IP].src == config['vlanServer']['ip'] and pkt[IP].dst == config['esp_vlanServer_ip']:
            vlanServer_return_flag = True

    assert vlanServer_forward_flag and vlanServer_return_flag


# UDP Test
def udp_server(serverip: str, port: int) -> None:
    print(f'UDP server listening on IP: {serverip} port: {port}')
    run_cmd(f'timeout 10s iperf3 -s -p {port}')


def udp_client(serverip: str, port: int) -> None:
    time.sleep(1)
    print(run_cmd_sec(f'timeout 10s ip netns exec ns_vlanClient iperf3 -c {serverip} -u -p {port} --bidir -k 20'))


def udp_server_client_comm(config: dict, port: int) -> None:
    server_thread = threading.Thread(target=udp_server, args=(config['vlanServer']['ip'], port,))
    client_thread = threading.Thread(target=udp_client, args=(config['vlanServer']['ip'], port))

    server_thread.start()
    client_thread.start()

    server_thread.join()
    client_thread.join()
    print('UDP Test Done')


def udp_lfilter(packet: layers.l2.Ether) -> layers.l2.Ether:
    return UDP in packet and (packet[UDP].dport == udp_port or packet[UDP].sport == udp_port)


def udp_test(config: dict) -> None:

    setup_network(config)

    capture = AsyncSniffer(iface=config['vlanServer']['name'],
                           filter='udp',
                           lfilter=udp_lfilter,
                           count=10)

    # Start sniffing
    capture.start()

    time.sleep(1)

    # Run network test commands here
    udp_server_client_comm(config, udp_port)

    # Stop sniffing
    capture.join(timeout=20)
    vlanServer_pkt_list = capture.results

    clear_network(config)

    # Test Validation
    vlanServer_forward_flag = False
    vlanServer_return_flag = False

    if vlanServer_pkt_list is None:
        print('Failure: No packets captured')
        assert False

    print(f"Captured: {len(vlanServer_pkt_list)} packets on interface {config['vlanServer']['name']}")
    for pkt in vlanServer_pkt_list:
        print('Summary: ', pkt.summary())
        if UDP in pkt:
            if pkt[UDP].dport == udp_port and pkt[IP].src == config['esp_vlanServer_ip'] and pkt[IP].dst == config['vlanServer']['ip']:
                vlanServer_forward_flag = True
            if pkt[UDP].sport == udp_port and pkt[IP].src == config['vlanServer']['ip'] and pkt[IP].dst == config['esp_vlanServer_ip']:
                vlanServer_return_flag = True

    assert vlanServer_forward_flag and vlanServer_return_flag


# TCP Test
def tcp_server(serverip: str, port: int) -> None:
    print(f'TCP server listening on IP: {serverip} port: {port}')
    run_cmd(f'timeout 10s iperf3 -s -p {port}')


def tcp_client(serverip: str, port: int) -> None:
    time.sleep(1)
    print(run_cmd_sec(f'timeout 10s ip netns exec ns_vlanClient iperf3 -c {serverip} -p {port} --bidir -k 20'))


def tcp_server_client_comm(config: dict, port: int) -> None:
    server_thread = threading.Thread(target=tcp_server, args=(config['vlanServer']['ip'], port,))
    client_thread = threading.Thread(target=tcp_client, args=(config['vlanServer']['ip'], port))

    server_thread.start()
    client_thread.start()

    server_thread.join()
    client_thread.join()
    print('TCP Test Done')


def tcp_lfilter(packet: layers.l2.Ether) -> layers.l2.Ether:
    return TCP in packet and (packet[TCP].dport == tcp_port or packet[TCP].sport == tcp_port)


def tcp_test(config: dict) -> None:

    setup_network(config)

    capture = AsyncSniffer(iface=config['vlanServer']['name'],
                           filter='tcp',
                           lfilter=tcp_lfilter,
                           count=10)

    # Start sniffing
    capture.start()

    time.sleep(1)

    # Run network test commands here
    tcp_server_client_comm(config, tcp_port)

    # Stop sniffing
    capture.join(timeout=20)
    vlanServer_pkt_list = capture.results

    clear_network(config)

    # Test Validation
    vlanServer_forward_flag = False
    vlanServer_return_flag = False

    if vlanServer_pkt_list is None:
        print('Failure: No packets captured')
        assert False

    print(f"Captured: {len(vlanServer_pkt_list)} packets on interface {config['vlanServer']['name']}")
    for pkt in vlanServer_pkt_list:
        print('Summary: ', pkt.summary())
        if TCP in pkt:
            if pkt[TCP].dport == tcp_port and pkt[IP].src == config['esp_vlanServer_ip'] and pkt[IP].dst == config['vlanServer']['ip']:
                vlanServer_forward_flag = True
            if pkt[TCP].sport == tcp_port and pkt[IP].src == config['vlanServer']['ip'] and pkt[IP].dst == config['esp_vlanServer_ip']:
                vlanServer_return_flag = True

    assert vlanServer_forward_flag and vlanServer_return_flag


@pytest.mark.esp32
@pytest.mark.ethernet_vlan
def test_vlan_napt_pingtest(dut: Dut) -> None:
    dut.expect('main_task: Returned from app_main()')
    test_conf = create_config(dut)
    ping_test(test_conf)


@pytest.mark.esp32
@pytest.mark.ethernet_vlan
def test_vlan_napt_udptest(dut: Dut) -> None:
    dut.expect('main_task: Returned from app_main()')
    test_conf = create_config(dut)
    udp_test(test_conf)


@pytest.mark.esp32
@pytest.mark.ethernet_vlan
def test_vlan_napt_tcptest(dut: Dut) -> None:
    dut.expect('main_task: Returned from app_main()')
    test_conf = create_config(dut)
    tcp_test(test_conf)
