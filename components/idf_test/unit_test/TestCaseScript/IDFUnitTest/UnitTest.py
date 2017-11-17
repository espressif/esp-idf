# -*- coding: utf-8 -*-
import re
import time

from TCAction import PerformanceTCBase
from TCAction import TCActionBase
from NativeLog import NativeLog


class UnitTest(PerformanceTCBase.PerformanceTCBase):
    def __init__(self, test_case, test_env, cmd_set, timeout=30, log_path=TCActionBase.LOG_PATH):
        PerformanceTCBase.PerformanceTCBase.__init__(self, test_case, test_env, cmd_set=cmd_set,
                                                     timeout=timeout, log_path=log_path)
        self.case = cmd_set[1][0]
        self.test_timeout = 20
        self.reset_reason = test_case['reset']
        self.result_cntx = TCActionBase.ResultCheckContext(self, test_env, self.tc_name)

    def send_commands(self):
        self.flush_data("UT1")
        try:
            self.serial_write_line("UT1", "\"" + self.case + "\"")
            data = ""

            for _ in range(self.timeout):
                time.sleep(1)  # wait for test to run before reading result
                data += self.serial_read_data("UT1")
                if re.search('[^0].* Tests 0 F',
                             data):  # check that number of tests run != 0 and number of tests failed == 0
                    self.set_result("Succeed")
                    break
            else:
                self.set_result("Fail")

            reset_list = self.reset_reason.split(",") if self.reset_reason else ''
            pattern = re.compile(r"(ets [\w]{3}  [\d]{1,2} [\d]{4} [\d]{2}:[\d]{2}:[\d]{2}[^(\)]*\([\w].*?\))"
                                 r"|(Guru Meditation Error: Core  [\d] panic'ed \([\w].*?\))")
            reset_exception = pattern.findall(data)
            if reset_list and len(reset_list) == len(reset_exception):
                for i, reset in enumerate(reset_list):
                    temp_reset = reset_exception[i]
                    if reset not in "".join(temp_reset):
                        self.set_result("Fail")
                        break
                else:
                    self.set_result("Succeed")

        except StandardError, e:
            NativeLog.add_exception_log(e)

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        self.send_commands()


def main():
    pass


if __name__ == '__main__':
    pass
