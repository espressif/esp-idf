import os
import time
from TCAction import TCActionBase
from NativeLog import NativeLog
from Utility import RSSICalibrator
from Utility import MakeFolder

MAX_RSSI = 0
MIN_RSSI = -110
MAX_ATT = 60
LOG_FOLDER = os.path.join("Performance", "JAP")
AP_PROP_KEY = ("ssid", "password", "apc")


class Performance(object):
    RSSI_THRESHOLD = -90

    def __init__(self):
        self.succeed_rssi = dict.fromkeys(range(MIN_RSSI, MAX_RSSI), 0)
        self.failed_rssi = dict.fromkeys(range(MIN_RSSI, MAX_RSSI), 0)
        self.failed_att = dict.fromkeys(range(MAX_ATT), 0)
        self.result = True
        pass

    def log_performance(self, result, att, rssi):
        if result == "Succeed":
            self.succeed_rssi[rssi] += 1
        else:
            if rssi == 0:
                self.failed_att[att] += 1
            else:
                self.failed_rssi[rssi] += 1
                if rssi > self.RSSI_THRESHOLD:
                    self.result = False
        pass

    def get_result(self):
        return self.result

    def __str__(self):
        data = "Succeed:\r\n"
        for rssi in self.succeed_rssi:
            if self.succeed_rssi[rssi] > 0:
                data += "\tRSSI%4d: %2d times\r\n" % (rssi, self.succeed_rssi[rssi])

        data += "Failed during scan:\r\n"
        for att in self.failed_att:
            if self.failed_att[att] > 0:
                data += "\tATT%3d: %2d times\r\n" % (att, self.failed_att[att])

        data += "Failed during JAP:\r\n"
        for rssi in self.failed_rssi:
            if self.failed_rssi[rssi] > 0:
                data += "\tRSSI%4d: %2d times\r\n" % (rssi, self.failed_rssi[rssi])

        return data

    pass


class TestCase(TCActionBase.CommonTCActionBase):
    def __init__(self, test_case, test_env, timeout=30, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, test_case, test_env, timeout=timeout, log_path=log_path)
        self.att_test_list = range(60)
        self.performance_folder_path = log_path
        # load param from excel
        cmd_set = test_case["cmd set"]
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string

        self.ap_list = self.get_parameter("shield_box_ap_list")

        self.performance = dict([(ap_prop["ssid"], Performance()) for ap_prop in self.ap_list])
        # create log file
        folder_path = MakeFolder.make_folder(self.performance_folder_path + "\\" + LOG_FOLDER)
        self.performance_log = os.path.join(folder_path,
                                            "JAP_Att_%s.log" % time.strftime("%d%H%M%S", time.localtime()))

        self.result_cntx = TCActionBase.ResultCheckContext(self, test_env, self.tc_name)
        pass

    def log_performance(self, att, rssi, ssid, result):
        NativeLog.add_prompt_trace("[%s][ssid %s] [att %s] [rssi %s]" % (result, ssid, att, rssi))
        data = ""
        self.performance[ssid].log_performance(result, att, rssi)
        for _ssid in self.performance:
            data += "[ssid] %s\r\n%s\r\n" % (_ssid, self.performance[_ssid])
        with open(self.performance_log, "wb+") as f:
            f.write(data)
        pass

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        self.result_cntx.start()

        try:
            # configurable params
            ap_list = self.ap_list
            att_test_list = self.att_test_list
            test_count = self.test_count
            # configurable params
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for JAPAtt script, error is %s" % e)
            raise StandardError("Error configuration")

        for x in xrange(test_count):
            for ap_prop in ap_list:
                ssid = ap_prop[0]
                password = ap_prop[1]
                apc = ap_prop[2]
                if ap_prop[1] == "":
                    # set a default string for open ap
                    password = "1"

                # switch off all outlet, switch on AP outlet
                outlet_config_dict = dict.fromkeys(range(1, 9), "OFF")
                outlet_config_dict[apc] = "ON"
                apc_cmd = "APC <APC1>"
                for outlet in outlet_config_dict:
                    apc_cmd += " %s %s" % (outlet_config_dict[outlet], outlet)
                checker_stings = ["P PC_COM L OK"]
                fail_string = "Fail, Fail to switch apc"
                if self.load_and_exe_one_step(checker_stings, [apc_cmd], fail_string) is False:
                    return

                # wait AP ready
                time.sleep(20)

                # create RSSI Calibrator
                calibrator = RSSICalibrator.Calibrator()

                ret = True
                for att_value in att_test_list:
                    # step 0 set att value
                    checker_stings = ["R PC_COM L OK"]
                    test_action_string = ["ATT <att_port> %s" % att_value]
                    fail_string = "Fail, Fail to set att value"
                    if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                        continue

                    # step 1 get AP RSSI
                    checker_stings = ["R SSC1 A <rssi>:\+SCAN:%s,[:\d\w]+,\d+,\d+,([-\d]+)" % ssid]
                    test_action_string = ["SSC SSC1 sta -S -s %s" % ssid]
                    fail_string = "Fail, Fail to scan"
                    rssi = scan_count = 0
                    for i in range(3):
                        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string,
                                                      check_freq=1, check_time=5) is False:
                            self.log_performance(att_value, 0, ssid, "Failed to measure RSSI")
                            continue
                        rssi += int(self.test_env.get_variable_by_name("rssi")[1])
                        scan_count += 1

                    rssi = calibrator.calibrate_rssi(float(rssi) / scan_count if scan_count > 0 else 0, att_value)

                    # step 2 connect to AP
                    checker_stings = ["R SSC1 C +JAP:CONNECTED"]
                    test_action_string = ["SSC SSC1 sta -C -s %s -p %s" % (ssid, password)]
                    fail_string = "Fail, Fail to JAP"
                    if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string,
                                                  check_freq=1, check_time=45) is False:
                        self.log_performance(att_value, rssi, ssid, "Failed to JAP")
                        if rssi < -90:
                            continue
                        else:
                            ret = False
                            break
                if ret:
                    self.log_performance(att_value, rssi, ssid, "Succeed")
                else:
                    self.log_performance(att_value, rssi, ssid, "Failed")

        # finally, execute done
        for ssid in self.performance:
            if self.performance[ssid].get_result() is False:
                self.result_cntx.set_result("Failed")
                break
        else:
            self.result_cntx.set_result("Succeed")

    def result_check(self, port_name, data):
        TCActionBase.CommonTCActionBase.result_check(self, port_name, data)
        self.result_cntx.append_data(port_name, data)


def main():
    pass


if __name__ == '__main__':
    main()
