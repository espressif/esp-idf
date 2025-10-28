# SPDX-FileCopyrightText: 2022-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
"""
Test case for iperf example.

This test case might have problem running on Windows:

- use `sudo killall iperf` to force kill iperf, didn't implement windows version

"""

import os
import subprocess

import pytest
from common_test_methods import get_host_ip4_by_dest_ip
from idf_iperf_test_util import IperfUtility
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

try:
    from collections.abc import Callable
    from typing import Any
except ImportError:
    # Only used for type annotations
    pass

NO_BANDWIDTH_LIMIT = -1  # iperf send bandwidth is not limited


class IperfTestUtilityEth(IperfUtility.IperfTestUtility):
    """iperf test implementation"""

    def __init__(
        self, dut: str, config_name: str, pc_nic_ip: str, pc_iperf_log_file: str, test_result: Any = None
    ) -> None:
        IperfUtility.IperfTestUtility.__init__(
            self, dut, config_name, 'None', 'None', pc_nic_ip, pc_iperf_log_file, test_result
        )

    def setup(self) -> tuple[str, int]:
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
        self.dut.expect("Type 'help' to get the list of commands.")
        self.dut.expect('iperf>')
        dut_ip = self.dut.expect(r'esp_netif_handlers: .+ ip: (\d+\.\d+\.\d+\.\d+),').group(1)
        rssi = 0
        return dut_ip, rssi


def test_esp_eth_iperf(
    dut: Dut,
    log_performance: Callable[[str, object], None],
    check_performance: Callable[[str, float, str], None],
    udp_tx_bw_lim: int | None = NO_BANDWIDTH_LIMIT,
    udp_rx_bw_lim: int | None = NO_BANDWIDTH_LIMIT,
    spi_eth: bool | None = False,
) -> None:
    """
    steps: |
      1. test TCP tx rx and UDP tx rx throughput
      2. compare with the pre-defined pass standard
    """

    # 1. wait for DUT
    dut.expect_exact('iperf>')

    # 2. preparing
    pc_iperf_log_file = os.path.join(dut.logdir, 'pc_iperf_log.md')
    dut_ip = dut.expect(r'esp_netif_handlers: .+ ip: (\d+\.\d+\.\d+\.\d+),').group(1)
    pc_nic_ip = get_host_ip4_by_dest_ip(dut_ip)
    test_result = {
        'tcp_tx': IperfUtility.TestResult('tcp', 'tx', 'ethernet'),
        'tcp_rx': IperfUtility.TestResult('tcp', 'rx', 'ethernet'),
        'udp_tx': IperfUtility.TestResult('udp', 'tx', 'ethernet'),
        'udp_rx': IperfUtility.TestResult('udp', 'rx', 'ethernet'),
    }
    test_utility = IperfTestUtilityEth(dut, 'ethernet', pc_nic_ip, pc_iperf_log_file, test_result)

    # 3. run test for TCP Tx, Rx and UDP Tx, Rx
    test_utility.run_test('tcp', 'tx', 0, NO_BANDWIDTH_LIMIT)
    test_utility.run_test('tcp', 'rx', 0, NO_BANDWIDTH_LIMIT)
    test_utility.run_test('udp', 'tx', 0, udp_tx_bw_lim)
    test_utility.run_test('udp', 'rx', 0, udp_rx_bw_lim)

    # 4. log performance and compare with pass standard
    for throughput_type in test_result:
        log_performance(
            f'{throughput_type}_throughput',
            f'{test_result[throughput_type].get_best_throughput():.02f} Mbps',
        )

    # do check after logging, otherwise test will exit immediately if check fail, some performance can't be logged.
    for throughput_type in test_result:
        if spi_eth:
            check_performance(
                f'{throughput_type}_eth_throughput_spi_eth',
                test_result[throughput_type].get_best_throughput(),
                dut.target,
            )
        else:
            check_performance(
                f'{throughput_type}_eth_throughput',
                test_result[throughput_type].get_best_throughput(),
                dut.target,
            )


@pytest.mark.eth_ip101
@pytest.mark.parametrize(
    'config',
    [
        'default_ip101',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_esp_eth_iperf_ip101(
    dut: Dut,
    log_performance: Callable[[str, object], None],
    check_performance: Callable[[str, float, str], None],
) -> None:
    test_esp_eth_iperf(dut, log_performance, check_performance, udp_tx_bw_lim=90)


@pytest.mark.eth_ip101
@pytest.mark.parametrize(
    'config',
    [
        'default_ip101_esp32p4',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32p4'], indirect=['target'])
def test_esp_eth_iperf_ip101_esp32p4(
    dut: Dut,
    log_performance: Callable[[str, object], None],
    check_performance: Callable[[str, float, str], None],
) -> None:
    test_esp_eth_iperf(dut, log_performance, check_performance, udp_tx_bw_lim=96)


@pytest.mark.eth_lan8720
@pytest.mark.parametrize(
    'config',
    [
        'default_lan8720',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_esp_eth_iperf_lan8720(
    dut: Dut,
    log_performance: Callable[[str, object], None],
    check_performance: Callable[[str, float, str], None],
) -> None:
    test_esp_eth_iperf(dut, log_performance, check_performance, udp_tx_bw_lim=90)


@pytest.mark.eth_rtl8201
@pytest.mark.parametrize(
    'config',
    [
        'default_rtl8201',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_esp_eth_iperf_rtl8201(
    dut: Dut,
    log_performance: Callable[[str, object], None],
    check_performance: Callable[[str, float, str], None],
) -> None:
    test_esp_eth_iperf(dut, log_performance, check_performance, udp_tx_bw_lim=90)


@pytest.mark.eth_dp83848
@pytest.mark.parametrize(
    'config',
    [
        'default_dp83848',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_esp_eth_iperf_dp83848(
    dut: Dut,
    log_performance: Callable[[str, object], None],
    check_performance: Callable[[str, float, str], None],
) -> None:
    test_esp_eth_iperf(dut, log_performance, check_performance, udp_tx_bw_lim=90)


@pytest.mark.eth_ksz8041
@pytest.mark.parametrize(
    'config',
    [
        'default_ksz8041',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_esp_eth_iperf_ksz8041(
    dut: Dut,
    log_performance: Callable[[str, object], None],
    check_performance: Callable[[str, float, str], None],
) -> None:
    test_esp_eth_iperf(dut, log_performance, check_performance, udp_tx_bw_lim=90)


@pytest.mark.eth_dm9051
@pytest.mark.parametrize(
    'config',
    [
        'default_dm9051',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_esp_eth_iperf_dm9051(
    dut: Dut,
    log_performance: Callable[[str, object], None],
    check_performance: Callable[[str, float, str], None],
) -> None:
    test_esp_eth_iperf(dut, log_performance, check_performance, spi_eth=True, udp_rx_bw_lim=10)


@pytest.mark.eth_w5500
@pytest.mark.parametrize(
    'config',
    [
        'default_w5500',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_esp_eth_iperf_w5500(
    dut: Dut,
    log_performance: Callable[[str, object], None],
    check_performance: Callable[[str, float, str], None],
) -> None:
    test_esp_eth_iperf(dut, log_performance, check_performance, spi_eth=True, udp_rx_bw_lim=10)


@pytest.mark.eth_ksz8851snl
@pytest.mark.parametrize(
    'config',
    [
        'default_ksz8851snl',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32'], indirect=['target'])
def test_esp_eth_iperf_ksz8851snl(
    dut: Dut,
    log_performance: Callable[[str, object], None],
    check_performance: Callable[[str, float, str], None],
) -> None:
    test_esp_eth_iperf(dut, log_performance, check_performance, spi_eth=True, udp_rx_bw_lim=10)
