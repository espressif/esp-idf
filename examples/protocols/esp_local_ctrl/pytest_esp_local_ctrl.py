# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0

import logging
import os
import re
import sys

import pexpect
import pytest
from common_test_methods import get_env_config_variable
from pytest_embedded import Dut


def get_sdk_path() -> str:
    idf_path = os.getenv('IDF_PATH')
    assert idf_path
    assert os.path.exists(idf_path)
    return idf_path


class CustomProcess(object):
    def __init__(self, cmd: str, logfile: str, verbose:bool =True) -> None:
        self.verbose = verbose
        self.f = open(logfile, 'w')
        if self.verbose:
            logging.info('Starting {} > {}'.format(cmd, self.f.name))
        self.pexpect_proc = pexpect.spawn(cmd, timeout=60, logfile=self.f, encoding='utf-8', codec_errors='ignore')

    def __enter__(self):    # type: ignore
        return self

    def close(self) -> None:
        self.pexpect_proc.terminate(force=True)

    def __exit__(self, type, value, traceback):     # type: ignore
        self.close()
        self.f.close()


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.wifi_router
@pytest.mark.parametrize(
    'config',
    [
        'default',
        'http',
    ],
    indirect=True,
)
def test_examples_esp_local_ctrl(config: str, dut: Dut) -> None:

    rel_project_path = os.path.join('examples', 'protocols', 'esp_local_ctrl')
    idf_path = get_sdk_path()

    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    dut_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]')[1].decode()
    if config == 'default':
        dut.expect('esp_https_server: Starting server')
        dut.expect('esp_https_server: Server listening on port 443')
    dut.expect('control: esp_local_ctrl service started with name : my_esp_ctrl_device')

    def dut_expect_read() -> None:
        dut.expect_exact('control: Reading property : timestamp (us)', timeout=20)
        dut.expect_exact('control: Reading property : property1')
        dut.expect_exact('control: Reading property : property2')
        dut.expect_exact('control: Reading property : property3')

    # Running mDNS services in docker is not a trivial task. Therefore, the script won't connect to the host name but
    # to IP address. However, the certificates were generated for the host name and will be rejected.
    if config == 'default':
        cmd = ' '.join([sys.executable, os.path.join(idf_path, rel_project_path, 'scripts/esp_local_ctrl.py'),
                        '--sec_ver 2',
                        '--sec2_username wifiprov',
                        '--sec2_pwd abcd1234',
                        '--name', dut_ip,
                        '--dont-check-hostname'])  # don't reject the certificate because of the hostname
    elif config == 'http':
        cmd = ' '.join([sys.executable, os.path.join(idf_path, rel_project_path, 'scripts/esp_local_ctrl.py'),
                        '--sec_ver 2',
                        '--transport http',
                        '--sec2_username wifiprov',
                        '--sec2_pwd abcd1234',
                        '--name', dut_ip,
                        '--dont-check-hostname'])
    esp_local_ctrl_log = os.path.join(idf_path, rel_project_path, 'esp_local_ctrl.log')
    with CustomProcess(cmd, esp_local_ctrl_log) as ctrl_py:

        def expect_properties(prop1: int, prop3: str) -> None:
            dut_expect_read()
            ctrl_py.pexpect_proc.expect_exact('==== Available Properties ====')
            ctrl_py.pexpect_proc.expect(re.compile(r'S.N. Name\s+Type\s+Flags\s+Value'))
            ctrl_py.pexpect_proc.expect(re.compile(r'\[ 1\] timestamp \(us\)\s+TIME\(us\)\s+Read-Only\s+\d+'))
            ctrl_py.pexpect_proc.expect(re.compile(r'\[ 2\] property1\s+INT32\s+{}'.format(prop1)))
            ctrl_py.pexpect_proc.expect(re.compile(r'\[ 3\] property2\s+BOOLEAN\s+Read-Only\s+(True)|(False)'))
            ctrl_py.pexpect_proc.expect(re.compile(r'\[ 4\] property3\s+STRING\s+{}'.format(prop3)))
            ctrl_py.pexpect_proc.expect_exact('Select properties to set (0 to re-read, \'q\' to quit) :')

        property1 = 123456789
        property3 = ''

        ctrl_py.pexpect_proc.expect_exact('Connecting to {}'.format(dut_ip))
        if config == 'default':
            dut.expect('esp_https_server: performing session handshake', timeout=60)
        expect_properties(property1, property3)

        ctrl_py.pexpect_proc.sendline('1')
        ctrl_py.pexpect_proc.expect_exact('Enter value to set for property (timestamp (us)) :')
        ctrl_py.pexpect_proc.sendline('2')
        ctrl_py.pexpect_proc.expect_exact('Failed to set values!')
        dut.expect_exact('control: timestamp (us) is read-only')
        expect_properties(property1, property3)

        property1 = 638
        ctrl_py.pexpect_proc.sendline('2')
        ctrl_py.pexpect_proc.expect_exact('Enter value to set for property (property1) :')
        ctrl_py.pexpect_proc.sendline(str(property1))
        dut.expect_exact('control: Setting property1 value to {}'.format(property1))
        expect_properties(property1, property3)

        property3 = 'test'
        ctrl_py.pexpect_proc.sendline('4')
        ctrl_py.pexpect_proc.expect_exact('Enter value to set for property (property3) :')
        ctrl_py.pexpect_proc.sendline(property3)
        dut.expect_exact('control: Setting property3 value to {}'.format(property3))
        expect_properties(property1, property3)

        ctrl_py.pexpect_proc.sendline('q')
        ctrl_py.pexpect_proc.expect_exact('Quitting...')
