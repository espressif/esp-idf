import os
import re
import socket
from collections.abc import Callable
from threading import Thread

import tiny_test_fw
import ttfw_idf
from scapy.all import Ether, raw
from ttfw_idf import TestFormat

try:
    import typing  # noqa: F401 # pylint: disable=unused-import
except ImportError:
    pass


def configure_eth_if(func):         # type: (typing.Any) -> typing.Any
    def inner(*args, **kwargs):     # type: (typing.Any, typing.Any) -> typing.Any
        # try to determine which interface to use
        netifs = os.listdir('/sys/class/net/')
        target_if = ''
        print('detected interfaces: ' + str(netifs))
        for netif in netifs:
            if netif.find('eth') == 0 or netif.find('enp') == 0 or netif.find('eno') == 0:
                target_if = netif
                break
        if target_if == '':
            raise Exception('no network interface found')
        print('Use ' + target_if + ' for testing')
        so = socket.socket(socket.AF_PACKET, socket.SOCK_RAW, 0x2222)
        so.bind((target_if, 0))

        func(so, *args, **kwargs)

        so.close()

    return inner


@configure_eth_if
def check_eth_recv_packet(so):    # type: (socket.socket) -> None
    so.settimeout(10)
    try:
        eth_frame = Ether(so.recv(1024))
        for i in range(0, 1010):
            if eth_frame.load[i] != i & 0xff:
                raise Exception('Packet content mismatch')
    except Exception as e:
        raise e


@configure_eth_if
def send_eth_packet(so, mac):    # type: (socket.socket, str) -> None
    so.settimeout(10)
    payload = bytearray(1010)
    for i, _ in enumerate(payload):
        payload[i] = i & 0xff
    eth_frame = Ether(dst=mac, src=so.getsockname()[4], type=0x2222) / raw(payload)
    try:
        so.send(raw(eth_frame))
    except Exception as e:
        raise e


@configure_eth_if
def recv_resp_poke(so, i):    # type: (socket.socket, int) -> None
    so.settimeout(10)
    try:
        eth_frame = Ether(so.recv(60))

        if eth_frame.type == 0x2222 and eth_frame.load[0] == 0xfa:
            if eth_frame.load[1] != i:
                raise Exception('Missed Poke Packet')
            eth_frame.dst = eth_frame.src
            eth_frame.src = so.getsockname()[4]
            eth_frame.load = bytes.fromhex('fb')    # POKE_RESP code
            so.send(raw(eth_frame))
    except Exception as e:
        raise e


@configure_eth_if
def traffic_gen(so, mac, enabled):  # type: (socket.socket, str, Callable) -> None
    payload = bytes.fromhex('ff')    # DUMMY_TRAFFIC code
    payload += bytes(1485)
    eth_frame = Ether(dst=mac, src=so.getsockname()[4], type=0x2222) / raw(payload)
    try:
        while enabled() == 1:
            so.send(raw(eth_frame))
    except Exception as e:
        raise e


def test_component_ut_esp_eth(env, appname):  # type: (tiny_test_fw.Env, str) -> None
    dut = env.get_dut('esp_eth', 'components/esp_eth/test_apps', app_config_name=appname)
    dut.start_app()
    stdout = dut.expect('Press ENTER to see the list of tests', full_stdout=True)
    dut.write('"start_and_stop"')
    stdout += dut.expect("Enter next test, or 'enter' to see menu", full_stdout=True)
    ttfw_idf.ComponentUTResult.parse_result(stdout, test_format=TestFormat.UNITY_BASIC)
    dut.write('"get_set_mac"')
    stdout = dut.expect("Enter next test, or 'enter' to see menu", full_stdout=True)
    ttfw_idf.ComponentUTResult.parse_result(stdout, test_format=TestFormat.UNITY_BASIC)
    dut.write('"ethernet_broadcast_transmit"')
    check_eth_recv_packet()
    stdout = dut.expect("Enter next test, or 'enter' to see menu", full_stdout=True)
    ttfw_idf.ComponentUTResult.parse_result(stdout, test_format=TestFormat.UNITY_BASIC)
    dut.write('"recv_pkt"')
    expect_result = dut.expect(re.compile(
        r'([\s\S]*)'
        r'DUT MAC: ([0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2})'),
        timeout=10
    )
    stdout = expect_result[0]
    send_eth_packet('ff:ff:ff:ff:ff:ff')      # broadcast frame
    send_eth_packet('01:00:00:00:00:00')      # multicast frame
    send_eth_packet(expect_result[1])   # unicast frame
    stdout += dut.expect("Enter next test, or 'enter' to see menu", full_stdout=True)
    ttfw_idf.ComponentUTResult.parse_result(stdout, test_format=TestFormat.UNITY_BASIC)

    dut.write('"start_stop_stress_test"')
    expect_result = dut.expect(re.compile(
        r'([\s\S]*)'
        r'DUT MAC: ([0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2})'),
        timeout=10
    )

    # Start/stop under heavy Tx traffic
    for tx_i in range(10):
        recv_resp_poke(tx_i)

    # Start/stop under heavy Rx traffic
    traffic_en = 1
    thread = Thread(target=traffic_gen, args=(expect_result[1], lambda:traffic_en, ))
    thread.start()
    try:
        for rx_i in range(10):
            recv_resp_poke(rx_i)
    finally:
        traffic_en = 0
        thread.join()

    stdout = dut.expect("Enter next test, or 'enter' to see menu", full_stdout=True)
    ttfw_idf.ComponentUTResult.parse_result(stdout, test_format=TestFormat.UNITY_BASIC)


@ttfw_idf.idf_component_unit_test(env_tag='COMPONENT_UT_IP101', target=['esp32'])
def test_component_ut_esp_eth_ip101(env, _):  # type: (tiny_test_fw.Env, typing.Any) -> None
    test_component_ut_esp_eth(env, 'ip101')


@ttfw_idf.idf_component_unit_test(env_tag='COMPONENT_UT_LAN8720', target=['esp32'])
def test_component_ut_esp_eth_lan8720(env, _):  # type: (tiny_test_fw.Env, typing.Any) -> None
    test_component_ut_esp_eth(env, 'lan8720')


if __name__ == '__main__':
    test_component_ut_esp_eth_ip101()
    test_component_ut_esp_eth_lan8720()
