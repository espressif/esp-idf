"""
Test case for iperf example.

This test case might have problem running on windows:

1. direct use of `make`
2. use `sudo killall iperf` to force kill iperf, didn't implement windows version

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
from __future__ import division, unicode_literals

import os
import re
import subprocess
import time
from builtins import range, str

import ttfw_idf
from idf_iperf_test_util import Attenuator, IperfUtility, PowerControl, TestReport
from idf_iperf_test_util.IperfUtility import SCAN_RETRY_COUNT, SCAN_TIMEOUT, TEST_TIME
from tiny_test_fw import DUT, TinyFW, Utility

# configurations
RETRY_COUNT_FOR_BEST_PERFORMANCE = 2
ATTEN_VALUE_LIST = range(0, 60, 2)
NO_BANDWIDTH_LIMIT = -1  # iperf send bandwith is not limited

CONFIG_NAME_PATTERN = re.compile(r'sdkconfig\.ci\.(.+)')

# We need to auto compare the difference between adjacent configs (01 -> 00, 02 -> 01, ...) and put them to reports.
# Using numbers for config will make this easy.
# Use default value `99` for config with best performance.
BEST_PERFORMANCE_CONFIG = '99'


class IperfTestUtilitySoftap(IperfUtility.IperfTestUtility):
    """ iperf test implementation """
    def __init__(self, dut, softap_dut, config_name, test_result=None):
        IperfUtility.IperfTestUtility.__init__(self, dut, config_name, 'softap', '1234567890', None, None, test_result)
        self.softap_dut = softap_dut
        self.softap_ip = '192.168.4.1'

    def setup(self):
        """
        setup iperf test:

        1. kill current iperf process
        2. reboot DUT (currently iperf is not very robust, need to reboot DUT)
        3. scan to get AP RSSI
        4. connect to AP
        """
        self.softap_dut.write('restart')
        self.softap_dut.expect_any('iperf>', 'esp32>', timeout=30)
        self.softap_dut.write('ap {} {}'.format(self.ap_ssid, self.ap_password))
        self.dut.write('restart')
        self.dut.expect_any('iperf>', 'esp32>', timeout=30)
        self.dut.write('scan {}'.format(self.ap_ssid))
        for _ in range(SCAN_RETRY_COUNT):
            try:
                rssi = int(self.dut.expect(re.compile(r'\[{}]\[rssi=(-\d+)]'.format(self.ap_ssid)),
                                           timeout=SCAN_TIMEOUT)[0])
                break
            except DUT.ExpectTimeout:
                continue
        else:
            raise AssertionError('Failed to scan AP')
        self.dut.write('sta {} {}'.format(self.ap_ssid, self.ap_password))
        dut_ip = self.dut.expect(re.compile(r'sta ip: ([\d.]+), mask: ([\d.]+), gw: ([\d.]+)'))[0]
        return dut_ip, rssi

    def _test_once(self, proto, direction):
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
                except DUT.ExpectTimeout:
                    # compatible with old iperf example binary
                    pass
                self.dut.write('iperf -c {} -i 1 -t {}'.format(self.softap_ip, TEST_TIME))
            else:
                self.softap_dut.write('iperf -s -u -i 1 -t {}'.format(TEST_TIME))
                self.dut.write('iperf -c {} -u -i 1 -t {}'.format(self.softap_ip, TEST_TIME))
        else:
            if proto == 'tcp':
                self.dut.write('iperf -s -i 1 -t {}'.format(TEST_TIME))
                # wait until DUT TCP server created
                try:
                    self.dut.expect('iperf tcp server create successfully', timeout=1)
                except DUT.ExpectTimeout:
                    # compatible with old iperf example binary
                    pass
                self.softap_dut.write('iperf -c {} -i 1 -t {}'.format(dut_ip, TEST_TIME))
            else:
                self.dut.write('iperf -s -u -i 1 -t {}'.format(TEST_TIME))
                self.softap_dut.write('iperf -c {} -u -i 1 -t {}'.format(dut_ip, TEST_TIME))
        time.sleep(60)

        if direction == 'tx':
            server_raw_data = self.dut.read()
        else:
            server_raw_data = self.softap_dut.read()
        self.dut.write('iperf -a')
        self.softap_dut.write('iperf -a')
        self.dut.write('heap')
        heap_size = self.dut.expect(re.compile(r'min heap size: (\d+)\D'))[0]

        # return server raw data (for parsing test results) and RSSI
        return server_raw_data, rssi, heap_size


@ttfw_idf.idf_example_test(env_tag='Example_ShieldBox_Basic', target=['ESP32', 'ESP32S2', 'ESP32C3', 'ESP32S3'], category='stress')
def test_wifi_throughput_with_different_configs(env, extra_data):
    """
    steps: |
      1. build iperf with specified configs
      2. test throughput for all routers
    """
    pc_nic_ip = env.get_pc_nic_info('pc_nic', 'ipv4')['addr']
    pc_iperf_log_file = os.path.join(env.log_path, 'pc_iperf_log.md')
    ap_info = {
        'ssid': env.get_variable('ap_ssid'),
        'password': env.get_variable('ap_password'),
    }

    config_names_raw = subprocess.check_output(['ls', os.path.dirname(os.path.abspath(__file__))])
    config_names = CONFIG_NAME_PATTERN.findall(config_names_raw)
    if not config_names:
        raise ValueError('no configs found in {}'.format(os.path.dirname(__file__)))

    test_result = dict()
    sdkconfig_files = dict()

    for config_name in config_names:
        # 1. get the config
        sdkconfig_files[config_name] = os.path.join(os.path.dirname(__file__),
                                                    'sdkconfig.ci.{}'.format(config_name))

        # 2. get DUT and download
        dut = env.get_dut('iperf', 'examples/wifi/iperf', app_config_name=config_name)
        dut.start_app()
        dut.expect_any('iperf>', 'esp32>')

        # 3. run test for each required att value
        test_result[config_name] = {
            'tcp_tx': IperfUtility.TestResult('tcp', 'tx', config_name),
            'tcp_rx': IperfUtility.TestResult('tcp', 'rx', config_name),
            'udp_tx': IperfUtility.TestResult('udp', 'tx', config_name),
            'udp_rx': IperfUtility.TestResult('udp', 'rx', config_name),
        }

        test_utility = IperfUtility.IperfTestUtility(dut, config_name, ap_info['ssid'], ap_info['password'], pc_nic_ip,
                                                     pc_iperf_log_file, test_result[config_name])

        for _ in range(RETRY_COUNT_FOR_BEST_PERFORMANCE):
            test_utility.run_all_cases(0, NO_BANDWIDTH_LIMIT)

        for result_type in test_result[config_name]:
            summary = str(test_result[config_name][result_type])
            if summary:
                Utility.console_log(summary, color='orange')

        # 4. check test results
        env.close_dut('iperf')

    # 5. generate report
    report = TestReport.ThroughputForConfigsReport(os.path.join(env.log_path, 'Performance',
                                                                'ThroughputForConfigsReport'),
                                                   ap_info['ssid'], test_result, sdkconfig_files)
    report.generate_report()


@ttfw_idf.idf_example_test(env_tag='Example_ShieldBox', target=['ESP32', 'ESP32S2', 'ESP32C3', 'ESP32S3'], category='stress')
def test_wifi_throughput_vs_rssi(env, extra_data):
    """
    steps: |
      1. build with best performance config
      2. switch on one router
      3. set attenuator value from 0-60 for each router
      4. test TCP tx rx and UDP tx rx throughput
    """
    att_port = env.get_variable('attenuator_port')
    ap_list = env.get_variable('ap_list')
    pc_nic_ip = env.get_pc_nic_info('pc_nic', 'ipv4')['addr']
    apc_ip = env.get_variable('apc_ip')
    pc_iperf_log_file = os.path.join(env.log_path, 'pc_iperf_log.md')

    test_result = {
        'tcp_tx': IperfUtility.TestResult('tcp', 'tx', BEST_PERFORMANCE_CONFIG),
        'tcp_rx': IperfUtility.TestResult('tcp', 'rx', BEST_PERFORMANCE_CONFIG),
        'udp_tx': IperfUtility.TestResult('udp', 'tx', BEST_PERFORMANCE_CONFIG),
        'udp_rx': IperfUtility.TestResult('udp', 'rx', BEST_PERFORMANCE_CONFIG),
    }

    # 1. get DUT and download
    dut = env.get_dut('iperf', 'examples/wifi/iperf', app_config_name=BEST_PERFORMANCE_CONFIG)
    dut.start_app()
    dut.expect_any('iperf>', 'esp32>')

    # 2. run test for each required att value
    for ap_info in ap_list:
        test_utility = IperfUtility.IperfTestUtility(dut, BEST_PERFORMANCE_CONFIG, ap_info['ssid'],
                                                     ap_info['password'], pc_nic_ip, pc_iperf_log_file, test_result)

        PowerControl.Control.control_rest(apc_ip, ap_info['outlet'], 'OFF')
        PowerControl.Control.control(apc_ip, {ap_info['outlet']: 'ON'})
        Attenuator.set_att(att_port, 0)

        if not test_utility.wait_ap_power_on():
            Utility.console_log('[{}] failed to power on, skip testing this AP'
                                .format(ap_info['ssid']), color='red')
            continue

        for atten_val in ATTEN_VALUE_LIST:
            assert Attenuator.set_att(att_port, atten_val) is True
            try:
                test_utility.run_all_cases(atten_val, NO_BANDWIDTH_LIMIT)
            except AssertionError:
                break

    # 3. check test results
    env.close_dut('iperf')

    # 4. generate report
    report = TestReport.ThroughputVsRssiReport(os.path.join(env.log_path, 'Performance', 'STAThroughputVsRssiReport'),
                                               test_result)
    report.generate_report()


@ttfw_idf.idf_example_test(env_tag='Example_ShieldBox_Basic',
                           target=['ESP32', 'ESP32S2', 'ESP32C3', 'ESP32S3'], ci_target=['ESP32'])
def test_wifi_throughput_basic(env, extra_data):
    """
    steps: |
      1. test TCP tx rx and UDP tx rx throughput
      2. compare with the pre-defined pass standard
    """
    pc_nic_ip = env.get_pc_nic_info('pc_nic', 'ipv4')['addr']
    pc_iperf_log_file = os.path.join(env.log_path, 'pc_iperf_log.md')
    ap_info = {
        'ssid': env.get_variable('ap_ssid'),
        'password': env.get_variable('ap_password'),
    }

    # 1. get DUT
    dut = env.get_dut('iperf', 'examples/wifi/iperf', app_config_name=BEST_PERFORMANCE_CONFIG)
    dut.start_app()
    dut.expect_any('iperf>', 'esp32>')

    # 2. preparing
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
    performance_items = []
    for throughput_type in test_result:
        ttfw_idf.log_performance('{}_throughput'.format(throughput_type),
                                 '{:.02f} Mbps'.format(test_result[throughput_type].get_best_throughput()))
        performance_items.append(['{}_throughput'.format(throughput_type),
                                  '{:.02f} Mbps'.format(test_result[throughput_type].get_best_throughput())])

    # 5. save to report
    TinyFW.JunitReport.update_performance(performance_items)
    # do check after logging, otherwise test will exit immediately if check fail, some performance can't be logged.
    for throughput_type in test_result:
        ttfw_idf.check_performance('{}_throughput'.format(throughput_type),
                                   test_result[throughput_type].get_best_throughput(), dut.TARGET)

    env.close_dut('iperf')


@ttfw_idf.idf_example_test(env_tag='Example_ShieldBox2', target=['ESP32', 'ESP32S2', 'ESP32C3', 'ESP32S3'], category='stress')
def test_softap_throughput_vs_rssi(env, extra_data):
    """
    steps: |
      1. build with best performance config
      2. switch on one router
      3. set attenuator value from 0-60 for each router
      4. test TCP tx rx and UDP tx rx throughput
    """
    att_port = env.get_variable('attenuator_port')

    test_result = {
        'tcp_tx': IperfUtility.TestResult('tcp', 'tx', BEST_PERFORMANCE_CONFIG),
        'tcp_rx': IperfUtility.TestResult('tcp', 'rx', BEST_PERFORMANCE_CONFIG),
        'udp_tx': IperfUtility.TestResult('udp', 'tx', BEST_PERFORMANCE_CONFIG),
        'udp_rx': IperfUtility.TestResult('udp', 'rx', BEST_PERFORMANCE_CONFIG),
    }

    # 1. get DUT and download
    softap_dut = env.get_dut('softap_iperf', 'examples/wifi/iperf')
    softap_dut.start_app()
    softap_dut.expect_any('iperf>', 'esp32>')

    sta_dut = env.get_dut('sta_iperf', 'examples/wifi/iperf', app_config_name=BEST_PERFORMANCE_CONFIG)
    sta_dut.start_app()
    sta_dut.expect_any('iperf>', 'esp32>')

    # 2. run test for each required att value
    test_utility = IperfTestUtilitySoftap(sta_dut, softap_dut, BEST_PERFORMANCE_CONFIG, test_result)

    Attenuator.set_att(att_port, 0)

    for atten_val in ATTEN_VALUE_LIST:
        assert Attenuator.set_att(att_port, atten_val) is True
        try:
            test_utility.run_all_cases(atten_val, NO_BANDWIDTH_LIMIT)
        except AssertionError:
            break

    env.close_dut('softap_iperf')
    env.close_dut('sta_iperf')

    # 3. generate report
    report = TestReport.ThroughputVsRssiReport(os.path.join(env.log_path, 'Performance',
                                                            'SoftAPThroughputVsRssiReport'),test_result)
    report.generate_report()


if __name__ == '__main__':
    # test_wifi_throughput_basic(env_config_file='EnvConfig.yml')
    # test_wifi_throughput_with_different_configs(env_config_file='EnvConfig.yml')
    test_wifi_throughput_vs_rssi(env_config_file='EnvConfig.yml', target='ESP32C3')
    test_softap_throughput_vs_rssi(env_config_file='EnvConfig.yml')
