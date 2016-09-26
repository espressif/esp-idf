import os
import time
from TCAction import TCActionBase
from NativeLog import NativeLog
from Utility import RSSICalibrator
from Utility import MakeFolder

MAX_RSSI = 0
MIN_RSSI = -110
MAX_ATT = 60
LOG_FOLDER = os.path.join("AT_LOG", "Performance", "JAP")
AP_PROP_KEY = ("ssid", "password", "apc")


class Performance(object):
    def __init__(self):
        self.succeed_rssi = dict.fromkeys(range(MIN_RSSI, MAX_RSSI), 0)
        self.failed_rssi = dict.fromkeys(range(MIN_RSSI, MAX_RSSI), 0)
        self.failed_att = dict.fromkeys(range(MAX_ATT), 0)
        pass

    def log_performance(self, result, att, rssi):
        if result == "Succeed":
            self.succeed_rssi[rssi] += 1
        else:
            if rssi == 0:
                self.failed_att[att] += 1
            else:
                self.failed_rssi[rssi] += 1
        pass

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


class WifiJAPAtt(TCActionBase.CommonTCActionBase):

    def __init__(self, name, test_env, cmd_set, timeout=30, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                 timeout=timeout, log_path=log_path)
        self.att_test_list = range(60)
        # load param from excel
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        # read AP list
        self.ap_list = []
        for i in range(1, len(cmd_set)):
            for j in range(len(cmd_set[i][1])):
                if cmd_set[i][1][j] != "":
                    cmd_string = "self.ap_list.append(dict(zip(AP_PROP_KEY, " + cmd_set[i][1][j] + ")))"
                    exec cmd_string

        self.performance = dict([(ap_prop["ssid"], Performance()) for ap_prop in self.ap_list])
        # create log file
        folder_path = MakeFolder.make_folder(LOG_FOLDER)
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
                if ap_prop["password"] == "":
                    # set a default string for open ap
                    ap_prop["password"] = "1"

                # switch off all outlet, switch on AP outlet
                outlet_config_dict = dict.fromkeys(range(1, 9), "OFF")
                outlet_config_dict[ap_prop["apc"]] = "ON"
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

                for att_value in att_test_list:
                    # step 0 set att value
                    checker_stings = ["R PC_COM L OK"]
                    test_action_string = ["ATT <att_port> %s" % att_value]
                    fail_string = "Fail, Fail to set att value"
                    if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                        continue

                    # step 1 get AP RSSI
                    checker_stings = ["R SSC1 A <rssi>:\+SCAN:%s,[:\d\w]+,\d+,\d+,([-\d]+)" % ap_prop["ssid"]]
                    test_action_string = ["SSC SSC1 sta -S -s %s" % ap_prop["ssid"]]
                    fail_string = "Fail, Fail to scan"
                    rssi = scan_count = 0
                    for i in range(3):
                        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string,
                                                      check_freq=1, check_time=5) is False:
                            self.log_performance(att_value, 0, ap_prop["ssid"], "Failed to measure RSSI")
                            continue
                        rssi += int(self.test_env.get_variable_by_name("rssi")[1])
                        scan_count += 1

                    rssi = calibrator.calibrate_rssi(float(rssi)/scan_count if scan_count > 0 else 0, att_value)

                    # step 2 connect to AP
                    checker_stings = ["R SSC1 C +JAP:CONNECTED"]
                    test_action_string = ["SSC SSC1 sta -C -s %s -p %s" % (ap_prop["ssid"], ap_prop["password"])]
                    fail_string = "Fail, Fail to JAP"
                    if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string,
                                                  check_freq=1, check_time=45) is False:
                        self.log_performance(att_value, rssi, ap_prop["ssid"], "Failed to JAP")
                        continue

                    self.log_performance(att_value, rssi, ap_prop["ssid"], "Succeed")

        # finally, execute done
        self.result_cntx.set_result("Succeed")

    def result_check(self, port_name, data):
        TCActionBase.CommonTCActionBase.result_check(self, port_name, data)
        self.result_cntx.append_data(port_name, data)


def main():
    pass

if __name__ == '__main__':
    main()
