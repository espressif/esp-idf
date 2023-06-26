# SPDX-FileCopyrightText: 2022-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
"""
Test case for iperf example.

This test case might have problem running on windows:

- use `sudo killall iperf` to force kill iperf, didn't implement windows version

The test env Example_ShieldBox do need the following config::

  Example_ShieldBox:
    ap_list:
      - ssid: "ssid"
        password: "password"
        outlet: 1
    apc_ip: "192.168.1.88"
    attenuator_port: "/dev/ttyUSB0"
    iperf: "/dev/ttyUSB1"
    apc_ip: "192.168.1.88"
    pc_nic: "eth0"
"""

import logging
import os
import re
import time
from typing import Any, Callable, Dict, Generator, Tuple

import pexpect
import pytest
from common_test_methods import get_env_config_variable, get_host_ip_by_interface
from idf_iperf_test_util import Attenuator, IperfUtility, PowerControl, TestReport
from idf_iperf_test_util.IperfUtility import SCAN_RETRY_COUNT, SCAN_TIMEOUT, TEST_TIME
from pytest_embedded import Dut
from pytest_embedded_idf.dut import IdfDut

# configurations
RETRY_COUNT_FOR_BEST_PERFORMANCE = 2
ATTEN_VALUE_LIST = range(0, 60, 2)
NO_BANDWIDTH_LIMIT = -1  # iperf send bandwith is not limited

# We need to auto compare the difference between adjacent configs (01 -> 00, 02 -> 01, ...) and put them to reports.
# Using numbers for config will make this easy.
# Use default value `99` for config with best performance.
BEST_PERFORMANCE_CONFIG = '99'


class IperfTestUtilitySoftap(IperfUtility.IperfTestUtility):
    """ iperf test implementation """
    def __init__(self, dut:IdfDut, softap_dut:IdfDut, config_name:str, test_result:Any=None) -> None:
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


@pytest.fixture(name='generate_report_different_configs', scope='session')
def fixture_generate_report_different_configs(
    session_tempdir:str
) -> Generator[Callable[[Dict[str, Any], Dict[str, Any], str], None], None, None]:
    _test_result_dict = dict()
    _sdkconfig_files_dict = dict()
    _ap_info = dict()

    def add_config(ap_info:Dict[str, Any], test_result:Dict[str, Any], config_name:str) -> None:
        """
        Collects results for each config and stores it to a dictionary
            Args:
                ap_info: AP info
                test_result: test results for a specific config
                config_name: config name
        """
        # need to store the SSID to generate the report in the teardown period
        # note that the info passed along with the last call of the fixture is used in the teardown period
        _ap_info['ssid'] = ap_info['ssid']

        _test_result_dict[config_name] = test_result
        _sdkconfig_files_dict[config_name] = 'sdkconfig.ci.' + config_name

    yield add_config

    # the final report for all config results is generated during fixture's teardown period
    report = TestReport.ThroughputForConfigsReport(os.path.join(session_tempdir, 'Performance',
                                                   'ThroughputForConfigsReport'), _ap_info['ssid'],
                                                   _test_result_dict, _sdkconfig_files_dict)
    report.generate_report()


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.temp_skip_ci(targets=['esp32s2', 'esp32c3', 'esp32s3'], reason='lack of runners (run only for ESP32)')
@pytest.mark.timeout(1200)
@pytest.mark.Example_ShieldBox_Basic
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
    env_name = 'Example_ShieldBox_Basic'
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


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.temp_skip_ci(targets=['esp32', 'esp32s2', 'esp32c3', 'esp32s3'], reason='local stress test')
@pytest.mark.timeout(1200)
@pytest.mark.Example_ShieldBox_Basic
@pytest.mark.parametrize('config', [
    '00',
    '01',
    '02',
    '03',
    '04',
    '05',
    '06',
    '07',
    '99'
], indirect=True)
def test_wifi_throughput_with_different_configs(
    dut: Dut,
    generate_report_different_configs: Callable[[Dict[str, Any], Dict[str, Any], str], None],
) -> None:
    """
    steps: |
      1. build iperf with specified configs
      2. test throughput for all routers
    """
    # 1. wait for DUT
    dut.expect('iperf>')

    # 2. preparing
    env_name = 'Example_ShieldBox_Basic'
    pc_nic = get_env_config_variable(env_name, 'pc_nic')
    pc_nic_ip = get_host_ip_by_interface(pc_nic)
    pc_iperf_log_file = os.path.join(dut.logdir, 'pc_iperf_log.md')
    ap_info = {
        'ssid': get_env_config_variable(env_name, 'ap_ssid'),
        'password': get_env_config_variable(env_name, 'ap_password'),
    }

    found_config = re.search(r'esp32.*\.(\w+)\.', dut.test_case_name)
    if found_config is not None:
        config_name = found_config.group(1)
    else:
        raise Exception('config name not found')

    # 3. run test for each required att value
    test_result = {
        'tcp_tx': IperfUtility.TestResult('tcp', 'tx', config_name),
        'tcp_rx': IperfUtility.TestResult('tcp', 'rx', config_name),
        'udp_tx': IperfUtility.TestResult('udp', 'tx', config_name),
        'udp_rx': IperfUtility.TestResult('udp', 'rx', config_name),
    }
    test_utility = IperfUtility.IperfTestUtility(dut, config_name, ap_info['ssid'], ap_info['password'], pc_nic_ip,
                                                 pc_iperf_log_file, test_result)
    for _ in range(RETRY_COUNT_FOR_BEST_PERFORMANCE):
        test_utility.run_all_cases(0, NO_BANDWIDTH_LIMIT)

    for result_type in test_result:
        summary = str(test_result[result_type])
        if summary:
            logging.info(summary)

    generate_report_different_configs(ap_info, test_result, config_name)


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.temp_skip(targets=['esp32', 'esp32s2', 'esp32c3', 'esp32s3'], reason='lack of runners')
@pytest.mark.timeout(3600)
@pytest.mark.Example_ShieldBox
@pytest.mark.parametrize('config', [
    BEST_PERFORMANCE_CONFIG
], indirect=True)
def test_wifi_throughput_vs_rssi(
    dut: Dut,
    session_tempdir:str,
) -> None:
    """
    steps: |
      1. build with best performance config
      2. switch on one router
      3. set attenuator value from 0-60 for each router
      4. test TCP tx rx and UDP tx rx throughput
    """
    # 1. wait for DUT
    dut.expect('iperf>')

    # 2. preparing
    env_name = 'Example_ShieldBox'
    att_port = get_env_config_variable(env_name, 'attenuator_port')
    ap_list = get_env_config_variable(env_name, 'ap_list')
    pc_nic = get_env_config_variable(env_name, 'pc_nic')
    pc_nic_ip = get_host_ip_by_interface(pc_nic)
    apc_ip = get_env_config_variable(env_name, 'apc_ip')
    pc_iperf_log_file = os.path.join(dut.logdir, 'pc_iperf_log.md')

    test_result = {
        'tcp_tx': IperfUtility.TestResult('tcp', 'tx', BEST_PERFORMANCE_CONFIG),
        'tcp_rx': IperfUtility.TestResult('tcp', 'rx', BEST_PERFORMANCE_CONFIG),
        'udp_tx': IperfUtility.TestResult('udp', 'tx', BEST_PERFORMANCE_CONFIG),
        'udp_rx': IperfUtility.TestResult('udp', 'rx', BEST_PERFORMANCE_CONFIG),
    }

    # 3. run test for each required att value
    for ap_info in ap_list:
        test_utility = IperfUtility.IperfTestUtility(dut, BEST_PERFORMANCE_CONFIG, ap_info['ssid'],
                                                     ap_info['password'], pc_nic_ip, pc_iperf_log_file, test_result)
        PowerControl.Control.control_rest(apc_ip, ap_info['outlet'], 'OFF')
        PowerControl.Control.control(apc_ip, {ap_info['outlet']: 'ON'})
        Attenuator.set_att(att_port, 0)
        if not test_utility.wait_ap_power_on():
            logging.error('[{}] failed to power on, skip testing this AP'.format(ap_info['ssid']))
            continue
        for atten_val in ATTEN_VALUE_LIST:
            assert Attenuator.set_att(att_port, atten_val) is True
            try:
                test_utility.run_all_cases(atten_val, NO_BANDWIDTH_LIMIT)
            except AssertionError:
                break

    # 4. generate report
    report = TestReport.ThroughputVsRssiReport(os.path.join(session_tempdir, 'Performance', 'STAThroughputVsRssiReport'),
                                               test_result)
    report.generate_report()


@pytest.mark.esp32
@pytest.mark.esp32s2
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.temp_skip(targets=['esp32', 'esp32s2', 'esp32c3', 'esp32s3'], reason='lack of runners')
@pytest.mark.parametrize('count, config', [
    (2, BEST_PERFORMANCE_CONFIG),
], indirect=True)
def test_softap_throughput_vs_rssi(
    dut: Tuple[IdfDut, IdfDut],
    session_tempdir:str,
) -> None:
    """
    steps: |
      1. build with best performance config
      2. switch on one router
      3. set attenuator value from 0-60 for each router
      4. test TCP tx rx and UDP tx rx throughput
    """
    # 1. wait for DUTs
    softap_dut = dut[0]
    sta_dut = dut[1]
    softap_dut.expect('iperf>')
    sta_dut.expect('iperf>')

    # 2. preparing
    env_name = 'Example_ShieldBox2'
    att_port = get_env_config_variable(env_name, 'attenuator_port')

    test_result = {
        'tcp_tx': IperfUtility.TestResult('tcp', 'tx', BEST_PERFORMANCE_CONFIG),
        'tcp_rx': IperfUtility.TestResult('tcp', 'rx', BEST_PERFORMANCE_CONFIG),
        'udp_tx': IperfUtility.TestResult('udp', 'tx', BEST_PERFORMANCE_CONFIG),
        'udp_rx': IperfUtility.TestResult('udp', 'rx', BEST_PERFORMANCE_CONFIG),
    }

    # 3. run test for each required att value
    test_utility = IperfTestUtilitySoftap(sta_dut, softap_dut, BEST_PERFORMANCE_CONFIG, test_result)

    Attenuator.set_att(att_port, 0)

    for atten_val in ATTEN_VALUE_LIST:
        assert Attenuator.set_att(att_port, atten_val) is True
        try:
            test_utility.run_all_cases(atten_val, NO_BANDWIDTH_LIMIT)
        except AssertionError:
            break

    # 4. generate report
    report = TestReport.ThroughputVsRssiReport(os.path.join(session_tempdir, 'Performance',
                                                            'SoftAPThroughputVsRssiReport'),test_result)
    report.generate_report()
