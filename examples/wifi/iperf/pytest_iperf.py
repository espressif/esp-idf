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
from typing import Callable

import pytest
from common_test_methods import get_env_config_variable
from common_test_methods import get_host_ip_by_interface
from idf_iperf_test_util import IperfUtility
from pytest_embedded import Dut

# configurations
RETRY_COUNT_FOR_BEST_PERFORMANCE = 2
NO_BANDWIDTH_LIMIT = -1  # iperf send bandwith is not limited

# Use default value `99` for config with best performance.
BEST_PERFORMANCE_CONFIG = '99'


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
