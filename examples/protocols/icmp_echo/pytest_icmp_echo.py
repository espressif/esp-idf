# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import os

import pytest
from common_test_methods import get_env_config_variable
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


def _run_test(dut: Dut) -> None:
    if dut.app.sdkconfig.get('EXAMPLE_PROVIDE_WIFI_CONSOLE_CMD') is True:
        dut.expect('esp>')
        env_name = 'wifi_ap'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        ap_channel = get_env_config_variable(env_name, 'ap_channel', default=0)
        dut.write(f'wifi_connect {ap_ssid} {ap_password} -n {ap_channel}')
    else:
        # for local test may config ssid/password from menuconfig
        pass

    dut.expect('Got IPv4 event:', timeout=30)

    ping_dest = os.getenv('EXAMPLE_ICMP_SERVER', 'ci.espressif.cn')
    dut.write('ping {} -c 5'.format(ping_dest))

    # expect at least two packets (there could be lost packets)
    ip = dut.expect(r'64 bytes from (\d+\.\d+\.\d+\.\d+) icmp_seq=\d ttl=\d+ time=\d+ ms')[1].decode()
    dut.expect(rf'64 bytes from {ip} icmp_seq=[2-5] ttl=\d+ time=')

    dut.expect(r'5 packets transmitted, [2-5] received, \d{1,3}% packet loss')
    dut.write('')
    dut.expect('esp>')


@pytest.mark.wifi_ap
@idf_parametrize(
    'target',
    ['esp32', 'esp32c2', 'esp32s2', 'esp32c3', 'esp32s3', 'esp32c5', 'esp32c6', 'esp32c61'],
    indirect=['target'],
)
def test_protocols_icmp_echo(dut: Dut) -> None:
    _run_test(dut)


@pytest.mark.wifi_ap
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'config, baud',
    [
        ('c2_xtal26m', '74880'),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c2'], indirect=['target'])
def test_protocols_icmp_echo_esp32c2_26mhz(dut: Dut) -> None:
    _run_test(dut)


@pytest.mark.wifi_router
@pytest.mark.parametrize(
    'config',
    [
        'ipv6_only',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_protocols_icmp_echo_ipv6_only(dut: Dut) -> None:
    # Parse IP address of STA
    logging.info('Waiting to connect with AP')
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    # expect all 8 octets from IPv6 (assumes it's printed in the long form)
    ipv6_r = r':'.join((r'[0-9a-fA-F]{4}',) * 8)
    ipv6 = dut.expect(ipv6_r, timeout=30)[0].decode()
    logging.info(f'Connected AP with IPv6={ipv6}')
    interface_nr = dut.expect(r'Connected on interface: [a-z]{2}\d \((\d+)\)', timeout=30)[1].decode()

    # ping our own address to simplify things
    dut.write('ping -I {} {} -c 5'.format(interface_nr, ipv6))

    # expect at least two packets (there could be lost packets)
    ip = dut.expect(r'64 bytes from ([0-9a-fA-F:]+) icmp_seq=\d ttl=\d+ time=\d+ ms')[1].decode()
    dut.expect(rf'64 bytes from {ip} icmp_seq=[2-5] ttl=\d+ time=')

    dut.expect(r'5 packets transmitted, [2-5] received, \d{1,3}% packet loss')
    dut.write('')
    dut.expect('esp>')
