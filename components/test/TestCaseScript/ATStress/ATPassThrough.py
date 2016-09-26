import time

from TCAction import TCActionBase
from NativeLog import NativeLog


BEACON_TIMEOUT = 3
WAIT_FOR_RECONNECT = 20


class ATPassThrough(TCActionBase.CommonTCActionBase):

    def __init__(self, name, test_env, cmd_set, timeout=30, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                 timeout=timeout, log_path=log_path)
        self.do_scan = True
        # load param from excel
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string

        self.result_cntx = TCActionBase.ResultCheckContext(self, test_env, self.tc_name)
        pass

    def cleanup(self):
        TCActionBase.CommonTCActionBase.cleanup(self)
        # turn on logging
        self.test_env.uart_ports["AT1"].set_uart_logging_flag(True)

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        self.result_cntx.start()

        # configurable params
        try:
            at_send_length = self.at_send_length
            soc_send_length = self.soc_send_length
            test_count = self.test_count
            tx_enable = self.tx_enable
            rx_enable = self.rx_enable
            att_set = self.att_set
            do_scan = self.do_scan
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for TCPClientMulti script, error is %s" % e)
            raise StandardError("Error configuration")
        # configurable params

        # step0, set att and join ap
        fail_string = "Fail, Fail on JAP, set to single link mode"

        checker_stings = ["R PC_COM L OK"]
        test_action_string = ["ATT <att_port> 1"]
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = ["R AT1 C ready"]
        test_action_string = ["ATS AT1 AT+RST"]
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = ["R AT1 L OK"]
        test_action_string = ["ATS AT1 AT+CWMODE=1"]
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = ["R AT1 L OK"]
        test_action_string = ["ATC AT1 CWJAP <ap_ssid> <ap_password>"]
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = ["R AT1 L OK"]
        test_action_string = ["ATS AT1 AT+CIPMUX=0"]
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step1, create TCP connection and enter pass through mode
        fail_string = "Fail, Fail on create server, create connection or enter pass through mode"

        checker_stings = ["R SOC_COM L OK"]
        test_action_string = ["SOC SOC1 LISTEN <test_tcp_port1>"]
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = ["R SOC1 C +ACCEPT", "R AT1 NC CLOSE L OK"]
        test_action_string = ["ATC AT1 CIPSTART \"TCP\" <pc_ip> <test_tcp_port1>"]
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = ["R AT1 L OK"]
        test_action_strings = ["ATS AT1 AT+CIPMODE=1"]
        if self.load_and_exe_one_step(checker_stings, test_action_strings, fail_string) is False:
            return

        checker_stings = ["R AT1 C >"]
        test_action_strings = ["ATS AT1 AT+CIPSEND"]
        if self.load_and_exe_one_step(checker_stings, test_action_strings, fail_string) is False:
            return

        checker_stings = ["R SOC_COM L OK"]
        test_action_strings = ["SOC SOC1 ACCEPT SOC2"]
        if self.load_and_exe_one_step(checker_stings, test_action_strings, fail_string) is False:
            return

        # step2
        # while
        # set att from, send data on both direction
        # if TCP connection disconnected, then set att to 1, wait reconnect succeed, continue test
        for i in xrange(test_count):
            for _att in att_set:

                # set att
                checker_stings = ["R PC_COM L OK"]
                test_action_string = ["ATT <att_port> %d" % _att]
                if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                    return
                time.sleep(BEACON_TIMEOUT)

                # do scan to get ssid
                if do_scan is True:
                    checker_stings = []
                    test_action_string = ["ATSO AT1 +++"]
                    if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                        return

                    checker_stings = ["R AT1 L OK"]
                    test_action_string = ["ATC AT1 CWLAP <ap_ssid>"]
                    if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                        return

                    checker_stings = ["R AT1 C >"]
                    test_action_strings = ["ATS AT1 AT+CIPSEND"]
                    if self.load_and_exe_one_step(checker_stings, test_action_strings, fail_string) is False:
                        return

                # send data
                checker_stings = []
                test_action_string = []
                if tx_enable is True:
                    checker_stings += ["P SOC2 RL %d" % at_send_length]
                    test_action_string += ["ATSN AT1 %d" % at_send_length]
                if rx_enable is True:
                    checker_stings += ["P AT1 RL %d" % soc_send_length]
                    test_action_string += ["SOC SOC2 SEND %d" % soc_send_length]

                if len(test_action_string) > 0:
                    if self.load_and_exe_one_step(checker_stings, test_action_string, "",
                                                  check_freq=1, check_time=30) is False:
                        # send data fail
                        NativeLog.add_prompt_trace("Failed to send data @ att %d" % _att)
                        # set att back to 1
                        checker_stings = ["R PC_COM L OK"]
                        test_action_string = ["ATT <att_port> 1"]
                        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                            return
                        # wait for reconnect
                        time.sleep(WAIT_FOR_RECONNECT)
                        fail_string = "Failed, failed to accept socket"
                        checker_stings = ["SOCR SOC_COM L OK"]
                        test_action_strings = ["SOC SOC1 ACCEPT SOC2"]
                        if self.load_and_exe_one_step(checker_stings, test_action_strings, fail_string) is False:
                            return
                        break
            pass

        # finally, execute done
        self.result_cntx.set_result("Succeed")

    def result_check(self, port_name, data):
        TCActionBase.CommonTCActionBase.result_check(self, port_name, data)
        self.result_cntx.append_data(port_name, data)


def main():
    pass

if __name__ == '__main__':
    main()

