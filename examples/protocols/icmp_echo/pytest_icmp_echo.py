# SPDX-FileCopyrightText: 2021-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

import os

import pytest
from common_test_methods import get_env_config_variable
from pytest_embedded import Dut


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
    dut.expect(fr'64 bytes from {ip} icmp_seq=[2-5] ttl=\d+ time=')

    dut.expect(r'5 packets transmitted, [2-5] received, \d{1,3}% packet loss')
    dut.write('')
    dut.expect('esp>')


@pytest.mark.esp32
@pytest.mark.esp32c2
@pytest.mark.esp32s2
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.esp32c6
@pytest.mark.wifi_ap
def test_protocols_icmp_echo(dut: Dut) -> None:
    _run_test(dut)


@pytest.mark.esp32c2
@pytest.mark.wifi_ap
@pytest.mark.xtal_26mhz
@pytest.mark.parametrize(
    'config, baud', [
        ('c2_xtal26m', '74880'),
    ], indirect=True
)
def test_protocols_icmp_echo_esp32c2_26mhz(dut: Dut) -> None:
    _run_test(dut)
