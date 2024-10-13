# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: CC0-1.0
import contextlib
import logging
import os
import socket
from multiprocessing import connection
from multiprocessing import Pipe
from multiprocessing import Process
from typing import Iterator

import pytest
from pytest_embedded_idf import IdfDut
from scapy.all import Ether
from scapy.all import raw

ETH_TYPE = 0x3300


class EthTestIntf(object):
    def __init__(self, eth_type: int, my_if: str = ''):
        self.target_if = ''
        self.eth_type = eth_type
        self.find_target_if(my_if)

    def find_target_if(self, my_if: str = '') -> None:
        # try to determine which interface to use
        netifs = os.listdir('/sys/class/net/')
        # order matters - ETH NIC with the highest number is connected to DUT on CI runner
        netifs.sort(reverse=True)
        logging.info('detected interfaces: %s', str(netifs))

        for netif in netifs:
            # if no interface defined, try to find it automatically
            if my_if == '':
                if netif.find('eth') == 0 or netif.find('enp') == 0 or netif.find('eno') == 0:
                    self.target_if = netif
                    break
            else:
                if netif.find(my_if) == 0:
                    self.target_if = my_if
                    break
        if self.target_if == '':
            raise RuntimeError('network interface not found')
        logging.info('Use %s for testing', self.target_if)

    @contextlib.contextmanager
    def configure_eth_if(self, eth_type:int=0) -> Iterator[socket.socket]:
        if eth_type == 0:
            eth_type = self.eth_type
        so = socket.socket(socket.AF_PACKET, socket.SOCK_RAW, socket.htons(eth_type))
        so.bind((self.target_if, 0))
        try:
            yield so
        finally:
            so.close()

    def send_eth_packet(self, mac: str) -> None:
        with self.configure_eth_if() as so:
            so.settimeout(10)
            payload = bytearray(1010)
            for i, _ in enumerate(payload):
                payload[i] = i & 0xff
            eth_frame = Ether(dst=mac, src=so.getsockname()[4], type=self.eth_type) / raw(payload)
            try:
                so.send(raw(eth_frame))
            except Exception as e:
                raise e

    def recv_resp_poke(self, mac:str, i:int=0) -> None:
        eth_type_ctrl = self.eth_type + 1
        with self.configure_eth_if(eth_type_ctrl) as so:
            so.settimeout(30)
            for _ in range(10):
                try:
                    eth_frame = Ether(so.recv(60))
                except Exception as e:
                    raise e
                if mac == eth_frame.src and eth_frame.load[0] == 0xfa:
                    if eth_frame.load[1] != i:
                        raise RuntimeError('Missed Poke Packet')
                    logging.info('Poke Packet received...')
                    eth_frame.dst = eth_frame.src
                    eth_frame.src = so.getsockname()[4]
                    eth_frame.load = bytes.fromhex('fb')    # POKE_RESP code
                    so.send(raw(eth_frame))
                    break
                else:
                    logging.warning('Unexpected Control packet')
                    logging.warning('Expected Ctrl command: 0xfa, actual: 0x%x', eth_frame.load[0])
                    logging.warning('Source MAC %s', eth_frame.src)
            else:
                raise RuntimeError('No Poke Packet!')

    def traffic_gen(self, mac: str, pipe_rcv:connection.Connection) -> None:
        with self.configure_eth_if() as so:
            payload = bytes.fromhex('ff')    # DUMMY_TRAFFIC code
            payload += bytes(1485)
            eth_frame = Ether(dst=mac, src=so.getsockname()[4], type=self.eth_type) / raw(payload)
            try:
                while pipe_rcv.poll() is not True:
                    so.send(raw(eth_frame))
            except Exception as e:
                raise e

    def eth_loopback(self, mac: str, pipe_rcv:connection.Connection) -> None:
        with self.configure_eth_if(self.eth_type) as so:
            so.settimeout(30)
            try:
                while pipe_rcv.poll() is not True:
                    try:
                        eth_frame = Ether(so.recv(1522))
                    except Exception as e:
                        raise e
                    if mac == eth_frame.src:
                        eth_frame.dst = eth_frame.src
                        eth_frame.src = so.getsockname()[4]
                        so.send(raw(eth_frame))
                    else:
                        logging.warning('Received frame from unexpected source')
                        logging.warning('Source MAC %s', eth_frame.src)
            except Exception as e:
                raise e


def ethernet_test(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='ethernet', timeout=980)


def ethernet_int_emac_test(dut: IdfDut) -> None:
    dut.run_all_single_board_cases(group='esp_emac', timeout=120)


def ethernet_l2_test(dut: IdfDut) -> None:
    target_if = EthTestIntf(ETH_TYPE)

    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('\n')
    dut.expect_exact('Enter test for running.')

    with target_if.configure_eth_if() as so:
        so.settimeout(30)
        dut.write('"ethernet broadcast transmit"')
        res = dut.expect(
            r'DUT MAC: ([0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2})'
        )

        # wait for POKE msg to be sure the switch already started forwarding the port's traffic
        # (there might be slight delay due to the RSTP execution)
        dut_mac = res.group(1).decode('utf-8')
        target_if.recv_resp_poke(mac=dut_mac)

        for _ in range(10):
            eth_frame = Ether(so.recv(1024))
            if dut_mac == eth_frame.src:
                break
        else:
            raise RuntimeError('No broadcast received from expected DUT MAC addr')

        for i in range(0, 1010):
            if eth_frame.load[i] != i & 0xff:
                raise RuntimeError('Packet content mismatch')
    dut.expect_unity_test_output()

    dut.expect_exact("Enter next test, or 'enter' to see menu")
    dut.write('"ethernet recv_pkt"')
    res = dut.expect(
        r'DUT MAC: ([0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2})'
    )
    dut_mac = res.group(1).decode('utf-8')
    # wait for POKE msg to be sure the switch already started forwarding the port's traffic
    # (there might be slight delay due to the RSTP execution)
    target_if.recv_resp_poke(mac=dut_mac)
    target_if.send_eth_packet('ff:ff:ff:ff:ff:ff')  # broadcast frame
    target_if.send_eth_packet('01:00:5e:00:00:00')  # IPv4 multicast frame (some SPI Eth modules filter multicast other than IP)
    target_if.send_eth_packet(mac=dut_mac)  # unicast frame
    dut.expect_unity_test_output(extra_before=res.group(1))

    dut.expect_exact("Enter next test, or 'enter' to see menu")
    dut.write('"ethernet start/stop stress test under heavy traffic"')
    res = dut.expect(
        r'DUT MAC: ([0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2})'
    )
    dut_mac = res.group(1).decode('utf-8')
    # Start/stop under heavy Tx traffic
    for tx_i in range(10):
        target_if.recv_resp_poke(dut_mac, tx_i)
        dut.expect_exact('Ethernet Stopped')

    for rx_i in range(10):
        target_if.recv_resp_poke(dut_mac, rx_i)
        # Start/stop under heavy Rx traffic
        pipe_rcv, pipe_send = Pipe(False)
        tx_proc = Process(target=target_if.traffic_gen, args=(dut_mac, pipe_rcv, ))
        tx_proc.start()
        dut.expect_exact('Ethernet Stopped')
        pipe_send.send(0)  # just send some dummy data
        tx_proc.join(5)
        if tx_proc.exitcode is None:
            tx_proc.terminate()

    dut.expect_unity_test_output(extra_before=res.group(1))


def ethernet_heap_alloc_test(dut: IdfDut) -> None:
    target_if = EthTestIntf(ETH_TYPE)

    dut.expect_exact('Press ENTER to see the list of tests')
    dut.write('\n')
    dut.expect_exact('Enter test for running.')
    dut.write('"heap utilization"')
    res = dut.expect(r'DUT PHY: (\w+)')
    dut_phy = res.group(1).decode('utf-8')
    # W5500 does not support internal loopback, we need to loopback for it
    if 'W5500' in dut_phy:
        logging.info('Starting loopback server...')
        res = dut.expect(
            r'DUT MAC: ([0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2})'
        )
        dut_mac = res.group(1).decode('utf-8')
        pipe_rcv, pipe_send = Pipe(False)
        loopback_proc = Process(target=target_if.eth_loopback, args=(dut_mac, pipe_rcv, ))
        loopback_proc.start()

        target_if.recv_resp_poke(mac=dut_mac)
        dut.expect_exact('Ethernet Stopped')
        pipe_send.send(0)  # just send some dummy data
        loopback_proc.join(5)
        if loopback_proc.exitcode is None:
            loopback_proc.terminate()

    dut.expect_unity_test_output()


# ----------- IP101 -----------
@pytest.mark.esp32
@pytest.mark.ethernet
@pytest.mark.parametrize('config', [
    'default_ip101',
    'release_ip101',
    'single_core_ip101'
], indirect=True)
@pytest.mark.flaky(reruns=3, reruns_delay=5)
def test_esp_ethernet(dut: IdfDut) -> None:
    ethernet_test(dut)


@pytest.mark.esp32
@pytest.mark.ethernet
@pytest.mark.parametrize('config', [
    'default_ip101',
], indirect=True)
def test_esp_emac(dut: IdfDut) -> None:
    ethernet_int_emac_test(dut)
    dut.serial.hard_reset()
    ethernet_heap_alloc_test(dut)


@pytest.mark.esp32
@pytest.mark.eth_ip101
@pytest.mark.parametrize('config', [
    'default_ip101',
], indirect=True)
def test_esp_eth_ip101(dut: IdfDut) -> None:
    ethernet_l2_test(dut)


# ----------- LAN8720 -----------
@pytest.mark.esp32
@pytest.mark.eth_lan8720
@pytest.mark.parametrize('config', [
    'default_lan8720',
], indirect=True)
def test_esp_eth_lan8720(dut: IdfDut) -> None:
    ethernet_test(dut)
    dut.serial.hard_reset()
    ethernet_l2_test(dut)


# ----------- RTL8201 -----------
@pytest.mark.esp32
@pytest.mark.eth_rtl8201
@pytest.mark.parametrize('config', [
    'default_rtl8201',
], indirect=True)
def test_esp_eth_rtl8201(dut: IdfDut) -> None:
    ethernet_test(dut)
    dut.serial.hard_reset()
    ethernet_l2_test(dut)


# ----------- KSZ8041 -----------
@pytest.mark.esp32
@pytest.mark.eth_ksz8041
@pytest.mark.parametrize('config', [
    'default_ksz8041',
], indirect=True)
def test_esp_eth_ksz8041(dut: IdfDut) -> None:
    ethernet_test(dut)
    dut.serial.hard_reset()
    ethernet_l2_test(dut)


# ----------- DP83848 -----------
@pytest.mark.esp32
@pytest.mark.eth_dp83848
@pytest.mark.parametrize('config', [
    'default_dp83848',
], indirect=True)
def test_esp_eth_dp83848(dut: IdfDut) -> None:
    ethernet_test(dut)
    dut.serial.hard_reset()
    ethernet_l2_test(dut)


# ----------- W5500 -----------
@pytest.mark.esp32
@pytest.mark.eth_w5500
@pytest.mark.parametrize('config', [
    'default_w5500',
    'poll_w5500',
], indirect=True)
def test_esp_eth_w5500(dut: IdfDut) -> None:
    ethernet_test(dut)
    dut.serial.hard_reset()
    ethernet_l2_test(dut)
    dut.serial.hard_reset()
    ethernet_heap_alloc_test(dut)


# ----------- KSZ8851SNL -----------
@pytest.mark.esp32
@pytest.mark.eth_ksz8851snl
@pytest.mark.parametrize('config', [
    'default_ksz8851snl',
    'poll_ksz8851snl',
], indirect=True)
def test_esp_eth_ksz8851snl(dut: IdfDut) -> None:
    ethernet_test(dut)
    dut.serial.hard_reset()
    ethernet_l2_test(dut)
    dut.serial.hard_reset()
    ethernet_heap_alloc_test(dut)


# ----------- DM9051 -----------
@pytest.mark.esp32
@pytest.mark.eth_dm9051
@pytest.mark.parametrize('config', [
    'default_dm9051',
    'poll_dm9051',
], indirect=True)
def test_esp_eth_dm9051(dut: IdfDut) -> None:
    ethernet_test(dut)
    dut.serial.hard_reset()
    ethernet_l2_test(dut)
    dut.serial.hard_reset()
    ethernet_heap_alloc_test(dut)
