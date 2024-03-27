# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
"""
Test case for iperf example.

This test case might have problem running on windows:

- use `sudo killall iperf` to force kill iperf, didn't implement windows version

The test env wifi_iperf do need the following config::

  wifi_iperf:
    ap_ssid: "myssid"
    ap_password: "mypassword"
    pc_nic: "eth1"

"""
import os
import time
from typing import Any
from typing import Callable
from typing import Tuple

import pexpect
import pytest
from common_test_methods import get_env_config_variable
from common_test_methods import get_host_ip_by_interface
from idf_iperf_test_util import IperfUtility
from idf_iperf_test_util.IperfUtility import SCAN_RETRY_COUNT
from idf_iperf_test_util.IperfUtility import SCAN_TIMEOUT
from idf_iperf_test_util.IperfUtility import TEST_TIME
from pytest_embedded import Dut

# configurations
RETRY_COUNT_FOR_BEST_PERFORMANCE = 2
NO_BANDWIDTH_LIMIT = -1  # iperf send bandwith is not limited

# Use default value `99` for config with best performance.
BEST_PERFORMANCE_CONFIG = '99'


class IperfTestUtilitySoftap(IperfUtility.IperfTestUtility):
    """ iperf test implementation """
    def __init__(self, dut: Dut, softap_dut: Dut, config_name:str, test_result:Any=None) -> None:
        IperfUtility.IperfTestUtility.__init__(self, dut, config_name, 'softap', '1234567890', None, None, test_result)
        self.softap_dut = softap_dut
        self.softap_ip = '192.168.4.1'

    def setup(self) -> Tuple[str,int]:
        """
        setup iperf test:

        1. kill current iperf process
        2. reboot DUT (currently iperf is not very robust, need to reboot DUT)
        3. scan to get AP RSSI
        4. connect to AP
        """
        self.softap_dut.write('restart')
        self.softap_dut.expect_exact("Type 'help' to get the list of commands.")
        self.softap_dut.expect('iperf>', timeout=30)
        self.softap_dut.write('ap {} {}'.format(self.ap_ssid, self.ap_password))
        self.dut.write('restart')
        self.dut.expect_exact("Type 'help' to get the list of commands.")
        self.dut.expect('iperf>', timeout=30)
        self.dut.write('scan {}'.format(self.ap_ssid))
        for _ in range(SCAN_RETRY_COUNT):
            try:
                rssi = int(self.dut.expect(r'\[{}]\[rssi=(-\d+)]'.format(self.ap_ssid),
                                           timeout=SCAN_TIMEOUT).group(1))
                break
            except pexpect.TIMEOUT:
                continue
        else:
            raise AssertionError('Failed to scan AP')
        self.dut.write('sta {} {}'.format(self.ap_ssid, self.ap_password))
        dut_ip = self.dut.expect(r'sta ip: ([\d.]+), mask: ([\d.]+), gw: ([\d.]+)').group(1).decode('utf-8')
        return dut_ip, rssi

    def _test_once(self, proto:str, direction:str, bw_limit:int) -> Tuple[str, int, int]:
        """ do measure once for one type """
        # connect and scan to get RSSI
        dut_ip, rssi = self.setup()

        assert direction in ['rx', 'tx']
        assert proto in ['tcp', 'udp']

        # run iperf test
        if direction == 'tx':
            if proto == 'tcp':
                self.softap_dut.write('iperf -s -i 1 -t {}'.format(TEST_TIME))
                # wait until DUT TCP server created
                try:
                    self.softap_dut.expect('iperf tcp server create successfully', timeout=1)
                except pexpect.TIMEOUT:
                    # compatible with old iperf example binary
                    pass
                if bw_limit > 0:
                    self.dut.write('iperf -c {} -i 1 -t {} -b {}'.format(self.softap_ip, TEST_TIME, bw_limit))
                else:
                    self.dut.write('iperf -c {} -i 1 -t {}'.format(self.softap_ip, TEST_TIME))
            else:
                self.softap_dut.write('iperf -s -u -i 1 -t {}'.format(TEST_TIME))
                if bw_limit > 0:
                    self.dut.write('iperf -c {} -u -i 1 -t {} -b {}'.format(self.softap_ip, TEST_TIME, bw_limit))
                else:
                    self.dut.write('iperf -c {} -u -i 1 -t {}'.format(self.softap_ip, TEST_TIME))
        else:
            if proto == 'tcp':
                self.dut.write('iperf -s -i 1 -t {}'.format(TEST_TIME))
                # wait until DUT TCP server created
                try:
                    self.dut.expect('iperf tcp server create successfully', timeout=1)
                except pexpect.TIMEOUT:
                    # compatible with old iperf example binary
                    pass
                if bw_limit > 0:
                    self.softap_dut.write('iperf -c {} -i 1 -t {} -b {}'.format(dut_ip, TEST_TIME, bw_limit))
                else:
                    self.softap_dut.write('iperf -c {} -i 1 -t {}'.format(dut_ip, TEST_TIME))
            else:
                self.dut.write('iperf -s -u -i 1 -t {}'.format(TEST_TIME))
                if bw_limit > 0:
                    self.softap_dut.write('iperf -c {} -u -i 1 -t {} -b {}'.format(dut_ip, TEST_TIME, bw_limit))
                else:
                    self.softap_dut.write('iperf -c {} -u -i 1 -t {}'.format(dut_ip, TEST_TIME))
        time.sleep(TEST_TIME + 5)

        if direction == 'tx':
            server_raw_data = self.dut.expect(pexpect.TIMEOUT, timeout=0).decode('utf-8')
        else:
            server_raw_data = self.dut.expect(pexpect.TIMEOUT, timeout=0).decode('utf-8')
        self.dut.write('iperf -a')
        self.softap_dut.write('iperf -a')
        self.dut.write('heap')
        heap_size = self.dut.expect(r'min heap size: (\d+)\D').group(1)

        # return server raw data (for parsing test results) and RSSI
        return server_raw_data, rssi, heap_size


@pytest.mark.esp32
@pytest.mark.temp_skip_ci(targets=['esp32s2', 'esp32c3', 'esp32s3'], reason='lack of runners (run only for ESP32)')
@pytest.mark.timeout(1200)
@pytest.mark.wifi_iperf
@pytest.mark.parametrize('config', [
    BEST_PERFORMANCE_CONFIG
], indirect=True)
def test_wifi_throughput_basic(
    dut: Dut,
    log_performance: Callable[[str, str], None],
    check_performance: Callable[[str, float, str], None],
) -> None:
    """
    steps: |
      1. test TCP tx rx and UDP tx rx throughput
      2. compare with the pre-defined pass standard
    """
    # 1. wait for DUT
    dut.expect('iperf>')

    # 2. preparing
    env_name = 'wifi_iperf'
    pc_nic = get_env_config_variable(env_name, 'pc_nic')
    pc_nic_ip = get_host_ip_by_interface(pc_nic)
    pc_iperf_log_file = os.path.join(dut.logdir, 'pc_iperf_log.md')
    ap_info = {
        'ssid': get_env_config_variable(env_name, 'ap_ssid'),
        'password': get_env_config_variable(env_name, 'ap_password'),
    }

    test_result = {
        'tcp_tx': IperfUtility.TestResult('tcp', 'tx', BEST_PERFORMANCE_CONFIG),
        'tcp_rx': IperfUtility.TestResult('tcp', 'rx', BEST_PERFORMANCE_CONFIG),
        'udp_tx': IperfUtility.TestResult('udp', 'tx', BEST_PERFORMANCE_CONFIG),
        'udp_rx': IperfUtility.TestResult('udp', 'rx', BEST_PERFORMANCE_CONFIG),
    }

    test_utility = IperfUtility.IperfTestUtility(dut, BEST_PERFORMANCE_CONFIG, ap_info['ssid'], ap_info['password'],
                                                 pc_nic_ip, pc_iperf_log_file, test_result)

    # 3. run test for TCP Tx, Rx and UDP Tx, Rx
    for _ in range(RETRY_COUNT_FOR_BEST_PERFORMANCE):
        test_utility.run_all_cases(0, NO_BANDWIDTH_LIMIT)

    # 4. log performance and compare with pass standard
    for throughput_type in test_result:
        log_performance('{}_throughput'.format(throughput_type),
                        '{:.02f} Mbps'.format(test_result[throughput_type].get_best_throughput()))

    # do check after logging, otherwise test will exit immediately if check fail, some performance can't be logged.
    for throughput_type in test_result:
        check_performance('{}_throughput'.format(throughput_type),
                          test_result[throughput_type].get_best_throughput(), dut.target)
