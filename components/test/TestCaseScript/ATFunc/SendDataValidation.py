from TCAction import TCActionBase
from TCAction import CmdHandler
from NativeLog import NativeLog
import time
import threading
import sys
reload(sys)
sys.setdefaultencoding('iso-8859-1')  # # use encoding that with 1 Byte length and contain 256 chars


VALIDATION_STRING = "".join([chr((m+65) % 256) for m in range(256)])  # make it start from 'A'


class ResultCheckCntx(TCActionBase.ResultCheckContext):

    def __init__(self, test_action, test_env, name):
        TCActionBase.ResultCheckContext.__init__(self, test_action, test_env, name)
        pass

    def run(self):
        tx_result = -1
        rx_result = -1

        while True:
            exit_flag = self.wait_exit_event(2)
            # force exit
            if exit_flag is True:
                break
            try:
                self.lock_data()
                rx_port = filter(lambda x: x[0] == "AT1", self.data_cache)
                tx_port = filter(lambda x: x[0] == "SOC2", self.data_cache)
            finally:
                self.unlock_data()

            if len(rx_port) == 1:
                data = rx_port[0][1]
                rx_result = data.find(VALIDATION_STRING)
            if len(tx_port) == 1:
                data = tx_port[0][1]
                tx_result = data.find(VALIDATION_STRING)

            if tx_result != -1:
                self.test_action.tx_check_done.set()
            if rx_result != -1:
                self.test_action.rx_check_done.set()


class SendDataValidation(TCActionBase.CommonTCActionBase):

    def __init__(self, name, test_env, cmd_set, timeout=30, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                 timeout=timeout, log_path=log_path)
        # load param from excel
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        self.result_cntx = TCActionBase.ResultCheckContext(self, test_env, self.tc_name)
        self.timestamp = time.strftime("%d%H%M%S", time.localtime())
        self.tx_check_done = threading.Event()
        self.rx_check_done = threading.Event()

        pass

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        self.result_cntx.start()

        try:
            # configurable params
            # enable target TCP TX
            tx_enable = self.tx_enable
            # enable target TCP RX
            rx_enable = self.rx_enable
            # transparent mode select
            is_transparent_mode = self.is_transparent_mode
            # configurable params
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for TCPTransparent script, error is %s" % e)
            raise StandardError("Error configuration")

        # step1 create PC server
        checker_stings = ["SOCR SOC_COM L OK"]
        test_action_string = ["SOC SOC1 LISTEN <test_tcp_port1>"]
        fail_string = "Fail, Fail on create PC server"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step2 target connect, switch to transparent
        checker_stings = ["SOCR SOC1 C +ACCEPT", "ATR AT1 NC CLOSE L OK"]
        test_action_strings = ["ATC AT1 CIPSTART \"TCP\" <pc_ip> <test_tcp_port1>"]
        fail_string = "Fail, Fail on connect to PC server"
        if self.load_and_exe_one_step(checker_stings, test_action_strings, fail_string) is False:
            return

        checker_stings = ["SOCR SOC_COM L OK"]
        test_action_strings = ["SOC SOC1 ACCEPT SOC2"]
        if self.load_and_exe_one_step(checker_stings, test_action_strings, fail_string) is False:
            return

        # set to transparent mode
        if is_transparent_mode is True:
            checker_stings = ["ATR AT1 L OK"]
            test_action_strings = ["ATS AT1 AT+CIPMODE=1"]
            if self.load_and_exe_one_step(checker_stings, test_action_strings, fail_string) is False:
                return

            checker_stings = ["ATR AT1 C >"]
            test_action_strings = ["ATS AT1 AT+CIPSEND"]
            if self.load_and_exe_one_step(checker_stings, test_action_strings, fail_string) is False:
                return
        else:
            checker_stings = ["ATR AT1 C >"]
            test_action_strings = ["ATS AT1 AT+CIPSEND=256"]
            if self.load_and_exe_one_step(checker_stings, test_action_strings, fail_string) is False:
                return

        # step 3

        # switch to new result check context
        self.result_cntx.stop_thread()
        self.result_cntx.join()
        self.result_cntx = ResultCheckCntx(self, self.test_env, self.tc_name)
        self.result_cntx.start()

        # step 3 send data
        if rx_enable is True:
            test_action = CmdHandler.parse_action("SOC SOC2 SEND 256", self.test_env)
            CmdHandler.do_actions(test_action[0], self.test_env)
            self.rx_check_done.wait(5)
            if self.rx_check_done.isSet() is False:
                # rx fail
                return
            # flush all data
            self.result_cntx.data_flush()
            self.tx_check_done.clear()

        if tx_enable is True:
            test_action = CmdHandler.parse_action("ATSN AT1 256", self.test_env)
            CmdHandler.do_actions(test_action[0], self.test_env)
            self.tx_check_done.wait(5)
            if self.tx_check_done.isSet() is False:
                # tx fail
                return

        # finally, execute done
        self.result_cntx.set_result("Succeed")

    def result_check(self, port_name, data):
        TCActionBase.CommonTCActionBase.result_check(self, port_name, data)
        self.result_cntx.append_data(port_name, data)
        pass


def main():
    pass

if __name__ == '__main__':
    main()

