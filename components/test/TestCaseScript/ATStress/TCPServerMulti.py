from TCAction import TCActionBase
from NativeLog import NativeLog


class TCPServerMulti(TCActionBase.CommonTCActionBase):

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
        self.test_env.uart_ports["AT1"].set_uart_logging_flag(True)

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        self.result_cntx.start()

        # configurable params
        try:
            at_send_length = self.at_send_length
            soc_send_length = self.soc_send_length
            test_count = self.test_count
            target_ip_str = self.target_ip_str
            enable_log = self.enable_log
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for TCPSeverMulti script, error is %s" % e)
            raise StandardError("Error configuration")
        # configurable params

        # turn off AT UART logging
        if enable_log is False:
            self.test_env.uart_ports["AT1"].set_uart_logging_flag(False)

        # step1 create TCP server on target
        checker_stings = ["R AT1 L OK"]
        test_action_string = ["ATC AT1 CIPSERVER 1 <test_tcp_port1>"]
        fail_string = "Fail, Fail on create target TCP server"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step2 PC connect to target server
        for j in range(0, test_count):
            data = "A" * at_send_length
            fail_string = "Fail, Fail on connect to target server"

            # check if all connection can send data on target
            checker_stings = ["P AT1 C OK"]
            test_action_strings = ["ATS AT1 AT+CIPCLOSE=%d" % self.max_conn]
            if self.load_and_exe_one_step(checker_stings, test_action_strings,
                                          fail_string, check_time=100) is False:
                NativeLog.add_trace_critical("Fail to close all connection")
                NativeLog.add_trace_critical("Test count is %d" % j)
                continue  # if fail on this step, we can recover and continue

            # a) do connect
            fail_flag = False
            for i in range(0, self.max_conn):
                checker_stings = ["P SOC_COM C OK", "P AT1 C CONNECT"]
                test_action_strings = ["SOC SOC%d CONNECT <test_tcp_port1> %s" % (i+1, target_ip_str)]
                if self.load_and_exe_one_step(checker_stings, test_action_strings,
                                              fail_string, check_time=50) is False:
                    NativeLog.add_trace_critical("Fail to connect to target for link %d" % i)
                    NativeLog.add_trace_critical("Test count is %d" % j)
                    # if fail on this step, we can recover and continue
                    fail_flag = True
                    break

            if fail_flag is True:
                # fail on step a)
                continue

            # b) check if all connection can recv data on target
            checker_stings = []
            test_action_strings = []
            for i in range(0, self.max_conn):
                checker_stings.extend(["P AT1 DL %d+%d" % (i, soc_send_length)])
                test_action_strings.extend(["SOC SOC%d SEND %d" % (i+1, soc_send_length)])

            if self.load_and_exe_one_step(checker_stings, test_action_strings,
                                          fail_string, check_time=50) is False:
                NativeLog.add_trace_critical("Fail to receive data from PC")
                NativeLog.add_trace_critical("Test count is %d" % j)
                continue  # if fail on this step, we can recover and continue

            # c) check if all connection can send data on target
            for i in range(0, self.max_conn):
                checker_stings = ["P AT1 C >"]
                test_action_strings = ["ATS AT1 AT+CIPSEND=%d,%d" % (i, at_send_length)]
                if self.load_and_exe_one_step(checker_stings, test_action_strings,
                                              fail_string, check_time=50) is False:
                    NativeLog.add_trace_critical("Fail on target send command for link %d" % i)
                    NativeLog.add_trace_critical("Test count is %d" % j)
                    return

                checker_stings = ["P SOC%d RL %d" % ((i+1), at_send_length), "P AT1 C OK"]
                test_action_strings = ["ATSO AT1 %s" % data]
                if self.load_and_exe_one_step(checker_stings, test_action_strings,
                                              fail_string, check_time=50) is False:
                    NativeLog.add_trace_critical("Fail on target send for link %d, send or recv error" % i)
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

