"""
Test case for iperf example.

This test case might have problem running on windows:

1. direct use of `make`
2. use `sudo killall iperf` to force kill iperf, didn't implement windows version

"""
from __future__ import division, unicode_literals

import os
import re
import subprocess
import time

import ttfw_idf
from idf_iperf_test_util import IperfUtility
from tiny_test_fw import TinyFW

try:
    from typing import Any, Tuple
except ImportError:
    # Only used for type annotations
    pass


class IperfTestUtilityEth(IperfUtility.IperfTestUtility):
    """ iperf test implementation """
    def __init__(self, dut, config_name, pc_nic_ip, pc_iperf_log_file, test_result=None):   # type: (str, str, str,str, Any) -> None
        IperfUtility.IperfTestUtility.__init__(self, dut, config_name, 'None', 'None', pc_nic_ip, pc_iperf_log_file, test_result)

    def setup(self):  # type: () -> Tuple[str,int]
        """
        setup iperf test:

        1. kill current iperf process
        2. reboot DUT (currently iperf is not very robust, need to reboot DUT)
        """
        try:
            subprocess.check_output('sudo killall iperf 2>&1 > /dev/null', shell=True)
        except subprocess.CalledProcessError:
            pass
        self.dut.write('restart')
        self.dut.expect_any('iperf>', 'esp32>')
        self.dut.write('ethernet start')
        time.sleep(10)
        self.dut.write('ethernet info')
        dut_ip = self.dut.expect(re.compile(r'ETHIP: ([\d.]+)'))[0]
        rssi = 0
        return dut_ip, rssi


@ttfw_idf.idf_example_test(env_tag='Example_Ethernet')
def test_ethernet_throughput_basic(env, _):  # type: (Any, Any) -> None
    """
    steps: |
      1. test TCP tx rx and UDP tx rx throughput
      2. compare with the pre-defined pass standard
    """
    pc_nic_ip = env.get_pc_nic_info('pc_nic', 'ipv4')['addr']
    pc_iperf_log_file = os.path.join(env.log_path, 'pc_iperf_log.md')

    # 1. get DUT
    dut = env.get_dut('iperf', 'examples/ethernet/iperf', dut_class=ttfw_idf.ESP32DUT)
    dut.start_app()
    dut.expect_any('iperf>', 'esp32>')

    # 2. preparing
    test_result = {
        'tcp_tx': IperfUtility.TestResult('tcp', 'tx', 'ethernet'),
        'tcp_rx': IperfUtility.TestResult('tcp', 'rx', 'ethernet'),
        'udp_tx': IperfUtility.TestResult('udp', 'tx', 'ethernet'),
        'udp_rx': IperfUtility.TestResult('udp', 'rx', 'ethernet'),
    }

    test_utility = IperfTestUtilityEth(dut, 'ethernet', pc_nic_ip, pc_iperf_log_file, test_result)

    # 3. run test for TCP Tx, Rx and UDP Tx, Rx

    test_utility.run_all_cases(0)

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
        ttfw_idf.check_performance('{}_throughput'.format(throughput_type + '_eth'),
                                   test_result[throughput_type].get_best_throughput(), dut.TARGET)

    env.close_dut('iperf')


if __name__ == '__main__':
    test_ethernet_throughput_basic(env_config_file='EnvConfig.yml')
