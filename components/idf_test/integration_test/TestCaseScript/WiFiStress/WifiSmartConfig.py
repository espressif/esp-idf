import random
import os
import time
import copy

from TCAction import TCActionBase
from NativeLog import NativeLog
from Utility import Encoding
from Utility import MakeFolder

AP_PROP = ("ssid", "ssid_len", "pwd",
           "pwd_len", "channel", "enc", "apc")

SMART_TYPE = ("esp-touch", "airkiss")

TEST_METHOD = ("ssid_broadcast", "ssid_hidden")

HT = ("ht20", "ht40")

TEST_STAT = ("total count", "fail count", "total time", "longest time")

_TEST_STAT_INIT_DICT = {"total count": 0,
                        "fail count": 0,
                        "total time": 0,
                        "longest time": 0,
                        }

LOG_FOLDER = os.path.join("Performance", "SmartConfig")


SSID_LEN_RANGE = (1, 32)  # in bytes
ENC_TYPE = (0, 2, 3, 4)  # do not support WEP for 8266 soft AP
PWD_RANGE = {0: [0, 0],
             1: [5, 5],
             2: [8, 32],
             3: [8, 32],
             4: [8, 32],
             }


class TestCase(TCActionBase.CommonTCActionBase):

    def __init__(self, test_case, test_env, timeout=30, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, test_case, test_env, timeout=timeout, log_path=log_path)
        self.performance_folder_path = log_path
        # default value for optional configurable params
        self.test_method = ["ssid_hidden", "ssid_broadcast"]
        self.bssid = "ff:ff:ff:ff:ff:ff"
        self.ht_ap = dict(zip(HT, [("<ht20_ap_ssid>", "<ht20_ap_password>"),
                                   ("<ht40_ap_ssid>", "<ht40_ap_password>")]))
        self.ap_channel = {"ht20": 1, "ht40": 1}
        self.delay_time = 3  # default 3s, wait for scan done
        # load param from excel
        cmd_set = test_case["cmd set"]
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy" and cmd_set[i][0] != "":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string

        folder_path = MakeFolder.make_folder(self.performance_folder_path + "\\" + LOG_FOLDER)
        file_name = "SmartConfig_log_%s.log" % (time.strftime("%m%d%H%M%S", time.localtime()))
        self._performance_log_file = os.path.join(folder_path, file_name)

        # type
        self.target_type = ["SSC" if test_env.get_port_by_name("AT1") is None else "AT"]
        self.target_type.append("SSC" if test_env.get_port_by_name("AT2") is None else "AT")

        # test statistics
        # better ways to create?
        _test_stat = dict.fromkeys(TEST_STAT, 0)
        _test_method = dict.fromkeys(TEST_METHOD)
        _test_ht = dict.fromkeys(HT)
        self.test_stat = dict.fromkeys(SMART_TYPE)
        for i in SMART_TYPE:
            self.test_stat[i] = copy.deepcopy(_test_ht)
            for j in HT:
                self.test_stat[i][j] = copy.deepcopy(_test_method)
                for k in TEST_METHOD:
                    self.test_stat[i][j][k] = copy.deepcopy(_test_stat)

        self.result_cntx = TCActionBase.ResultCheckContext(self, test_env, self.tc_name)
        pass

    def _generate_random_ap_prop(self, ht_type):
        ap_prop = dict.fromkeys(AP_PROP)
        # generate target ap_value
        ap_prop["ssid_len"] = random.randint(SSID_LEN_RANGE[0], SSID_LEN_RANGE[1])
        ap_prop["channel"] = self.ap_channel[ht_type]
        ap_prop["enc"] = random.choice(ENC_TYPE)
        ap_prop["pwd_len"] = random.randint(PWD_RANGE[ap_prop["enc"]][0], PWD_RANGE[ap_prop["enc"]][1])
        ap_prop["ssid"] = Encoding.generate_random_printable_str(ap_prop["ssid_len"])
        ap_prop["pwd"] = Encoding.generate_random_printable_str(ap_prop["pwd_len"])

        return ap_prop

    def _logging_performance(self, time_cost, ssid, password, smart_type, test_method, ht_type):
        # update test statistics
        stat = self.test_stat[smart_type][ht_type][test_method]
        stat["total count"] += 1
        # log performance to performance log file
        with open(self._performance_log_file, "ab+") as f:
            # log time and ssid
            if time_cost is not False:
                time_tmp = float(time_cost)/10
                f.write("\r\n[%s]:\r\n[Succeed] [%.2f]\r\n" %
                        (time.strftime("%m-%d %H:%M:%S", time.localtime()), time_tmp))
                stat["total time"] += time_tmp
                stat["longest time"] = time_tmp if time_tmp > stat["longest time"] else stat["longest time"]
            else:
                f.write("\r\n[%s]:\r\n[Fail]\r\n" %
                        time.strftime("%m-%d %H:%M:%S", time.localtime()))
                stat["fail count"] += 1

            f.write("[%s] [%s] [%s]\r\n" %
                    (smart_type, test_method, ht_type))
            f.write("[ssid] %s \r\n[password] %s\r\n" %
                    (ssid, password))
        pass

    def _generate_performance_report(self):
        with open(self._performance_log_file, "ab+") as f:
            for i in SMART_TYPE:
                for j in HT:
                    for k in TEST_METHOD:
                        stat = self.test_stat[i][j][k]
                        f.write("\r\n[Test report] [%s] [%s] [%s]\r\n" % (i, j, k))
                        if stat["total count"] > 0:
                            f.write("[Total]: %d\r\n" % stat["total count"])
                            f.write("[Failed]: %d\r\n" % stat["fail count"])
                            f.write("[Fail ratio]: %.2f%%\r\n" %
                                    (float(stat["fail count"])/stat["total count"] * 100))
                            f.write("[Longest time cost]: %.2f\r\n" % stat["longest time"])
                            if (stat["total count"] - stat["fail count"]) > 0:
                                f.write("[Average time cost]: %.2f\r\n" %
                                        (stat["total time"]/(stat["total count"]-stat["fail count"])))

    @staticmethod
    def cmd_exception_catcher(e):
        raise e
        pass

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        self.result_cntx.start()

        # mandatory configurable params
        try:
            test_count = self.test_count
            delay_time = self.delay_time
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for WifiJAP script, error is %s" % e)
            raise StandardError("Error configuration")

        # step 0 : set AT1 mode
        fail_string = "Fail to restore init condition"
        if self.target_type[0] == "AT":
            cmd = ["ATS AT1 AT+CWMODE=1"]
            checker_stings = ["R AT1 L OK"]
        else:
            cmd = ["SSC SSC1 op -S -o 1"]
            checker_stings = ["R SSC1 C +MODE:OK"]
        if self.target_type[1] == "AT":
            cmd.append("ATS AT2 AT+CWMODE=2")
            checker_stings.append("R AT2 L OK")
        else:
            cmd.append("SSC SSC2 op -S -o 2")
            checker_stings.append("R SSC2 C +MODE:OK")

        if self.load_and_exe_one_step(checker_stings, cmd,
                                      fail_string, check_time=150) is False:
            NativeLog.add_trace_critical(fail_string)
            return

        for i in xrange(test_count):
            _method = random.choice(self.test_method)
            _ht = random.choice(self.ht)
            _ap_prop = self._generate_random_ap_prop(_ht)
            _smart_type = random.choice(self.smart_type)
            _ht_ap = self.ht_ap[_ht]
            is_hidden = 0 if _method == "ssid_broadcast" else 1
            # get ip and

            # step 1 : restore init condition
            fail_string = "Fail to restore init condition"
            if self.target_type[0] == "AT":
                cmd = ["ATS AT1 AT+CWSTOPSMART", "WIFI <pc_wifi_nic> CONN %s %s <pc_ip_sc>" % (_ht_ap[0], _ht_ap[1])]
                checker_stings = ["P AT1 L OK", "P PC_COM L OK"]
            else:
                cmd = ["SSC SSC1 smart -E", "WIFI <pc_wifi_nic> CONN %s %s <pc_ip_sc>" % (_ht_ap[0], _ht_ap[1])]
                checker_stings = ["P SSC1 C +SC:OK", "P PC_COM L OK"]

            if self.load_and_exe_one_step(checker_stings, cmd,
                                          fail_string, check_time=200) is False:
                NativeLog.add_trace_critical(fail_string)
                continue
            NativeLog.add_prompt_trace("Step1 Done")

            # step 2 : test method is ssid_broadcast, then set AP on target 2
            if _method == "ssid_broadcast":
                fail_string = "Fail to set AP"
                if self.target_type[1] == "AT":
                    cmd = ["ATS AT2 AT+CWSAP=\"%s\",\"%s\",%d,%d" % (_ap_prop["ssid"], _ap_prop["pwd"],
                                                                     _ap_prop["channel"], _ap_prop["enc"])]
                    checker_stings = ["R AT2 L OK"]
                else:
                    cmd = ["SSC SSC2 ap -S -s %s -p %s -n %d -t %d" % (_ap_prop["ssid"], _ap_prop["pwd"],
                                                                       _ap_prop["channel"], _ap_prop["enc"])]
                    checker_stings = ["R SSC2 C +SAP:OK"]

                if self.load_and_exe_one_step(checker_stings, cmd,
                                              fail_string, check_time=50) is False:
                    NativeLog.add_trace_critical(fail_string)
                    continue
            NativeLog.add_prompt_trace("Step2 Done")

            # step 3 : start SMART
            fail_string = "Fail to start smart config"
            if self.target_type[0] == "AT":
                cmd = ["ATS AT1 AT+CWSTARTSMART"]
                checker_stings = ["R AT1 L OK"]
            else:
                cmd = ["SSC SSC1 smart -S -a 0"]
                checker_stings = ["R SSC1 C +SC:OK"]

            if self.load_and_exe_one_step(checker_stings, cmd,
                                          fail_string, check_time=50) is False:
                NativeLog.add_trace_critical(fail_string)
                continue
            # sleep for delay_time seconds to wait scan done or simulate delay config situation
            time.sleep(delay_time)
            NativeLog.add_prompt_trace("Step3 Done")

            # step 4 : do smart config
            fail_string = "Fail in smart config"
            cmd = ["SMART %s <pc_ip_sc> %s %s %s %d"
                   % (_smart_type, _ap_prop["ssid"], _ap_prop["pwd"], self.bssid, is_hidden)]
            if self.target_type[0] == "AT":
                checker_stings = ["P AT1 C Smart%20get%20wifi%20info",
                                  "P LOG1 C %s C %s" % (_ap_prop["ssid"], _ap_prop["pwd"])]
            else:
                checker_stings = ["P SSC1 C %s C %s" % (_ap_prop["ssid"], _ap_prop["pwd"])]

            try:
                time_cost = self.load_and_exe_one_step(checker_stings, cmd,
                                                       fail_string, check_time=400,
                                                       cmd_exception_catcher=self.cmd_exception_catcher)
            except StandardError:
                NativeLog.add_prompt_trace("Exception occurred during executing cmd")
                continue
                pass
            self._logging_performance(time_cost, _ap_prop["ssid"], _ap_prop["pwd"],
                                      _smart_type, _method, _ht)
            if time_cost is False:
                NativeLog.add_prompt_trace(fail_string)
                continue

            # continue to next loop
            NativeLog.add_prompt_trace("[WifiSmartConfig] Test count %d done" % i)

        # generate report and cleanup
        self._generate_performance_report()

        self.result_cntx.set_result("Succeed")

    def result_check(self, port_name, data):
        TCActionBase.CommonTCActionBase.result_check(self, port_name, data)
        self.result_cntx.append_data(port_name, data)


def main():
    pass

if __name__ == '__main__':
    main()
