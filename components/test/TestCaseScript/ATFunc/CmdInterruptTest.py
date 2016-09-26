from TCAction import TCActionBase
from NativeLog import NativeLog
from TCAction import CmdHandler
import time


ATCmdList = ["GMR",
             "UART=115200,8,1,0,0",
             "CWMODE=3",
             "CWJAP=\"TL_WR845N_T\",\"1234567890\"",
             "CWLAP",
             "CWQAP",
             "CWSAP=\"asdf\",\"123456789\",5,3",
             "CWLIF",
             "CWDHCP=3",
             "AT+CWAUTOCONN",
             "CIPSTAMAC=\"18:fe:34:97:f3:43\"",
             "CIPAPMAC=\"1a:fe:34:97:f3:43\"",
             "CIPSTA=\"192.168.1.2\"",
             "CIPAP=\"192.168.4.1\"",
             "CIPSTATUS",
             "CIPSTART=\"UDP\",\"192.168.1.4\",6531,7895,1",
             "CIPSTART=\"TCP\",\"192.168.1.4\",6531",
             "CIPCLOSE",
             "CIFSR",
             "CIPMUX=1",
             "CIPSERVER=1,4567",
             "CIPMODE=0",
             "CIPSTO=7200",
             "PING=\"192.168.1.4\""]


class CmdInterruptTest(TCActionBase.CommonTCActionBase):

    def __init__(self, name, test_env, cmd_set, timeout=20, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                 timeout=timeout, log_path=log_path)

        self.result_cntx = TCActionBase.ResultCheckContext(self, test_env, self.tc_name)
        pass

    def load_and_exe_one_step(self, checker_stings, test_action_strings, fail_string,
                              check_freq=0.1, check_time=50, sleep_time=0.1):
        # set checker for next executing step
        checkers = CmdHandler.parse_results(checker_stings, self.test_env)
        self.result_cntx.set_next_step(checkers, check_time, check_freq)
        # execute 1 step
        for action_string in test_action_strings:
            test_action = CmdHandler.parse_action(action_string, self.test_env)
            CmdHandler.do_actions(test_action, self.test_env)
            time.sleep(sleep_time)

        ret = self.wait_to_execute()

        if ret is False:  # # timeout
            self.result_cntx.set_result(fail_string)
        if ret == check_time:
            self.result_cntx.set_result(fail_string)
            ret = False

        self.require_waiting()

        return ret

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        self.result_cntx.start()

        # step 1, sleep time 0.1
        for cmd1 in ATCmdList:
            # check if match CMD - AT - busy - OK/ERROR pattern
            checker_stings = ["ATR AT1 C busy", "ATR AT1 R *"]
            test_action_string = ["ATS AT1 AT+%s" % cmd1, "ATS AT1 AT"]
            fail_string = "Fail, Fail on step 1"
            if self.load_and_exe_one_step(checker_stings, test_action_string,
                                          fail_string, sleep_time=0.1) is False:
                # check again if match CMD - OK/ERROR - AT - OK pattern
                checker_stings = ["ATR AT1 R *", "ATR AT1 C AT L OK"]
                test_action_string = ["ATS AT1 AT+%s" % cmd1, "ATS AT1 AT"]
                fail_string = "Fail, Fail on step 1"
                if self.load_and_exe_one_step(checker_stings, test_action_string,
                                              fail_string, sleep_time=0.1) is False:
                    NativeLog.add_trace_critical("CMD Fail: AT+%s; sleep time is 0.1" % cmd1)

        # step 2, sleep time 0
        for cmd1 in ATCmdList:
            # check if match CMD - AT - busy - OK/ERROR pattern
            checker_stings = ["ATR AT1 C busy", "ATR AT1 R *"]
            test_action_string = ["ATS AT1 AT+%s" % cmd1, "ATS AT1 AT"]
            fail_string = "Fail, Fail on step 1"
            if self.load_and_exe_one_step(checker_stings, test_action_string,
                                          fail_string, sleep_time=0.1) is False:
                # check again if match CMD - OK/ERROR - AT - OK pattern
                checker_stings = ["ATR AT1 R *", "ATR AT1 C AT L OK"]
                test_action_string = ["ATS AT1 AT+%s" % cmd1, "ATS AT1 AT"]
                fail_string = "Fail, Fail on step 1"
                if self.load_and_exe_one_step(checker_stings, test_action_string,
                                              fail_string, sleep_time=0.1) is False:
                    NativeLog.add_trace_critical("CMD Fail: AT+%s; sleep time is 0" % cmd1)

        # step 3, cat string
        for cmd1 in ATCmdList:
            # check if match CMD - AT - busy - OK/ERROR pattern
            checker_stings = ["ATR AT1 C busy", "ATR AT1 R *"]
            test_action_string = ["ATSO AT1 AT+%s\r\nAT\r\n" % cmd1]
            fail_string = "Fail, Fail on step 1"
            if self.load_and_exe_one_step(checker_stings, test_action_string,
                                          fail_string, sleep_time=0.1) is False:
                # check again if match CMD - OK/ERROR - AT - OK pattern
                checker_stings = ["ATR AT1 R *", "ATR AT1 C AT L OK"]
                test_action_string = ["ATS AT1 AT+%s" % cmd1, "ATS AT1 AT"]
                fail_string = "Fail, Fail on step 1"
                if self.load_and_exe_one_step(checker_stings, test_action_string,
                                              fail_string, sleep_time=0.1) is False:
                    NativeLog.add_trace_critical("CMD Fail: AT+%s; cat string" % cmd1)

        # finally, execute done
        self.result_cntx.set_result("Succeed")

    def result_check(self, port_name, data):
        TCActionBase.CommonTCActionBase.result_check(self, port_name, data)
        self.result_cntx.append_data(port_name, data)


def main():
    pass

if __name__ == '__main__':
    main()

