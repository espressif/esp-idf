from TCAction import TCActionBase
from NativeLog import NativeLog
import time


class UDPMulti(TCActionBase.CommonTCActionBase):

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
            target_ip_str = self.target_ip_str
            tx_enable = self.tx_enable
            rx_enable = self.rx_enable
            enable_log = self.enable_log
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for UDPMulti script, error is %s" % e)
            raise StandardError("Error configuration")
        # configurable params

        # step1, bind one PC UDP port
        checker_stings = ["R SOC_COM L OK"]
        test_action_string = ["SOC SOC1 BIND <test_udp_port9>"]
        fail_string = "Fail, Fail on binding socket"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step 2 create 5 UDP link on target
        for i in range(0, self.max_conn):
            checker_stings = ["R AT1 C CONNECT L OK"]
            test_action_strings = ["ATC AT1 CIPSTART %d \"UDP\" <pc_ip> <test_udp_port9> <test_udp_port%d> 1"
                                   % (i, i+1)]
            fail_string = "Fail, Fail on create UDP link"
            if self.load_and_exe_one_step(checker_stings, test_action_strings, fail_string) is False:
                return

        # step 3 send recv data
        # turn off AT UART logging
        if enable_log is False:
            self.test_env.uart_ports["AT1"].set_uart_logging_flag(False)

        for j in range(0, test_count):
            data = "A" * at_send_length
            fail_string = "Fail, Fail on send/recv data"

            if tx_enable is True:
                # target link 0-5 sendto PC
                for i in range(0, self.max_conn):
                    checker_stings = ["P AT1 C >"]
                    test_action_strings = ["ATS AT1 AT+CIPSEND=%d,%d" % (i, at_send_length)]
                    if self.load_and_exe_one_step(checker_stings, test_action_strings,
                                                  fail_string, check_time=20) is False:
                        NativeLog.add_trace_critical("Target fail on send cmd on link %d" % i)
                        NativeLog.add_trace_critical("Test count is %d" % j)

                    checker_stings = ["P SOC_COM C RECV_LEN=%d P <test_udp_port%d>" % (at_send_length, i+1),
                                      "P AT1 C OK"]
                    test_action_strings = ["ATSO AT1 %s" % data]
                    if self.load_and_exe_one_step(checker_stings, test_action_strings,
                                                  fail_string, check_time=20) is False:
                        NativeLog.add_trace_critical("Target sent UDP packet error on link %d" % i)
                        NativeLog.add_trace_critical("Test count is %d" % j)

            if rx_enable is True:
                # PC send to target
                checker_stings = []
                test_action_strings = []
                for i in range(0, self.max_conn):
                    checker_stings.extend(["P AT1 DL %d+%d" % (i, soc_send_length)])
                    test_action_strings.extend(["SOC SOC1 SENDTO %d <test_udp_port%d> %s"
                                                % (soc_send_length, i+1, target_ip_str)])

                if self.load_and_exe_one_step(checker_stings, test_action_strings,
                                              fail_string, check_time=20) is False:
                    NativeLog.add_trace_critical("PC sent UDP packet error")
                    NativeLog.add_trace_critical("Test count is %d" % j)

        # finally, execute done
        self.result_cntx.set_result("Succeed")

    def result_check(self, port_name, data):
        self.result_cntx.append_data(port_name, data)


def main():
    pass

if __name__ == '__main__':
    main()

