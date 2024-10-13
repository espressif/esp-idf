# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import contextlib
import logging
import os
import socket
import sys
from typing import Iterator

import pytest
from pytest_embedded import Dut
from scapy.all import Ether
from scapy.all import raw

ETH_TYPE_1 = 0x2220
ETH_TYPE_2 = 0x2221
ETH_TYPE_3 = 0x2223


@contextlib.contextmanager
def configure_eth_if(eth_type: int, target_if: str='') -> Iterator[socket.socket]:
    if target_if == '':
        # try to determine which interface to use
        netifs = os.listdir('/sys/class/net/')
        # order matters - ETH NIC with the highest number is connected to DUT on CI runner
        netifs.sort(reverse=True)
        logging.info('detected interfaces: %s', str(netifs))
        for netif in netifs:
            if netif.find('eth') == 0 or netif.find('enx') == 0 or netif.find('enp') == 0 or netif.find('eno') == 0:
                target_if = netif
                break
        if target_if == '':
            raise Exception('no network interface found')
    logging.info('Use %s for testing', target_if)

    so = socket.socket(socket.AF_PACKET, socket.SOCK_RAW, socket.htons(eth_type))
    so.bind((target_if, 0))

    try:
        yield so
    finally:
        so.close()


def send_recv_eth_frame(payload_str: str, eth_type: int, dest_mac: str, eth_if: str='') -> str:
    with configure_eth_if(eth_type, eth_if) as so:
        so.settimeout(10)
        eth_frame = Ether(dst=dest_mac, src=so.getsockname()[4], type=eth_type) / raw(payload_str.encode())
        try:
            so.send(raw(eth_frame))
            logging.info('Sent %d bytes to %s', len(eth_frame), dest_mac)
            logging.info('Sent msg: "%s"', payload_str)

            eth_frame_repl = Ether(so.recv(128))
            if eth_frame_repl.type == eth_type:
                logging.info('Received %d bytes echoed from %s', len(eth_frame_repl), eth_frame_repl.src)
                logging.info('Echoed msg: "%s"', eth_frame_repl.load.decode())
        except Exception as e:
            raise e
    # return echoed message and remove possible null characters which might have been appended since
    # minimal size of Ethernet frame to be transmitted physical layer is 60B (not including CRC)
    return str(eth_frame_repl.load.decode().rstrip('\x00'))


def recv_eth_frame(eth_type: int, eth_if: str='') -> str:
    with configure_eth_if(eth_type, eth_if) as so:
        so.settimeout(10)
        try:
            eth_frame = Ether(so.recv(128))
            if eth_frame.type == eth_type:
                logging.info('Received %d bytes from %s', len(eth_frame), eth_frame.src)
                logging.info('Received msg: "%s"', eth_frame.load.decode())
        except Exception as e:
            raise e
    return str(eth_frame.load.decode().rstrip('\x00'))


def actual_test(dut: Dut) -> None:
    # Get DUT's MAC address
    res = dut.expect(
        r'([\s\S]*)'
        r'Ethernet HW Addr ([0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2})'
    )
    dut_mac = res.group(2)

    # Receive "ESP32 Hello frame"
    recv_eth_frame(ETH_TYPE_3)

    # Sent a message and receive its echo
    message = 'ESP32 test message with EthType ' + hex(ETH_TYPE_1)
    echoed = send_recv_eth_frame(message, ETH_TYPE_1, dut_mac)
    if echoed == message:
        logging.info('PASS')
    else:
        raise Exception('Echoed message does not match!')
    message = 'ESP32 test message with EthType ' + hex(ETH_TYPE_2)
    echoed = send_recv_eth_frame(message, ETH_TYPE_2, dut_mac)
    if echoed == message:
        logging.info('PASS')
    else:
        raise Exception('Echoed message does not match!')


@pytest.mark.esp32  # internally tested using ESP32 with IP101 but may support all targets with SPI Ethernet
@pytest.mark.eth_ip101
@pytest.mark.flaky(reruns=3, reruns_delay=5)
def test_esp_netif_l2tap_example(dut: Dut) -> None:
    actual_test(dut)


if __name__ == '__main__':
    logging.basicConfig(format='%(asctime)s %(message)s', level=logging.INFO)
    message_1 = 'ESP32 test message with EthType ' + hex(ETH_TYPE_1)
    message_2 = 'ESP32 test message with EthType ' + hex(ETH_TYPE_2)
    # Usage: pytest_example_l2tap_echo.py [<dest_mac_address>] [<host_eth_interface>]
    if sys.argv[2:]:    # if two arguments provided:
        send_recv_eth_frame(message_1, ETH_TYPE_1, sys.argv[1], sys.argv[2])
        send_recv_eth_frame(message_2, ETH_TYPE_2, sys.argv[1], sys.argv[2])
    elif sys.argv[1:]:    # if one argument provided:
        send_recv_eth_frame(message_1, ETH_TYPE_1, sys.argv[1])
        send_recv_eth_frame(message_2, ETH_TYPE_2, sys.argv[1])
    else:    # if no argument provided:
        send_recv_eth_frame(message_1, ETH_TYPE_1, 'ff:ff:ff:ff:ff:ff')
        send_recv_eth_frame(message_2, ETH_TYPE_2, 'ff:ff:ff:ff:ff:ff')
