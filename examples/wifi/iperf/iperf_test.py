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
from __future__ import division
from __future__ import unicode_literals
from builtins import str
from builtins import range
from builtins import object
import re
import os
import time
import subprocess

from tiny_test_fw import TinyFW, DUT, Utility
import ttfw_idf
from idf_iperf_test_util import (Attenuator, PowerControl, LineChart, TestReport)


# configurations
TEST_TIME = TEST_TIMEOUT = 60
WAIT_AP_POWER_ON_TIMEOUT = 90
SCAN_TIMEOUT = 3
SCAN_RETRY_COUNT = 3
RETRY_COUNT_FOR_BEST_PERFORMANCE = 2
ATTEN_VALUE_LIST = range(0, 60, 2)

# constants
FAILED_TO_SCAN_RSSI = -97
INVALID_HEAP_SIZE = 0xFFFFFFFF

PC_IPERF_TEMP_LOG_FILE = ".tmp_iperf.log"
CONFIG_NAME_PATTERN = re.compile(r"sdkconfig\.ci\.(.+)")

# We need to auto compare the difference between adjacent configs (01 -> 00, 02 -> 01, ...) and put them to reports.
# Using numbers for config will make this easy.
# Use default value `99` for config with best performance.
BEST_PERFORMANCE_CONFIG = "99"


class TestResult(object):
    """ record, analysis test result and convert data to output format """

    PC_BANDWIDTH_LOG_PATTERN = re.compile(r"(\d+).0\s*-\s*(\d+).0\s+sec\s+[\d.]+\s+MBytes\s+([\d.]+)\s+Mbits/sec")
    DUT_BANDWIDTH_LOG_PATTERN = re.compile(r"(\d+)-\s+(\d+)\s+sec\s+([\d.]+)\s+Mbits/sec")

    ZERO_POINT_THRESHOLD = -88  # RSSI, dbm
    ZERO_THROUGHPUT_THRESHOLD = -92  # RSSI, dbm
    BAD_POINT_RSSI_THRESHOLD = -85  # RSSI, dbm
    BAD_POINT_MIN_THRESHOLD = 3  # Mbps
    BAD_POINT_PERCENTAGE_THRESHOLD = 0.3

    # we need at least 1/2 valid points to qualify the test result
    THROUGHPUT_QUALIFY_COUNT = TEST_TIME // 2

    def __init__(self, proto, direction, config_name):
        self.proto = proto
        self.direction = direction
        self.config_name = config_name
        self.throughput_by_rssi = dict()
        self.throughput_by_att = dict()
        self.att_rssi_map = dict()
        self.heap_size = INVALID_HEAP_SIZE
        self.error_list = []

    def _save_result(self, throughput, ap_ssid, att, rssi, heap_size):
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

        def record_throughput(database, key_value):
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

    def add_result(self, raw_data, ap_ssid, att, rssi, heap_size):
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
        result_list = self.PC_BANDWIDTH_LOG_PATTERN.findall(raw_data)
        if not result_list:
            # failed to find raw data by PC pattern, it might be DUT pattern
            result_list = self.DUT_BANDWIDTH_LOG_PATTERN.findall(raw_data)

        for result in result_list:
            if int(result[1]) - int(result[0]) != 1:
                # this could be summary, ignore this
                continue
            throughput_list.append(float(result[2]))
            if float(result[2]) == 0 and rssi > self.ZERO_POINT_THRESHOLD \
                    and fall_to_0_recorded < 1:
                # throughput fall to 0 error. we only record 1 records for one test
                self.error_list.append("[Error][fall to 0][{}][att: {}][rssi: {}]: 0 throughput interval: {}-{}"
                                       .format(ap_ssid, att, rssi, result[0], result[1]))
                fall_to_0_recorded += 1

        if len(throughput_list) > self.THROUGHPUT_QUALIFY_COUNT:
            throughput = sum(throughput_list) / len(throughput_list)
        else:
            throughput = 0.0

        if throughput == 0 and rssi > self.ZERO_THROUGHPUT_THRESHOLD:
            self.error_list.append("[Error][Fatal][{}][att: {}][rssi: {}]: No throughput data found"
                                   .format(ap_ssid, att, rssi))

        self._save_result(throughput, ap_ssid, att, rssi, heap_size)

        return throughput

    def post_analysis(self):
        """
        some rules need to be checked after we collected all test raw data:

        1. throughput value 30% worse than the next point with lower RSSI
        2. throughput value 30% worse than the next point with larger attenuate
        """
        def analysis_bad_point(data, index_type):
            for ap_ssid in data:
                result_dict = data[ap_ssid]
                index_list = list(result_dict.keys())
                index_list.sort()
                if index_type == "att":
                    index_list.reverse()

                for i, index_value in enumerate(index_list[1:]):
                    if index_value < self.BAD_POINT_RSSI_THRESHOLD or \
                            result_dict[index_list[i]] < self.BAD_POINT_MIN_THRESHOLD:
                        continue
                    _percentage = result_dict[index_value] / result_dict[index_list[i]]
                    if _percentage < 1 - self.BAD_POINT_PERCENTAGE_THRESHOLD:
                        self.error_list.append("[Error][Bad point][{}][{}: {}]: drop {:.02f}%"
                                               .format(ap_ssid, index_type, index_value,
                                                       (1 - _percentage) * 100))

        analysis_bad_point(self.throughput_by_rssi, "rssi")
        analysis_bad_point(self.throughput_by_att, "att")

    @staticmethod
    def _convert_to_draw_format(data, label):
        keys = list(data.keys())
        keys.sort()
        return {
            "x-axis": keys,
            "y-axis": [data[x] for x in keys],
            "label": label,
        }

    def draw_throughput_figure(self, path, ap_ssid, draw_type):
        """
        :param path: folder to save figure. make sure the folder is already created.
        :param ap_ssid: ap ssid string or a list of ap ssid string
        :param draw_type: "att" or "rssi"
        :return: file_name
        """
        if draw_type == "rssi":
            type_name = "RSSI"
            data = self.throughput_by_rssi
        elif draw_type == "att":
            type_name = "Att"
            data = self.throughput_by_att
        else:
            raise AssertionError("draw type not supported")
        if isinstance(ap_ssid, list):
            file_name = "ThroughputVs{}_{}_{}_{}.png".format(type_name, self.proto, self.direction,
                                                             hash(ap_ssid)[:6])
            data_list = [self._convert_to_draw_format(data[_ap_ssid], _ap_ssid)
                         for _ap_ssid in ap_ssid]
        else:
            file_name = "ThroughputVs{}_{}_{}_{}.png".format(type_name, self.proto, self.direction, ap_ssid)
            data_list = [self._convert_to_draw_format(data[ap_ssid], ap_ssid)]

        LineChart.draw_line_chart(os.path.join(path, file_name),
                                  "Throughput Vs {} ({} {})".format(type_name, self.proto, self.direction),
                                  "{} (dbm)".format(type_name),
                                  "Throughput (Mbps)",
                                  data_list)
        return file_name

    def draw_rssi_vs_att_figure(self, path, ap_ssid):
        """
        :param path: folder to save figure. make sure the folder is already created.
        :param ap_ssid: ap to use
        :return: file_name
        """
        if isinstance(ap_ssid, list):
            file_name = "AttVsRSSI_{}.png".format(hash(ap_ssid)[:6])
            data_list = [self._convert_to_draw_format(self.att_rssi_map[_ap_ssid], _ap_ssid)
                         for _ap_ssid in ap_ssid]
        else:
            file_name = "AttVsRSSI_{}.png".format(ap_ssid)
            data_list = [self._convert_to_draw_format(self.att_rssi_map[ap_ssid], ap_ssid)]
        LineChart.draw_line_chart(os.path.join(path, file_name),
                                  "Att Vs RSSI",
                                  "Att (dbm)",
                                  "RSSI (dbm)",
                                  data_list)
        return file_name

    def get_best_throughput(self):
        """  get the best throughput during test """
        best_for_aps = [max(self.throughput_by_att[ap_ssid].values())
                        for ap_ssid in self.throughput_by_att]
        return max(best_for_aps)

    def __str__(self):
        """
        returns summary for this test:

        1. test result (success or fail)
        2. best performance for each AP
        3. min free heap size during test
        """
        if self.throughput_by_att:
            ret = "[{}_{}][{}]: {}\r\n\r\n".format(self.proto, self.direction, self.config_name,
                                                   "Fail" if self.error_list else "Success")
            ret += "Performance for each AP:\r\n"
            for ap_ssid in self.throughput_by_att:
                ret += "[{}]: {:.02f} Mbps\r\n".format(ap_ssid, max(self.throughput_by_att[ap_ssid].values()))
            if self.heap_size != INVALID_HEAP_SIZE:
                ret += "Minimum heap size: {}".format(self.heap_size)
        else:
            ret = ""
        return ret


class IperfTestUtility(object):
    """ iperf test implementation """

    def __init__(self, dut, config_name, ap_ssid, ap_password,
                 pc_nic_ip, pc_iperf_log_file, test_result=None):
        self.config_name = config_name
        self.dut = dut

        self.pc_iperf_log_file = pc_iperf_log_file
        self.ap_ssid = ap_ssid
        self.ap_password = ap_password
        self.pc_nic_ip = pc_nic_ip

        if test_result:
            self.test_result = test_result
        else:
            self.test_result = {
                "tcp_tx": TestResult("tcp", "tx", config_name),
                "tcp_rx": TestResult("tcp", "rx", config_name),
                "udp_tx": TestResult("udp", "tx", config_name),
                "udp_rx": TestResult("udp", "rx", config_name),
            }

    def setup(self):
        """
        setup iperf test:

        1. kill current iperf process
        2. reboot DUT (currently iperf is not very robust, need to reboot DUT)
        3. scan to get AP RSSI
        4. connect to AP
        """
        try:
            subprocess.check_output("sudo killall iperf 2>&1 > /dev/null", shell=True)
        except subprocess.CalledProcessError:
            pass
        self.dut.write("restart")
        self.dut.expect_any("iperf>", "esp32>")
        self.dut.write("scan {}".format(self.ap_ssid))
        for _ in range(SCAN_RETRY_COUNT):
            try:
                rssi = int(self.dut.expect(re.compile(r"\[{}]\[rssi=(-\d+)]".format(self.ap_ssid)),
                                           timeout=SCAN_TIMEOUT)[0])
                break
            except DUT.ExpectTimeout:
                continue
        else:
            raise AssertionError("Failed to scan AP")
        self.dut.write("sta {} {}".format(self.ap_ssid, self.ap_password))
        dut_ip = self.dut.expect(re.compile(r"sta ip: ([\d.]+), mask: ([\d.]+), gw: ([\d.]+)"))[0]
        return dut_ip, rssi

    def _save_test_result(self, test_case, raw_data, att, rssi, heap_size):
        return self.test_result[test_case].add_result(raw_data, self.ap_ssid, att, rssi, heap_size)

    def _test_once(self, proto, direction):
        """ do measure once for one type """
        # connect and scan to get RSSI
        dut_ip, rssi = self.setup()

        assert direction in ["rx", "tx"]
        assert proto in ["tcp", "udp"]

        # run iperf test
        if direction == "tx":
            with open(PC_IPERF_TEMP_LOG_FILE, "w") as f:
                if proto == "tcp":
                    process = subprocess.Popen(["iperf", "-s", "-B", self.pc_nic_ip,
                                                "-t", str(TEST_TIME), "-i", "1", "-f", "m"],
                                               stdout=f, stderr=f)
                    self.dut.write("iperf -c {} -i 1 -t {}".format(self.pc_nic_ip, TEST_TIME))
                else:
                    process = subprocess.Popen(["iperf", "-s", "-u", "-B", self.pc_nic_ip,
                                                "-t", str(TEST_TIME), "-i", "1", "-f", "m"],
                                               stdout=f, stderr=f)
                    self.dut.write("iperf -c {} -u -i 1 -t {}".format(self.pc_nic_ip, TEST_TIME))

                for _ in range(TEST_TIMEOUT):
                    if process.poll() is not None:
                        break
                    time.sleep(1)
                else:
                    process.terminate()

            with open(PC_IPERF_TEMP_LOG_FILE, "r") as f:
                pc_raw_data = server_raw_data = f.read()
        else:
            with open(PC_IPERF_TEMP_LOG_FILE, "w") as f:
                if proto == "tcp":
                    self.dut.write("iperf -s -i 1 -t {}".format(TEST_TIME))
                    # wait until DUT TCP server created
                    try:
                        self.dut.expect("iperf tcp server create successfully", timeout=1)
                    except DUT.ExpectTimeout:
                        # compatible with old iperf example binary
                        pass
                    process = subprocess.Popen(["iperf", "-c", dut_ip,
                                                "-t", str(TEST_TIME), "-f", "m"],
                                               stdout=f, stderr=f)
                else:
                    self.dut.write("iperf -s -u -i 1 -t {}".format(TEST_TIME))
                    process = subprocess.Popen(["iperf", "-c", dut_ip, "-u", "-b", "100M",
                                                "-t", str(TEST_TIME), "-f", "m"],
                                               stdout=f, stderr=f)

                for _ in range(TEST_TIMEOUT):
                    if process.poll() is not None:
                        break
                    time.sleep(1)
                else:
                    process.terminate()

            server_raw_data = self.dut.read()
            with open(PC_IPERF_TEMP_LOG_FILE, "r") as f:
                pc_raw_data = f.read()

        # save PC iperf logs to console
        with open(self.pc_iperf_log_file, "a+") as f:
            f.write("## [{}] `{}`\r\n##### {}"
                    .format(self.config_name,
                            "{}_{}".format(proto, direction),
                            time.strftime("%m-%d %H:%M:%S", time.localtime(time.time()))))
            f.write('\r\n```\r\n\r\n' + pc_raw_data + '\r\n```\r\n')
        self.dut.write("heap")
        heap_size = self.dut.expect(re.compile(r"min heap size: (\d+)\D"))[0]

        # return server raw data (for parsing test results) and RSSI
        return server_raw_data, rssi, heap_size

    def run_test(self, proto, direction, atten_val):
        """
        run test for one type, with specified atten_value and save the test result

        :param proto: tcp or udp
        :param direction: tx or rx
        :param atten_val: attenuate value
        """
        rssi = FAILED_TO_SCAN_RSSI
        heap_size = INVALID_HEAP_SIZE
        try:
            server_raw_data, rssi, heap_size = self._test_once(proto, direction)
            throughput = self._save_test_result("{}_{}".format(proto, direction),
                                                server_raw_data, atten_val,
                                                rssi, heap_size)
            Utility.console_log("[{}][{}_{}][{}][{}]: {:.02f}"
                                .format(self.config_name, proto, direction, rssi, self.ap_ssid, throughput))
        except Exception as e:
            self._save_test_result("{}_{}".format(proto, direction), "", atten_val, rssi, heap_size)
            Utility.console_log("Failed during test: {}".format(e))

    def run_all_cases(self, atten_val):
        """
        run test for all types (udp_tx, udp_rx, tcp_tx, tcp_rx).

        :param atten_val: attenuate value
        """
        self.run_test("tcp", "tx", atten_val)
        self.run_test("tcp", "rx", atten_val)
        self.run_test("udp", "tx", atten_val)
        self.run_test("udp", "rx", atten_val)

    def wait_ap_power_on(self):
        """
        AP need to take sometime to power on. It changes for different APs.
        This method will scan to check if the AP powers on.

        :return: True or False
        """
        self.dut.write("restart")
        self.dut.expect_any("iperf>", "esp32>")
        for _ in range(WAIT_AP_POWER_ON_TIMEOUT // SCAN_TIMEOUT):
            try:
                self.dut.write("scan {}".format(self.ap_ssid))
                self.dut.expect(re.compile(r"\[{}]\[rssi=(-\d+)]".format(self.ap_ssid)),
                                timeout=SCAN_TIMEOUT)
                ret = True
                break
            except DUT.ExpectTimeout:
                pass
        else:
            ret = False
        return ret


@ttfw_idf.idf_example_test(env_tag="Example_ShieldBox_Basic", category="stress", target=['ESP32', 'ESP32S2'])
def test_wifi_throughput_with_different_configs(env, extra_data):
    """
    steps: |
      1. build iperf with specified configs
      2. test throughput for all routers
    """
    pc_nic_ip = env.get_pc_nic_info("pc_nic", "ipv4")["addr"]
    pc_iperf_log_file = os.path.join(env.log_path, "pc_iperf_log.md")
    ap_info = {
        "ssid": env.get_variable("ap_ssid"),
        "password": env.get_variable("ap_password"),
    }

    config_names_raw = subprocess.check_output(["ls", os.path.dirname(os.path.abspath(__file__))])
    config_names = CONFIG_NAME_PATTERN.findall(config_names_raw)
    if not config_names:
        raise ValueError("no configs found in {}".format(os.path.dirname(__file__)))

    test_result = dict()
    sdkconfig_files = dict()

    for config_name in config_names:
        # 1. get the config
        sdkconfig_files[config_name] = os.path.join(os.path.dirname(__file__),
                                                    "sdkconfig.ci.{}".format(config_name))

        # 2. get DUT and download
        dut = env.get_dut("iperf", "examples/wifi/iperf", dut_class=ttfw_idf.ESP32DUT,
                          app_config_name=config_name)
        dut.start_app()
        dut.expect_any("iperf>", "esp32>")

        # 3. run test for each required att value
        test_result[config_name] = {
            "tcp_tx": TestResult("tcp", "tx", config_name),
            "tcp_rx": TestResult("tcp", "rx", config_name),
            "udp_tx": TestResult("udp", "tx", config_name),
            "udp_rx": TestResult("udp", "rx", config_name),
        }

        test_utility = IperfTestUtility(dut, config_name, ap_info["ssid"],
                                        ap_info["password"], pc_nic_ip, pc_iperf_log_file, test_result[config_name])

        for _ in range(RETRY_COUNT_FOR_BEST_PERFORMANCE):
            test_utility.run_all_cases(0)

        for result_type in test_result[config_name]:
            summary = str(test_result[config_name][result_type])
            if summary:
                Utility.console_log(summary, color="orange")

        # 4. check test results
        env.close_dut("iperf")

    # 5. generate report
    report = TestReport.ThroughputForConfigsReport(os.path.join(env.log_path, "ThroughputForConfigsReport"),
                                                   ap_info["ssid"], test_result, sdkconfig_files)
    report.generate_report()


@ttfw_idf.idf_example_test(env_tag="Example_ShieldBox", category="stress", target=['ESP32', 'ESP32S2'])
def test_wifi_throughput_vs_rssi(env, extra_data):
    """
    steps: |
      1. build with best performance config
      2. switch on one router
      3. set attenuator value from 0-60 for each router
      4. test TCP tx rx and UDP tx rx throughput
    """
    att_port = env.get_variable("attenuator_port")
    ap_list = env.get_variable("ap_list")
    pc_nic_ip = env.get_pc_nic_info("pc_nic", "ipv4")["addr"]
    apc_ip = env.get_variable("apc_ip")
    pc_iperf_log_file = os.path.join(env.log_path, "pc_iperf_log.md")

    test_result = {
        "tcp_tx": TestResult("tcp", "tx", BEST_PERFORMANCE_CONFIG),
        "tcp_rx": TestResult("tcp", "rx", BEST_PERFORMANCE_CONFIG),
        "udp_tx": TestResult("udp", "tx", BEST_PERFORMANCE_CONFIG),
        "udp_rx": TestResult("udp", "rx", BEST_PERFORMANCE_CONFIG),
    }

    # 1. get DUT and download
    dut = env.get_dut("iperf", "examples/wifi/iperf", dut_class=ttfw_idf.ESP32DUT,
                      app_config_name=BEST_PERFORMANCE_CONFIG)
    dut.start_app()
    dut.expect_any("iperf>", "esp32>")

    # 2. run test for each required att value
    for ap_info in ap_list:
        test_utility = IperfTestUtility(dut, BEST_PERFORMANCE_CONFIG, ap_info["ssid"], ap_info["password"],
                                        pc_nic_ip, pc_iperf_log_file, test_result)

        PowerControl.Control.control_rest(apc_ip, ap_info["outlet"], "OFF")
        PowerControl.Control.control(apc_ip, {ap_info["outlet"]: "ON"})
        Attenuator.set_att(att_port, 0)

        if not test_utility.wait_ap_power_on():
            Utility.console_log("[{}] failed to power on, skip testing this AP"
                                .format(ap_info["ssid"]), color="red")
            continue

        for atten_val in ATTEN_VALUE_LIST:
            assert Attenuator.set_att(att_port, atten_val) is True
            test_utility.run_all_cases(atten_val)

    # 3. check test results
    env.close_dut("iperf")

    # 4. generate report
    report = TestReport.ThroughputVsRssiReport(os.path.join(env.log_path, "ThroughputVsRssiReport"),
                                               test_result)
    report.generate_report()


@ttfw_idf.idf_example_test(env_tag="Example_ShieldBox_Basic")
def test_wifi_throughput_basic(env, extra_data):
    """
    steps: |
      1. test TCP tx rx and UDP tx rx throughput
      2. compare with the pre-defined pass standard
    """
    pc_nic_ip = env.get_pc_nic_info("pc_nic", "ipv4")["addr"]
    pc_iperf_log_file = os.path.join(env.log_path, "pc_iperf_log.md")
    ap_info = {
        "ssid": env.get_variable("ap_ssid"),
        "password": env.get_variable("ap_password"),
    }

    # 1. get DUT
    dut = env.get_dut("iperf", "examples/wifi/iperf", dut_class=ttfw_idf.ESP32DUT,
                      app_config_name=BEST_PERFORMANCE_CONFIG)
    dut.start_app()
    dut.expect_any("iperf>", "esp32>")

    # 2. preparing
    test_result = {
        "tcp_tx": TestResult("tcp", "tx", BEST_PERFORMANCE_CONFIG),
        "tcp_rx": TestResult("tcp", "rx", BEST_PERFORMANCE_CONFIG),
        "udp_tx": TestResult("udp", "tx", BEST_PERFORMANCE_CONFIG),
        "udp_rx": TestResult("udp", "rx", BEST_PERFORMANCE_CONFIG),
    }

    test_utility = IperfTestUtility(dut, BEST_PERFORMANCE_CONFIG, ap_info["ssid"],
                                    ap_info["password"], pc_nic_ip, pc_iperf_log_file, test_result)

    # 3. run test for TCP Tx, Rx and UDP Tx, Rx
    for _ in range(RETRY_COUNT_FOR_BEST_PERFORMANCE):
        test_utility.run_all_cases(0)

    # 4. log performance and compare with pass standard
    performance_items = []
    for throughput_type in test_result:
        ttfw_idf.log_performance("{}_throughput".format(throughput_type),
                                 "{:.02f} Mbps".format(test_result[throughput_type].get_best_throughput()))
        performance_items.append(["{}_throughput".format(throughput_type),
                                  "{:.02f} Mbps".format(test_result[throughput_type].get_best_throughput())])

    # 5. save to report
    TinyFW.JunitReport.update_performance(performance_items)
    # do check after logging, otherwise test will exit immediately if check fail, some performance can't be logged.
    for throughput_type in test_result:
        ttfw_idf.check_performance("{}_throughput".format(throughput_type),
                                   test_result[throughput_type].get_best_throughput(), dut.TARGET)

    env.close_dut("iperf")


if __name__ == '__main__':
    test_wifi_throughput_basic(env_config_file="EnvConfig.yml")
    test_wifi_throughput_with_different_configs(env_config_file="EnvConfig.yml")
    test_wifi_throughput_vs_rssi(env_config_file="EnvConfig.yml")
