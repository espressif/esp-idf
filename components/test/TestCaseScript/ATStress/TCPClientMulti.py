from TCAction import TCActionBase
from NativeLog import NativeLog


class TCPClientMulti(TCActionBase.CommonTCActionBase):

    def __init__(self, name, test_env, cmd_set, timeout=30, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                 timeout=timeout, log_path=log_path)
        # load param from excel
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string

        self.result_cntx = TCActionBase.ResultCheckContext(self, test_env, self.tc_name)
        self.max_conn = test_env.get_variable_by_name("max_conn")[1]
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
            enable_log = self.enable_log
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for TCPClientMulti script, error is %s" % e)
            raise StandardError("Error configuration")
        # configurable params

        # step1
        checker_stings = ["R SOC_COM L OK"]
        test_action_string = ["SOC SOC1 LISTEN <test_tcp_port1>"]
        fail_string = "Fail, Fail on create PC server"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step2
        for i in range(0, self.max_conn):
            checker_stings = ["R SOC1 C +ACCEPT", "R AT1 NC CLOSE L OK"]
            test_action_strings = ["ATC AT1 CIPSTART %d \"TCP\" <pc_ip> <test_tcp_port1>" % i]
            fail_string = "Fail, Fail on connect to PC server"
            if self.load_and_exe_one_step(checker_stings, test_action_strings, fail_string) is False:
                return

            checker_stings = ["R SOC_COM L OK"]
            test_action_strings = ["SOC SOC1 ACCEPT SOC%d" % (i+2)]
            if self.load_and_exe_one_step(checker_stings, test_action_strings, fail_string) is False:
                return

        # step 3
        # turn off AT UART logging
        if enable_log is False:
            self.test_env.uart_ports["AT1"].set_uart_logging_flag(False)

        data = "A" * at_send_length
        fail_string = "Fail, Fail on send and recv data"

        for j in range(0, test_count):

            if tx_enable is True:
                for i in range(0, self.max_conn):
                    checker_stings = ["P AT1 C >"]
                    test_action_strings = ["ATS AT1 AT+CIPSEND=%d,%d" % (i, at_send_length)]
                    if self.load_and_exe_one_step(checker_stings, test_action_strings,
                                                  fail_string, check_time=50) is False:
                        NativeLog.add_trace_critical("Fail on target send command for link %d" % i)
                        NativeLog.add_trace_critical("Test count is %d" % j)
                        return

                    checker_stings = ["P SOC%d RL %d" % ((i+2), at_send_length), "P AT1 C OK"]
                    test_action_strings = ["ATSO AT1 %s" % data]
                    if self.load_and_exe_one_step(checker_stings, test_action_strings,
                                                  fail_string, check_time=50) is False:
                        NativeLog.add_trace_critical("Fail on target send for link %d, send or recv error" % i)
                        NativeLog.add_trace_critical("Test count is %d" % j)
                        return

            if rx_enable is True:
                checker_stings = []
                test_action_strings = []
                for i in range(0, self.max_conn):
                    checker_stings.extend(["P AT1 DL %d+%d" % (i, soc_send_length)])
                    test_action_strings.extend(["SOC SOC%d SEND %d %s" % (i+2, soc_send_length, data)])

                if self.load_and_exe_one_step(checker_stings, test_action_strings,
                                              fail_string, check_time=50) is False:
                    NativeLog.add_trace_critical("Fail to receive PC sent data")
                    NativeLog.add_trace_critical("Test count is %d" % j)
                    return

        # finally, execute done
        self.result_cntx.set_result("Succeed")

    def result_check(self, port_name, data):
        TCActionBase.CommonTCActionBase.result_check(self, port_name, data)
        self.result_cntx.append_data(port_name, data)


def main():
    pass

if __name__ == '__main__':
    main()

