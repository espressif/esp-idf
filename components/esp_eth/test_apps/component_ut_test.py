import os
import re
import socket

import tiny_test_fw
import ttfw_idf
from tiny_test_fw import Utility
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
        Utility.console_log('detected interfaces: ' + str(netifs))
        for netif in netifs:
            if netif.find('eth') == 0 or netif.find('enp') == 0 or netif.find('eno') == 0:
                target_if = netif
                break
        if target_if == '':
            raise Exception('no network interface found')
        Utility.console_log('Use ' + target_if + ' for testing')
        so = socket.socket(socket.AF_PACKET, socket.SOCK_RAW, 0x2222)
        so.bind((target_if, 0))

        func(so, *args, **kwargs)

        so.close()

    return inner


@configure_eth_if
def check_eth_recv_packet(so, before_recv=None):    # type: (socket.socket, typing.Any) -> None
    so.settimeout(10)
    if before_recv is not None:
        before_recv()   # If configured, execute user function just before sock recv
    try:
        pkt = so.recv(1024)
        for i in range(128, 1024):
            if pkt[i] != i & 0xff:
                raise Exception('Packet content mismatch')
    except Exception as e:
        raise e


@configure_eth_if
def send_eth_packet(so, mac):    # type: (socket.socket, bytes) -> None
    so.settimeout(10)
    pkt = bytearray()
    pkt += mac                      # dest
    pkt += so.getsockname()[4]      # src
    pkt += bytes.fromhex('2222')    # proto
    pkt += bytes(1010)              # padding to 1024
    for i in range(128, 1024):
        pkt[i] = i & 0xff
    try:
        so.send(pkt)
    except Exception as e:
        raise e


def test_component_ut_esp_eth(env, appname):  # type: (tiny_test_fw.Env, str) -> None
    dut = env.get_dut('esp_eth', 'components/esp_eth/test_apps', app_config_name=appname)
    dut.start_app()
    stdout = dut.expect('Press ENTER to see the list of tests', full_stdout=True)

    Utility.console_log('Running test case: start_and_stop')
    dut.write('"start_and_stop"')
    stdout += dut.expect("Enter next test, or 'enter' to see menu", full_stdout=True)
    ttfw_idf.ComponentUTResult.parse_result(stdout, test_format=TestFormat.UNITY_BASIC)

    Utility.console_log('Running test case: get_set_mac')
    dut.write('"get_set_mac"')
    stdout = dut.expect("Enter next test, or 'enter' to see menu", full_stdout=True)
    ttfw_idf.ComponentUTResult.parse_result(stdout, test_format=TestFormat.UNITY_BASIC)

    Utility.console_log('Running test case: ethernet_broadcast_transmit')
    check_eth_recv_packet(dut.write('"ethernet_broadcast_transmit"'))  # Need to start the test after the socket is bound
    stdout = dut.expect("Enter next test, or 'enter' to see menu", full_stdout=True)
    ttfw_idf.ComponentUTResult.parse_result(stdout, test_format=TestFormat.UNITY_BASIC)

    Utility.console_log('Running test case: recv_pkt')
    dut.write('"recv_pkt"')
    expect_result = dut.expect(re.compile(r'([\s\S]*)DUT MAC: ([0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2}:[0-9A-Fa-f]{2})'),
                               timeout=10)
    stdout = expect_result[0]
    Utility.console_log('DUTs MAC address: {}'.format(expect_result[1]))
    send_eth_packet(bytes.fromhex('ffffffffffff'))      # broadcast frame
    send_eth_packet(bytes.fromhex('010000000000'))      # multicast frame
    send_eth_packet(bytes.fromhex(expect_result[1].replace(':', '')))   # unicast frame
    stdout += dut.expect("Enter next test, or 'enter' to see menu", full_stdout=True)
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
