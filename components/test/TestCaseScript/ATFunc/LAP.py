from TCAction import TCActionBase
import time
import re


class LAP(TCActionBase.CommonTCActionBase):
    def __init__(self, name, test_env, cmd_set, timeout=30, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                 timeout=timeout, log_path=log_path)
        # load param from excel
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        self.result_cntx = TCActionBase.ResultCheckContext(self, test_env, self.tc_name)
        pass

    def cleanup(self):
        # restore set LAPOPT
        if self.load_and_exe_one_step(["R AT1 L OK"],
                                      ["ATS AT1 AT+CWLAPOPT=0,127"],
                                      "Failed to set LAP option") is False:
            return

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        self.result_cntx.start()

        # step 1. set LAPOPT
        if self.load_and_exe_one_step(["R AT1 L OK"],
                                      ["ATS AT1 AT+CWLAPOPT=1,4"],
                                      "Failed to set LAP option") is False:
            return

        # step 2. LAP
        if self.load_and_exe_one_step(["R AT1 A <lap_result>:([[^OK]]+)OK"],  # [] is list generator, use [[]] for []
                                      ["ATS AT1 AT+CWLAP"],
                                      "Failed to LAP") is False:
            return
        lap_result = self.get_parameter("lap_result")
        rssi_list = re.findall("CWLAP:\((-\d+)\)", lap_result)
        if len(rssi_list) > 1:
            for i in range(len(rssi_list)-1):
                if int(rssi_list[i]) < int(rssi_list[i+1]):
                    break
            else:
                self.result_cntx.set_result("Succeed")
        else:
            self.result_cntx.set_result("Succeed")
        pass

    def result_check(self, port_name, data):
        TCActionBase.CommonTCActionBase.result_check(self, port_name, data)
        self.result_cntx.append_data(port_name, data)
        pass
    pass


def main():
    pass


if __name__ == '__main__':
    main()
