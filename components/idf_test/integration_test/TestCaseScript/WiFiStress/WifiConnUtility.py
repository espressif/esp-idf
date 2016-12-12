from NativeLog import NativeLog
import time
import random


ERROR_AP_PROP = {"ssid": "123456789012345678901234567890",
                 "ssid_len": 30,
                 "pwd": "12345678901234567890",
                 "pwd_len": 20,
                 "channel": 10,
                 "enc": 3,
                 "apc": 9,  # invalid apc count
                 }


class WifiConnUtilError(StandardError):
    pass


class WifiConnUtility(object):

    def __init__(self, tc_action):
        self.tc_action = tc_action
        self.target_type = tc_action.target_type
        pass

    def set_mode(self, mode):
        ret = True
        fail_string = "set mode fail"
        cmd = []
        checker_stings = []
        for i in range(2):
            if self.target_type[0] == "SSC":
                cmd.append("SSCC SSC%d op -S -o %d" % (i+1, mode[i]))
                checker_stings.append("SSCP SSC%d C +MODE:OK" % (i+1))
                pass
            else:
                cmd.append("ATC AT%d CWMODE %d" % (i+1, mode[i]))
                checker_stings.append("ATP AT%d L OK" % (i+1))
                pass
        if self.tc_action.load_and_exe_one_step(checker_stings, cmd,
                                                fail_string, check_time=50) is False:
            NativeLog.add_trace_critical("Failed to set mode")
            ret = False
        return ret
        pass

    def _apc_switch(self, outlet_list, action_list):
        checker_stings = ["R PC_COM C OK"]
        switch_cmd = "APC <APC1>"
        fail_string = "Error when switching APC"
        ret = True

        for [_outlet, _action] in zip(action_list, outlet_list):
            switch_cmd += " %s %d" % (_action, _outlet)

        if self.tc_action.load_and_exe_one_step(checker_stings, [switch_cmd],
                                                fail_string, check_time=50) is False:
            NativeLog.add_trace_critical("Error when switching APC")
            ret = False
        return ret
        pass

    def _set_target_ap(self, ap_prop):
        ret = True
        fail_string = "set target ap fail, %s, %s" % (ap_prop["ssid"], ap_prop["pwd"])
        if self.target_type[1] == "SSC":
            if ap_prop["pwd"] == "":
                cmd = ["SSCC SSC2 ap -S -s %s -t %d" % (ap_prop["ssid"],
                                                        ap_prop["enc"])
                       ]
            else:
                cmd = ["SSCC SSC2 ap -S -s %s -p %s -t %d" % (ap_prop["ssid"],
                                                              ap_prop["pwd"],
                                                              ap_prop["enc"])
                       ]
            checker_stings = ["SSCP SSC2 C +SAP:OK"]
            pass
        else:
            cmd = ["ATC AT2 CWSAP \"%s\" \"%s\" %d %d" % (ap_prop["ssid"],
                                                          ap_prop["pwd"],
                                                          ap_prop["channel"],
                                                          ap_prop["enc"])
                   ]
            checker_stings = ["ATR AT2 L OK"]
            pass
        if self.tc_action.load_and_exe_one_step(checker_stings, cmd,
                                                fail_string, check_time=50) is False:
            NativeLog.add_trace_critical("set target ap fail")
            ret = False
        return ret
        pass

    def setup_ap(self, ap_type, ap_prop):
        if ap_type == "target":
            ret = self._set_target_ap(ap_prop)
            pass
        else:
            ret = self._apc_switch(["ON"], [ap_prop["apc"]])
            # delay for 5 seconds, wait AP ready
            time.sleep(5)
            pass
        return ret

    def do_scan(self, ap_prop):
        fail_string = "Scan fail"
        ret = True
        # do not check if the set AP can be scanned
        if self.target_type[1] == "SSC":
            cmd = ["SSCC SSC1 sta -S"]
            checker_stings = ["SSCR SSC1 C +SCANDONE"]
            pass
        else:
            cmd = ["ATS AT1 AT+CWLAP"]
            checker_stings = ["ATR AT1 L OK"]
            pass
        if self.tc_action.load_and_exe_one_step(checker_stings, cmd,
                                                fail_string, check_time=100) is False:
            NativeLog.add_trace_critical("Scan fail")
            ret = False
        return ret
        pass

    def _switch_off_target_ap(self, delay):
        time.sleep(delay)
        self._set_target_ap(ERROR_AP_PROP)
        pass

    def _switch_on_target_ap(self, ap_prop, delay):
        time.sleep(delay)
        self._set_target_ap(ap_prop)
        pass

    def _switch_off_ap(self, ap_type, ap_prop, delay_range):
        delay = random.randint(delay_range[0]*10, delay_range[1]*10)/10
        if ap_type == "target":
            self._switch_off_target_ap(delay)
        else:
            delay -= 1.5
            time.sleep(delay if delay > 0 else 0)
            self._apc_switch(["OFF"], [ap_prop["apc"]])
        pass

    def _switch_on_ap(self, ap_type, ap_prop, delay_range):
        delay = random.randint(delay_range[0]*10, delay_range[1]*10)/10
        if ap_type == "target":
            self._switch_on_target_ap(ap_prop, delay)
        else:
            delay -= 1.5
            time.sleep(delay if delay > 0 else 0)
            self._apc_switch(["ON"], [ap_prop["apc"]])
        pass

    def _join_ap(self, ap_prop, test_method):
        fail_string = "join target ap fail, %s, %s" % (ap_prop["ssid"], ap_prop["pwd"])
        if self.target_type[1] == "SSC":
            cmd = ["SSCC SSC1 ap -C -s %s -p %s" % (ap_prop["ssid"],
                                                    ap_prop["pwd"],)
                   ]
            checker_stings = ["SSCR SSC1 C +JAP:CONNECTED"]
            pass
        else:
            cmd = ["ATC AT1 CWJAP \"%s\" \"%s\"" % (ap_prop["ssid"],
                                                    ap_prop["pwd"])
                   ]
            checker_stings = ["ATR AT1 NC ERROR NC FAIL L OK"]
            pass
        if test_method == "Normal":
            ret = self.tc_action.load_and_exe_one_step(checker_stings, cmd,
                                                       fail_string, check_freq=0.1, check_time=350)
            if ret is not False:
                ret *= 0.1
        else:
            ret = self.tc_action.load_and_exe_one_step([], cmd, fail_string)
        return ret
        pass

    def _check_join_ap_result(self, ap_prop):
        ret = False
        fail_string = "join ap fail, %s, %s" % (ap_prop["ssid"], ap_prop["pwd"])

        if self.target_type[1] == "SSC":
            checker_stings = ["SSCR SSC1 C +JAP:CONNECTED"]
            ret = self.tc_action.load_and_exe_one_step(checker_stings, ["DELAY 0"],
                                                       fail_string, check_freq=1, check_time=120)
            pass
        else:
            cmd = ["ATS AT1 AT+CWJAP?"]
            checker_stings = ["ATR AT1 NC busy NC No%20AP C +CWJAP"]
            for i in range(3):
                ret = self.tc_action.load_and_exe_one_step(checker_stings, cmd,
                                                           fail_string, check_freq=1, check_time=2)
                if ret is not False:
                    break
                time.sleep(15)

        return ret
        pass

    def join_ap(self, join_test_method, ap_type, ap_prop, delay):

        if join_test_method == "WRONG_PROP":
            _prop = ERROR_AP_PROP
        else:
            _prop = ap_prop

        ret = self._join_ap(_prop, join_test_method)

        if join_test_method == "OFF_ON":
            self._switch_off_ap(ap_type, ap_prop, delay[0])
            self._switch_on_ap(ap_type, ap_prop, delay[1])
            ret = self._check_join_ap_result(_prop)
            pass
        elif join_test_method == "OFF":
            self._switch_off_ap(ap_type, ap_prop, delay[0])
            time.sleep(25)
            pass

        return ret
        pass

    def do_reconnect(self, reconnect_test_method, ap_type, ap_prop, delay):
        ret = True
        if reconnect_test_method == "OFF_ON":
            self._switch_off_ap(ap_type, ap_prop, delay[0])
            self._switch_on_ap(ap_type, ap_prop, delay[1])
            ret = self._check_join_ap_result(ap_prop)
            pass
        elif reconnect_test_method == "OFF":
            self._switch_off_ap(ap_type, ap_prop, delay[0])
            pass
        return ret
        pass


def main():
    pass

if __name__ == '__main__':
    main()
