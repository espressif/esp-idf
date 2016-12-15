import os
import random
import time

import WifiConnUtility
from NativeLog import NativeLog
from TCAction import TCActionBase
from Utility import Encoding
from Utility import MakeFolder

STEPS = {"SCAN1": 0x01, "JAP": 0x02, "SCAN2": 0x04, "RECONNECT": 0x08}

AP_PROP = ("ssid", "ssid_len", "pwd",
           "pwd_len", "channel", "enc", "apc")

JAP_TEST_METHOD = ("Normal", "OFF_ON", "OFF", "WRONG_PROP")

RECONNECT_TEST_METHOD = ("OFF_ON", "OFF")

LOG_FOLDER = os.path.join("Performance", "JAP")


SSID_LEN_RANGE = (1, 32)  # in bytes
ENC_TYPE = (0, 2, 3, 4)  # do not support WEP for 8266 soft AP
PWD_RANGE = {0: [0, 0],
             1: [5, 5],
             2: [8, 63],
             3: [8, 63],
             4: [8, 63],
             }


class TestCase(TCActionBase.CommonTCActionBase):

    def __init__(self, test_case, test_env, timeout=30, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, test_case, test_env, timeout=timeout, log_path=log_path)
        # default value for optional configurable params
        self.performance_folder_path = log_path
        self.pwd_len = [8, 64]
        self.step_config = [0x03, 0x01, 0x02, 0x0B, 0x0F]
        self.join_test_method = ["Normal"]
        self.join_delay = [[1.5, 5], [1.5, 5]]
        self.reconnect_test_method = ["OFF_ON"]
        self.reconnect_delay = [[1.5, 5], [1.5, 6]]
        # load param from excel
        cmd_set = test_case["cmd set"]
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy" and cmd_set[i][0] != "":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        # read AP list
        self.ap_list = []
        for i in range(1, len(cmd_set)):
            for j in range(len(cmd_set[i][1])):
                if cmd_set[i][1][j] != "":
                    cmd_string = "self.ap_list.append(dict(zip(AP_PROP, " + cmd_set[i][1][j] + ")))"
                    exec cmd_string

        folder_path = MakeFolder.make_folder(self.performance_folder_path + "\\" + LOG_FOLDER)
        file_name = "JAP_log_%s.log" % (time.strftime("%m%d%H%M%S", time.localtime()))
        self._performance_log_file = os.path.join(folder_path, file_name)

        # test statistics
        self._succeed_count = self._fail_count = self._time_cost_count = 0
        self._total_time = self._longest_time = 0

        self.result_cntx = TCActionBase.ResultCheckContext(self, test_env, self.tc_name)
        # get target type "SSC" or "AT"
        self.target_type = ["SSC" if test_env.get_port_by_name("AT1") is None else "AT"]
        self.target_type.append("SSC" if test_env.get_port_by_name("AT2") is None else "AT")
        self._utility = WifiConnUtility.WifiConnUtility(self)
        pass

    def _generate_random_ap_prop(self):
        ap_prop = dict.fromkeys(AP_PROP)
        # generate target ap_value
        ap_prop["ssid_len"] = random.randint(SSID_LEN_RANGE[0], SSID_LEN_RANGE[1])
        ap_prop["channel"] = random.choice(range(1, 14))
        ap_prop["enc"] = random.choice(ENC_TYPE)
        ap_prop["pwd_len"] = random.randint(PWD_RANGE[ap_prop["enc"]][0], PWD_RANGE[ap_prop["enc"]][1])
        # generate string
        if self.target_type[0] == self.target_type[1] == "AT":
            ap_prop["ssid"] = Encoding.generate_random_utf8_str(ap_prop["ssid_len"])
            ap_prop["pwd"] = Encoding.generate_random_utf8_str(ap_prop["pwd_len"])
            # NativeLog.add_trace_info("ssid hex is : %x" % ap_prop["ssid"])
            # NativeLog.add_trace_info("pwd hex is : %x" % ap_prop["pwd"])
        else:
            ap_prop["ssid"] = Encoding.generate_random_printable_str(ap_prop["ssid_len"])
            ap_prop["pwd"] = Encoding.generate_random_printable_str(ap_prop["pwd_len"])

        return ap_prop

    def _logging_performance(self, ssid, join_method="Normal", time_cost=0):
        # log performance to performance log file
        with open(self._performance_log_file, "ab+") as f:
            # log time and ssid
            f.write("\r\n[%s]:\r\n[AP name] %s\r\n" %
                    (time.strftime("%m-%d %H:%M:%S", time.localtime()), ssid))
            if join_method == "Normal" or join_method == "OFF_ON":
                if time_cost is not False:
                    self._succeed_count += 1
                    if join_method == "Normal":
                        f.write("[Succeed][%f]\r\n" % time_cost)
                        self._longest_time = (time_cost > self._longest_time and
                                              [time_cost] or [self._longest_time])[0]
                        self._time_cost_count += 1
                        self._total_time += time_cost
                    else:
                        f.write("[Succeed][%s]\r\n" % join_method)
                else:
                    self._fail_count += 1
                    f.write("[Fail][%s]\r\n" % join_method)
        pass

    def _logging_fail_step(self, ssid, step):
        with open(self._performance_log_file, "ab+") as f:
            f.write("\r\n[%s]:\r\n[AP name] %s\r\n" %
                    (time.strftime("%m-%d %H:%M:%S", time.localtime()), ssid))
            f.write("[Fail][%s]\r\n" % step)
        pass

    def _generate_performance_report(self):
        with open(self._performance_log_file, "ab+") as f:
            f.write("[Test report] Succeed: %d\r\n" % self._succeed_count)
            f.write("[Test report] Failed: %d\r\n" % self._fail_count)
            if self._succeed_count > 0 or self._fail_count > 0:
                f.write("[Test report] Pass Rate: %f\r\n" %
                        (self._succeed_count/(self._fail_count+self._succeed_count)))
            if self._time_cost_count > 0:
                f.write("[Test report] Average time: %f\r\n" % (self._total_time/self._time_cost_count))
                f.write("[Test report] Longest time: %f\r\n" % self._longest_time)

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        self.result_cntx.start()

        # mandatory configurable params
        try:
            target_ap_num = self.target_ap_num
            test_count = self.test_count
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for WifiJAP script, error is %s" % e)
            raise StandardError("Error configuration")

        # prepare ap list
        _ap_list = [["target", None]] * target_ap_num
        for _ap_prop in self.ap_list:
            _ap_list.append(["AP", _ap_prop])

        # set to correct mode first
        self._utility.set_mode([1, 2])

        for i in xrange(test_count):
            _ap = random.choice(_ap_list)
            # arrange ap
            _ap_type = _ap[0]
            _ap_prop = _ap[1]
            if _ap_type == "target":
                _ap_prop = self._generate_random_ap_prop()
                pass

            # step 1 : mandatory step, set up AP
            if self._utility.setup_ap(_ap_type, _ap_prop) is False:
                self._logging_fail_step(_ap_prop["ssid"], "Set AP")
                NativeLog.add_prompt_trace("[Step1] setup AP Fail")
                continue
            step_config = random.choice(self.step_config)
            NativeLog.add_prompt_trace("[Step1] setup AP succeed")

            # step 2 : optional step, do scan before connect
            if step_config & STEPS["SCAN1"] != 0:  # check option
                if self._utility.do_scan(_ap_prop) is False:
                    self._logging_fail_step(_ap_prop["ssid"], "Scan before JAP")
                NativeLog.add_prompt_trace("[Step2] Scan Done")

            # step 3 : mandatory step, join AP
            if step_config & STEPS["JAP"] != 0:  # check option
                _join_test_method = random.choice(self.join_test_method)
                time_cost = self._utility.join_ap(_join_test_method, _ap_type, _ap_prop, self.join_delay)
                # log performance to performance log file
                self._logging_performance(_ap_prop["ssid"], _join_test_method, time_cost)
                if time_cost is False:
                    # do scan once to check if AP exist
                    self._utility.do_scan(_ap_prop)
                    continue
                NativeLog.add_prompt_trace("[Step3] Join AP done")

            # step 4 : optional step, scan after join AP
            if step_config & STEPS["SCAN2"] != 0:  # check option
                if self._utility.do_scan(_ap_prop) is False:
                    self._logging_fail_step(_ap_prop["ssid"], "Scan after JAP")
                NativeLog.add_prompt_trace("[Step4] Scan done")

            # step 5 : optional step, reconnect test
            if step_config & STEPS["RECONNECT"] != 0:  # check option
                _reconnect_test_method = random.choice(self.reconnect_test_method)
                if self._utility.do_reconnect(_reconnect_test_method,
                                              _ap_type, _ap_prop, self.reconnect_delay) is False:
                    self._logging_fail_step(_ap_prop["ssid"], "Reconnect")

                NativeLog.add_prompt_trace("[Step5] Reconnect done")
            # continue to next loop
            NativeLog.add_prompt_trace("[WifiJAP] Test count %d done" % i)

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
