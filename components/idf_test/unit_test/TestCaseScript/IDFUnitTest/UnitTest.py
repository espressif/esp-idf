import re
import time

from TCAction import PerformanceTCBase
from TCAction import TCActionBase
from NativeLog import NativeLog

class UnitTest(PerformanceTCBase.PerformanceTCBase):
    def __init__(self, name, test_env, cmd_set, timeout=30, log_path=TCActionBase.LOG_PATH):
        PerformanceTCBase.PerformanceTCBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                     timeout=timeout, log_path=log_path)

        self.test_case = None
        self.test_timeout = 20

     # load param from excel
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self.test_case = " + "\"" + cmd_set[i][0] + "\""
                exec cmd_string
        self.result_cntx = TCActionBase.ResultCheckContext(self, test_env, self.tc_name)
        pass

    def send_commands(self):
        self.flush_data("UT1")

        try:
            self.serial_write_line("UT1", "\"" + self.test_case + "\"")
            data = ""
            for _ in range(self.timeout):
                time.sleep(1) #wait for test to run before reading result
                data += self.serial_read_data("UT1")
                if re.search('[^0].* Tests 0 F', data): #check that number of tests run != 0 and number of tests failed == 0
                    self.set_result("Succeed")
                    break
            else:
                self.set_result("Fail")

        except StandardError,e:
            NativeLog.add_exception_log(e)

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        self.send_commands()

def main():
    pass

if __name__ == '__main__':
    pass
