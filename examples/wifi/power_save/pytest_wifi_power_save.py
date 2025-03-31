# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging

import pexpect
import pytest
from common_test_methods import get_env_config_variable
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

bad_event_str = [
    'bcn_timeout',
    'm f probe req l',
    'abort() was called',
    'Guru Meditation Error',
]


def _run_test(dut: Dut) -> None:
    if dut.app.sdkconfig.get('EXAMPLE_GET_AP_INFO_FROM_STDIN') is True:
        env_name = 'wifi_ap'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        tag = 'power_save'
        dut.expect(f'{tag}: Input SSID:', timeout=5)
        dut.write(f'{ap_ssid}')
        dut.expect(f'{tag}: Input Password:', timeout=5)
        dut.write(f'{ap_password}')
    else:
        # for local test may config ssid/password from menuconfig
        pass

    try:
        dut.expect(r'got ip: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)
        log_after_got_ip = dut.expect(pexpect.TIMEOUT, timeout=10).decode()
        if any(s in log_after_got_ip for s in bad_event_str):
            logging.info('Abnormal connection log:')
            logging.info('\n' + log_after_got_ip)
            raise RuntimeError('Something abnormal happened after got ip')
    except pexpect.exceptions.TIMEOUT:
        raise RuntimeError('Failed to get ip in power save mode')


@pytest.mark.wifi_ap
@idf_parametrize(
    'target',
    ['esp32', 'esp32c2', 'esp32s2', 'esp32c3', 'esp32s3', 'esp32c6', 'esp32c5', 'esp32c61'],
    indirect=['target'],
)
def test_wifi_power_save(dut: Dut) -> None:
    _run_test(dut)


@pytest.mark.wifi_ap
@pytest.mark.parametrize(
    'config',
    [
        'pd_top',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c6'], indirect=['target'])
def test_wifi_power_save_pd_top(dut: Dut) -> None:
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
def test_wifi_power_save_esp32c2_26mhz(dut: Dut) -> None:
    _run_test(dut)


@pytest.mark.wifi_ap
@pytest.mark.xtal_26mhz
@pytest.mark.esp32c2eco4
@pytest.mark.parametrize(
    'config, baud',
    [
        ('c2eco4_xtal26m', '74880'),
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c2'], indirect=['target'])
def test_wifi_power_save_esp32c2eco4_26mhz(dut: Dut) -> None:
    _run_test(dut)


@pytest.mark.wifi_ap
@pytest.mark.esp32c3eco7
@pytest.mark.parametrize(
    'config',
    [
        'c3eco7',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32c3'], indirect=['target'])
def test_wifi_power_save_esp32c3eco7(dut: Dut) -> None:
    _run_test(dut)
