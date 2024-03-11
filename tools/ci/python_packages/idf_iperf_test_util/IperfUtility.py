# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import logging
import os
import re
import subprocess
import time

import pexpect
from idf_iperf_test_util import LineChart
from pytest_embedded import Dut

try:
    from typing import Any, Tuple
except ImportError:
    # Only used for type annotations
    pass

# configurations
TEST_TIME = TEST_TIMEOUT = 66
WAIT_AP_POWER_ON_TIMEOUT = 90
SCAN_TIMEOUT = 3
SCAN_RETRY_COUNT = 3

# constants
FAILED_TO_SCAN_RSSI = -97
INVALID_HEAP_SIZE = 0xFFFFFFFF

PC_IPERF_TEMP_LOG_FILE = '.tmp_iperf.log'


class TestResult(object):
    """ record, analysis test result and convert data to output format """

    PC_BANDWIDTH_LOG_PATTERN = re.compile(r'(\d+\.\d+)\s*-\s*(\d+.\d+)\s+sec\s+[\d.]+\s+MBytes\s+([\d.]+)\s+Mbits\/sec')
    DUT_BANDWIDTH_LOG_PATTERN = re.compile(r'([\d.]+)\s*-\s*([\d.]+)\s+sec\s+([\d.]+)\s+Mbits/sec')

    ZERO_POINT_THRESHOLD = -88  # RSSI, dbm
    ZERO_THROUGHPUT_THRESHOLD = -92  # RSSI, dbm
    BAD_POINT_RSSI_THRESHOLD = -75  # RSSI, dbm
    BAD_POINT_MIN_THRESHOLD = 10  # Mbps
    BAD_POINT_PERCENTAGE_THRESHOLD = 0.3

    # we need at least 1/2 valid points to qualify the test result
    THROUGHPUT_QUALIFY_COUNT = TEST_TIME // 2

    RSSI_RANGE = [-x for x in range(10, 100)]
    ATT_RANGE = [x for x in range(0, 64)]

    def __init__(self, proto:str, direction:str, config_name:str) -> None:
        self.proto = proto
        self.direction = direction
        self.config_name = config_name
        self.throughput_by_rssi = dict()  # type: dict
        self.throughput_by_att = dict()  # type: dict
        self.att_rssi_map = dict()  # type: dict
        self.heap_size = INVALID_HEAP_SIZE
        self.error_list = []  # type: list[str]

    def _save_result(self, throughput:float, ap_ssid:str, att:int, rssi:int, heap_size:str) -> None:
        """
        save the test results:

        * record the better throughput if att/rssi is the same.
        * record the min heap size.
        """
        if ap_ssid not in self.att_rssi_map:
            # for new ap, create empty dict()
            self.throughput_by_att[ap_ssid] = dict()
            self.throughput_by_rssi[ap_ssid] = dict()
            self.att_rssi_map[ap_ssid] = dict()

        self.att_rssi_map[ap_ssid][att] = rssi

        def record_throughput(database:dict, key_value:int) -> None:
            try:
                # we save the larger value for same att
                if throughput > database[ap_ssid][key_value]:
                    database[ap_ssid][key_value] = throughput
            except KeyError:
                database[ap_ssid][key_value] = throughput

        record_throughput(self.throughput_by_att, att)
        record_throughput(self.throughput_by_rssi, rssi)

        if int(heap_size) < self.heap_size:
            self.heap_size = int(heap_size)

    def add_result(self, raw_data:str, ap_ssid:str, att:int, rssi:int, heap_size:str) -> float:
        """
        add result for one test

        :param raw_data: iperf raw data
        :param ap_ssid: ap ssid that tested
        :param att: attenuate value
        :param rssi: AP RSSI
        :param heap_size: min heap size during test
        :return: throughput
        """
        fall_to_0_recorded = 0
        throughput_list = []
        max_throughput = 0.0
        result_list = self.PC_BANDWIDTH_LOG_PATTERN.findall(raw_data)
        if not result_list:
            # failed to find raw data by PC pattern, it might be DUT pattern
            result_list = self.DUT_BANDWIDTH_LOG_PATTERN.findall(raw_data)

        for result in result_list:
            t_start = float(result[0])
            t_end = float(result[1])
            throughput = float(result[2])
            if int(t_end - t_start) != 1:
                # this could be summary, ignore this
                continue
            throughput_list.append(throughput)
            max_throughput = max(max_throughput, throughput)
            if throughput == 0 and rssi > self.ZERO_POINT_THRESHOLD \
                    and fall_to_0_recorded < 1:
                # throughput fall to 0 error. we only record 1 records for one test
                self.error_list.append('[Error][fall to 0][{}][att: {}][rssi: {}]: 0 throughput interval: {}-{}'
                                       .format(ap_ssid, att, rssi, result[0], result[1]))
                fall_to_0_recorded += 1

        if len(throughput_list) < self.THROUGHPUT_QUALIFY_COUNT:
            self.error_list.append('[Error][Fatal][{}][att: {}][rssi: {}]: Only {} throughput values found, expected at least {}'
                                   .format(ap_ssid, att, rssi, len(throughput_list), self.THROUGHPUT_QUALIFY_COUNT))
            max_throughput = 0.0

        if max_throughput == 0 and rssi > self.ZERO_THROUGHPUT_THRESHOLD:
            self.error_list.append('[Error][Fatal][{}][att: {}][rssi: {}]: No throughput data found'
                                   .format(ap_ssid, att, rssi))

        self._save_result(max_throughput, ap_ssid, att, rssi, heap_size)

        return max_throughput

    def post_analysis(self) -> None:
        """
        some rules need to be checked after we collected all test raw data:

        1. throughput value 30% worse than the next point with lower RSSI
        2. throughput value 30% worse than the next point with larger attenuate
        """
        def analysis_bad_point(data:dict, index_type:str) -> None:
            for ap_ssid in data:
                result_dict = data[ap_ssid]
                index_list = list(result_dict.keys())
                index_list.sort()
                if index_type == 'att':
                    index_list.reverse()

                for i, index_value in enumerate(index_list[1:]):
                    if index_value < self.BAD_POINT_RSSI_THRESHOLD or \
                            result_dict[index_list[i]] < self.BAD_POINT_MIN_THRESHOLD:
                        continue
                    _percentage = result_dict[index_value] / result_dict[index_list[i]]
                    if _percentage < 1 - self.BAD_POINT_PERCENTAGE_THRESHOLD:
                        self.error_list.append('[Error][Bad point][{}][{}: {}]: drop {:.02f}%'
                                               .format(ap_ssid, index_type, index_value,
                                                       (1 - _percentage) * 100))

        analysis_bad_point(self.throughput_by_rssi, 'rssi')
        analysis_bad_point(self.throughput_by_att, 'att')

    def draw_throughput_figure(self, path:str, ap_ssid:str, draw_type:str) -> str:
        """
        :param path: folder to save figure. make sure the folder is already created.
        :param ap_ssid: ap ssid string or a list of ap ssid string
        :param draw_type: "att" or "rssi"
        :return: file_name
        """
        if draw_type == 'rssi':
            type_name = 'RSSI'
            data = self.throughput_by_rssi
            range_list = self.RSSI_RANGE
        elif draw_type == 'att':
            type_name = 'Att'
            data = self.throughput_by_att
            range_list = self.ATT_RANGE
        else:
            raise AssertionError('draw type not supported')
        if isinstance(ap_ssid, list):
            file_name = 'ThroughputVs{}_{}_{}.html'.format(type_name, self.proto, self.direction)
        else:
            file_name = 'ThroughputVs{}_{}_{}.html'.format(type_name, self.proto, self.direction)

        LineChart.draw_line_chart(os.path.join(path, file_name),
                                  'Throughput Vs {} ({} {})'.format(type_name, self.proto, self.direction),
                                  '{} (dbm)'.format(type_name),
                                  'Throughput (Mbps)',
                                  data, range_list)
        return file_name

    def draw_rssi_vs_att_figure(self, path:str, ap_ssid:str) -> str:
        """
        :param path: folder to save figure. make sure the folder is already created.
        :param ap_ssid: ap to use
        :return: file_name
        """
        if isinstance(ap_ssid, list):
            file_name = 'AttVsRSSI.html'
        else:
            file_name = 'AttVsRSSI.html'
        LineChart.draw_line_chart(os.path.join(path, file_name),
                                  'Att Vs RSSI',
                                  'Att (dbm)',
                                  'RSSI (dbm)',
                                  self.att_rssi_map,
                                  self.ATT_RANGE)
        return file_name

    def get_best_throughput(self) -> Any:
        """  get the best throughput during test """
        best_for_aps = [max(self.throughput_by_att[ap_ssid].values())
                        for ap_ssid in self.throughput_by_att]
        return max(best_for_aps)

    def __str__(self) -> str:
        """
        returns summary for this test:

        1. test result (success or fail)
        2. best performance for each AP
        3. min free heap size during test
        """
        if self.throughput_by_att:
            ret = '[{}_{}][{}]: {}\r\n\r\n'.format(self.proto, self.direction, self.config_name,
                                                   'Fail' if self.error_list else 'Success')
            ret += 'Performance for each AP:\r\n'
            for ap_ssid in self.throughput_by_att:
                ret += '[{}]: {:.02f} Mbps\r\n'.format(ap_ssid, max(self.throughput_by_att[ap_ssid].values()))
            if self.heap_size != INVALID_HEAP_SIZE:
                ret += 'Minimum heap size: {}'.format(self.heap_size)
        else:
            ret = ''
        return ret


class IperfTestUtility(object):
    """ iperf test implementation """

    def __init__(self, dut:Dut, config_name:str, ap_ssid:str, ap_password:str,
                 pc_nic_ip:str, pc_iperf_log_file:str, test_result:Any=None) -> None:
        self.config_name = config_name
        self.dut = dut

        self.pc_iperf_log_file = pc_iperf_log_file
        self.ap_ssid = ap_ssid
        self.ap_password = ap_password
        self.pc_nic_ip = pc_nic_ip
        self.fail_to_scan = 0
        self.lowest_rssi_scanned = 0

        if test_result:
            self.test_result = test_result
        else:
            self.test_result = {
                'tcp_tx': TestResult('tcp', 'tx', config_name),
                'tcp_rx': TestResult('tcp', 'rx', config_name),
                'udp_tx': TestResult('udp', 'tx', config_name),
                'udp_rx': TestResult('udp', 'rx', config_name),
            }

    def setup(self) -> Tuple[str,int]:
        """
        setup iperf test:

        1. kill current iperf process
        2. reboot DUT (currently iperf is not very robust, need to reboot DUT)
        3. scan to get AP RSSI
        4. connect to AP
        """
        try:
            subprocess.check_output('sudo killall iperf 2>&1 > /dev/null', shell=True)
        except subprocess.CalledProcessError:
            pass
        time.sleep(5)
        self.dut.write('restart')
        self.dut.expect_exact("Type 'help' to get the list of commands.")
        self.dut.expect('iperf>')
        self.dut.write('sta_scan {}'.format(self.ap_ssid))
        for _ in range(SCAN_RETRY_COUNT):
            try:
                rssi = int(self.dut.expect(r'\[{}]\[rssi=(-\d+)]'.format(self.ap_ssid),
                                           timeout=SCAN_TIMEOUT).group(1))
                break
            except pexpect.TIMEOUT:
                continue
        else:
            raise AssertionError('Failed to scan AP')
        self.dut.write('sta_connect {} {}'.format(self.ap_ssid, self.ap_password))
        dut_ip = self.dut.expect(r'sta ip: ([\d.]+), mask: ([\d.]+), gw: ([\d.]+)').group(1)
        return dut_ip, rssi

    def _save_test_result(self, test_case:str, raw_data:str, att:int, rssi:int, heap_size:int) -> Any:
        return self.test_result[test_case].add_result(raw_data, self.ap_ssid, att, rssi, heap_size)

    def _test_once(self, proto:str, direction:str, bw_limit:int) -> Tuple[str, int, int]:
        """ do measure once for one type """
        # connect and scan to get RSSI
        dut_ip, rssi = self.setup()

        assert direction in ['rx', 'tx']
        assert proto in ['tcp', 'udp']

        # run iperf test
        if direction == 'tx':
            with open(PC_IPERF_TEMP_LOG_FILE, 'w') as f:
                if proto == 'tcp':
                    process = subprocess.Popen(['iperf', '-s', '-B', self.pc_nic_ip,
                                                '-t', str(TEST_TIME), '-i', '1', '-f', 'm'],
                                               stdout=f, stderr=f)
                    if bw_limit > 0:
                        self.dut.write('iperf -c {} -i 1 -t {} -b {}'.format(self.pc_nic_ip, TEST_TIME, bw_limit))
                    else:
                        self.dut.write('iperf -c {} -i 1 -t {}'.format(self.pc_nic_ip, TEST_TIME))
                else:
                    process = subprocess.Popen(['iperf', '-s', '-u', '-B', self.pc_nic_ip,
                                                '-t', str(TEST_TIME), '-i', '1', '-f', 'm'],
                                               stdout=f, stderr=f)
                    if bw_limit > 0:
                        self.dut.write('iperf -c {} -u -i 1 -t {} -b {}'.format(self.pc_nic_ip, TEST_TIME, bw_limit))
                    else:
                        self.dut.write('iperf -c {} -u -i 1 -t {}'.format(self.pc_nic_ip, TEST_TIME))

                for _ in range(TEST_TIMEOUT):
                    if process.poll() is not None:
                        break
                    time.sleep(1)
                else:
                    process.terminate()

            with open(PC_IPERF_TEMP_LOG_FILE, 'r') as f:
                pc_raw_data = server_raw_data = f.read()
        else:
            with open(PC_IPERF_TEMP_LOG_FILE, 'w') as f:
                if proto == 'tcp':
                    self.dut.write('iperf -s -i 1 -t {}'.format(TEST_TIME))
                    # wait until DUT TCP server created
                    try:
                        self.dut.expect('Socket created', timeout=5)
                    except pexpect.TIMEOUT:
                        # compatible with old iperf example binary
                        logging.info('create iperf tcp server fail')
                    if bw_limit > 0:
                        process = subprocess.Popen(['iperf', '-c', dut_ip, '-b', str(bw_limit) + 'm',
                                                    '-t', str(TEST_TIME), '-f', 'm'], stdout=f, stderr=f)
                    else:
                        process = subprocess.Popen(['iperf', '-c', dut_ip,
                                                    '-t', str(TEST_TIME), '-f', 'm'], stdout=f, stderr=f)
                    for _ in range(TEST_TIMEOUT):
                        if process.poll() is not None:
                            break
                        time.sleep(1)
                    else:
                        process.terminate()
                else:
                    self.dut.write('iperf -s -u -i 1 -t {}'.format(TEST_TIME))
                    # wait until DUT TCP server created
                    try:
                        self.dut.expect('Socket bound', timeout=5)
                    except pexpect.TIMEOUT:
                        # compatible with old iperf example binary
                        logging.info('create iperf udp server fail')
                    if bw_limit > 0:
                        process = subprocess.Popen(['iperf', '-c', dut_ip, '-u', '-b', str(bw_limit) + 'm',
                                                    '-t', str(TEST_TIME), '-f', 'm'], stdout=f, stderr=f)
                        for _ in range(TEST_TIMEOUT):
                            if process.poll() is not None:
                                break
                            time.sleep(1)
                        else:
                            process.terminate()
                    else:
                        for bandwidth in range(50, 101, 5):
                            process = subprocess.Popen(['iperf', '-c', dut_ip, '-u', '-b', str(bandwidth) + 'm',
                                                        '-t', str(TEST_TIME / 11), '-f', 'm'], stdout=f, stderr=f)
                            for _ in range(TEST_TIMEOUT):
                                if process.poll() is not None:
                                    break
                                time.sleep(1)
                            else:
                                process.terminate()

            server_raw_data = self.dut.expect(pexpect.TIMEOUT, timeout=0).decode('utf-8')
            with open(PC_IPERF_TEMP_LOG_FILE, 'r') as f:
                pc_raw_data = f.read()

        if os.path.exists(PC_IPERF_TEMP_LOG_FILE):
            os.remove(PC_IPERF_TEMP_LOG_FILE)

        # save PC iperf logs to console
        with open(self.pc_iperf_log_file, 'a+') as f:
            f.write('## [{}] `{}`\r\n##### {}'
                    .format(self.config_name,
                            '{}_{}'.format(proto, direction),
                            time.strftime('%m-%d %H:%M:%S', time.localtime(time.time()))))
            f.write('\r\n```\r\n\r\n' + pc_raw_data + '\r\n```\r\n')
        self.dut.write('heap')
        heap_size = self.dut.expect(r'min heap size: (\d+)\D').group(1)

        # return server raw data (for parsing test results) and RSSI
        return server_raw_data, rssi, heap_size

    def run_test(self, proto:str, direction:str, atten_val:int, bw_limit:int) -> None:
        """
        run test for one type, with specified atten_value and save the test result

        :param proto: tcp or udp
        :param direction: tx or rx
        :param atten_val: attenuate value
        :param bw_limit: bandwidth limit
        """
        rssi = FAILED_TO_SCAN_RSSI
        heap_size = INVALID_HEAP_SIZE
        try:
            server_raw_data, rssi, heap_size = self._test_once(proto, direction, bw_limit)
            throughput = self._save_test_result('{}_{}'.format(proto, direction),
                                                server_raw_data, atten_val,
                                                rssi, heap_size)
            logging.info('[{}][{}_{}][{}][{}]: {:.02f}'
                         .format(self.config_name, proto, direction, rssi, self.ap_ssid, throughput))
            self.lowest_rssi_scanned = min(self.lowest_rssi_scanned, rssi)
        except (ValueError, IndexError):
            self._save_test_result('{}_{}'.format(proto, direction), '', atten_val, rssi, heap_size)
            logging.info('Fail to get throughput results.')
        except AssertionError:
            self.fail_to_scan += 1
            logging.info('Fail to scan AP.')

    def run_all_cases(self, atten_val:int, bw_limit:int) -> None:
        """
        run test for all types (udp_tx, udp_rx, tcp_tx, tcp_rx).

        :param atten_val: attenuate value
        :param bw_limit: bandwidth limit
        """
        self.run_test('tcp', 'tx', atten_val, bw_limit)
        self.run_test('tcp', 'rx', atten_val, bw_limit)
        self.run_test('udp', 'tx', atten_val, bw_limit)
        self.run_test('udp', 'rx', atten_val, bw_limit)
        if self.fail_to_scan > 10:
            logging.info(
                'Fail to scan AP for more than 10 times. Lowest RSSI scanned is {}'.format(self.lowest_rssi_scanned))
            raise AssertionError

    def wait_ap_power_on(self) -> bool:
        """
        AP need to take sometime to power on. It changes for different APs.
        This method will scan to check if the AP powers on.

        :return: True or False
        """
        self.dut.write('restart')
        self.dut.expect('iperf>')
        for _ in range(WAIT_AP_POWER_ON_TIMEOUT // SCAN_TIMEOUT):
            try:
                self.dut.write('scan {}'.format(self.ap_ssid))
                self.dut.expect(r'\[{}]\[rssi=(-\d+)]'.format(self.ap_ssid),
                                timeout=SCAN_TIMEOUT)
                ret = True
                break
            except pexpect.TIMEOUT:
                pass
        else:
            ret = False
        return ret
