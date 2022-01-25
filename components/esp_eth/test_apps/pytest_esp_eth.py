# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0

import contextlib
import logging
import os
import socket
from typing import Iterator

import pytest
from pytest_embedded import Dut


@contextlib.contextmanager
def configure_eth_if() -> Iterator[socket.socket]:
    # try to determine which interface to use
    netifs = os.listdir('/sys/class/net/')
    logging.info('detected interfaces: %s', str(netifs))

    target_if = ''
    for netif in netifs:
        if netif.find('eth') == 0 or netif.find('enp') == 0 or netif.find('eno') == 0:
            target_if = netif
            break
    if target_if == '':
        raise Exception('no network interface found')
    logging.info('Use %s for testing', target_if)

    so = socket.socket(socket.AF_PACKET, socket.SOCK_RAW, 0x2222)
    so.bind((target_if, 0))

    try:
        yield so
    finally:
        so.close()


def send_eth_packet(mac: bytes) -> None:
    with configure_eth_if() as so:
        so.settimeout(10)
        pkt = bytearray()
        pkt += mac  # dest
        pkt += so.getsockname()[4]  # src
        pkt += bytes.fromhex('2222')  # proto
        pkt += bytes(1010)  # padding to 1024
        for i in range(128, 1024):
            pkt[i] = i & 0xff
        try:
            so.send(pkt)
        except Exception as e:
            raise e


def actual_test(dut: Dut) -> None:
    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('\n')

    dut.expect_exact('Enter test for running.')
    dut.write('"start_and_stop"')
    dut.expect_unity_test_output()

    dut.expect_exact("Enter next test, or 'enter' to see menu")
    dut.write('"get_set_mac"')
    dut.expect_unity_test_output()

    dut.expect_exact("Enter next test, or 'enter' to see menu")
    with configure_eth_if() as so:
        so.settimeout(30)
        dut.write('"ethernet_broadcast_transmit"')
        pkt = so.recv(1024)
        for i in range(128, 1024):
            if pkt[i] != i & 0xff:
                raise Exception('Packet content mismatch')
    dut.expect_unity_test_output()

    dut.expect_exact("Enter next test, or 'enter' to see menu")
    dut.write('"recv_pkt"')
    res = dut.expect(
        r'([\s\S]*)'
        r'DUT MAC: ([0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2})'
    )
    send_eth_packet(bytes.fromhex('ffffffffffff'))  # broadcast frame # pylint: disable=no-value-for-parameter
    send_eth_packet(bytes.fromhex('010000000000'))  # multicast frame # pylint: disable=no-value-for-parameter
    send_eth_packet(bytes.fromhex(res.group(2).decode('utf-8').replace(':', '')))  # unicast fram  # pylint: disable=no-value-for-parameter, line-too-long # noqa
    dut.expect_unity_test_output(extra_before=res.group(1))


@pytest.mark.esp32
@pytest.mark.ip101
@pytest.mark.parametrize('config', [
    'ip101',
], indirect=True)
@pytest.mark.flaky(reruns=3, reruns_delay=5)
def test_esp_eth_ip101(dut: Dut) -> None:
    actual_test(dut)


@pytest.mark.esp32
@pytest.mark.lan8720
@pytest.mark.parametrize('config', [
    'lan8720',
], indirect=True)
@pytest.mark.flaky(reruns=3, reruns_delay=5)
def test_esp_eth_lan8720(dut: Dut) -> None:
    actual_test(dut)
