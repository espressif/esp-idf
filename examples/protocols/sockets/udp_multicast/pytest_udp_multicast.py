# SPDX-FileCopyrightText: 2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import ipaddress
import logging
import os
import socket
import struct
from typing import Any

import netifaces
import pytest
from common_test_methods import get_host_ip4_by_dest_ip
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

PORT = 3333
IPV6_REGEX = (
    r'(?<![0-9a-fA-F:])('
    r'(([0-9a-fA-F]{1,4}:){7}[0-9a-fA-F]{1,4}|'
    r'(([0-9a-fA-F]{1,4}:){1,7}:)|'
    r'(([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4})|'
    r'(([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2})|'
    r'(([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3})|'
    r'(([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4})|'
    r'(([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5})|'
    r'([0-9a-fA-F]{1,4}:)((:[0-9a-fA-F]{1,4}){1,6})|'
    r'(:)((:[0-9a-fA-F]{1,4}){1,7}|:)'
    r'))(?![0-9a-fA-F:])[^\w]'
)


def find_target_if(my_if: str = '') -> str:
    # try to determine which interface to use
    netifs = os.listdir('/sys/class/net/')
    # order matters - ETH NIC with the highest number is connected to DUT on CI runner
    netifs.sort(reverse=True)
    logging.info('detected interfaces: %s', str(netifs))

    for netif in netifs:
        # if no interface defined, try to find it automatically
        if my_if == '':
            if netif.find('eth') == 0 or netif.find('enp') == 0 or netif.find('eno') == 0:
                return netif
        else:
            if netif.find(my_if) == 0:
                return my_if

    raise RuntimeError('network interface not found')


def get_link_local_for_interface(if_name: str) -> Any:
    addrs = netifaces.ifaddresses(if_name)
    if netifaces.AF_INET6 in addrs:
        for addr in addrs[netifaces.AF_INET6]:
            if addr['addr'].startswith('fe80:'):
                return addr['addr'].split('%')[0]
    raise RuntimeError(f'No link-local address found on interface {if_name}')


def test_examples_udp_multicast_proto(dut: Dut, ip_version: str = 'ipv4', nic: str = '') -> None:
    """
    Test UDP multicast functionality for both IPv4 and IPv6.

    Args:
        dut: Device under test
        ip_version: 'ipv4', ipv4_mapped or 'ipv6'
    """
    # Create multicast socket based on IP version
    if ip_version == 'ipv4' or ip_version == 'ipv4_mapped':
        # IPv4 socket
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_TTL, 32)

        # Get ESP32's IPv4 address and multicast address
        ip_addr = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
        multicast_addr = dut.expect(r'Configured IPV4 Multicast address (\d+\.\d+\.\d+\.\d+)[^\d]')[1].decode()

        # Get host interface IP to bind to
        host_ip = get_host_ip4_by_dest_ip(ip_addr)

        # Bind to all interfaces
        sock.bind(('0.0.0.0', PORT))

        # Set socket options for multicast
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)

        # Specify the outgoing interface for multicast
        sock.setsockopt(socket.IPPROTO_IP, socket.IP_MULTICAST_IF, socket.inet_aton(host_ip))

        # Join multicast group
        sock.setsockopt(
            socket.IPPROTO_IP, socket.IP_ADD_MEMBERSHIP, socket.inet_aton(multicast_addr) + socket.inet_aton(host_ip)
        )

    elif ip_version == 'ipv6':
        net_if = find_target_if(nic)

        # IPv6 socket
        sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM)
        sock.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_MULTICAST_HOPS, 32)

        # Get ESP32's IPv6 address and multicast address
        ip_addr = dut.expect(rf'IPv6 address: {IPV6_REGEX}', timeout=30)[1].decode()
        ip_addr = str(ipaddress.IPv6Address(ip_addr))
        multicast_addr = dut.expect(rf'Configured IPV6 Multicast address {IPV6_REGEX}')[1].decode()

        # Bind to all interfaces
        sock.bind(('::', PORT))

        # Set socket options for multicast
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEPORT, 1)

        # Specify the outgoing interface for multicast
        sock.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_MULTICAST_IF, socket.if_nametoindex(net_if))

        # Get host interface IPv6
        host_ip = get_link_local_for_interface(net_if)

        # Join multicast group
        mreq = struct.pack('16si', socket.inet_pton(socket.AF_INET6, multicast_addr), socket.if_nametoindex(net_if))
        sock.setsockopt(socket.IPPROTO_IPV6, socket.IPV6_JOIN_GROUP, mreq)
    else:
        raise RuntimeError(f'Invalid IP version: {ip_version}')

    # Set socket timeout
    sock.settimeout(5.0)

    if ip_version == 'ipv4_mapped':
        dut.expect(r'Sending to IPV6 \(V4 mapped\) multicast address', timeout=5)
    else:
        dut.expect(f'Sending to {ip_version.upper()} multicast address', timeout=5)

    try:
        data, recv_addr = sock.recvfrom(1024)
        logging.info(f'Received {len(data)} bytes from {recv_addr}')
    except socket.timeout:
        raise RuntimeError(f'Timeout waiting for {ip_version} multicast message from ESP32')

    # Check if received from expected source
    if recv_addr[0] != ip_addr or recv_addr[1] != PORT:
        raise RuntimeError(f'Received {ip_version} multicast message from unexpected source')

    # Send multicast message
    message = '!!! Multicast test message from host !!!'.encode()
    logging.info(f'Sending {ip_version} multicast message to {multicast_addr}:{PORT}')
    sock.sendto(message, (multicast_addr, PORT))
    if ip_version == 'ipv4_mapped':
        dut.expect(f'received {len(message)} bytes from ::FFFF:{host_ip}', timeout=1)
    else:
        dut.expect(f'received {len(message)} bytes from {host_ip.upper()}', timeout=1)

    sock.close()


@pytest.mark.eth_ip101
@idf_parametrize(
    'target',
    ['esp32', 'esp32p4'],
    indirect=['target'],
)
def test_examples_udp_multicast(dut: Dut) -> None:
    test_examples_udp_multicast_proto(dut, 'ipv4_mapped')
    dut.serial.hard_reset()
    test_examples_udp_multicast_proto(dut, 'ipv6')
