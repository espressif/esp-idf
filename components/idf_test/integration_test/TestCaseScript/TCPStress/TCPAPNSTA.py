from TCAction import TCActionBase
from NativeLog import NativeLog
import time
import random
import string


TEST_COUNT_ONE_ROUND = 500


class TestCase(TCActionBase.CommonTCActionBase):

    def __init__(self, test_case, test_env, timeout=30, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, test_case, test_env, timeout=timeout, log_path=log_path)
        self.send_len = 1460
        self.server_echo = True
        self.sta_number = 4
        self.test_time = 12 * 60
        self.send_delay = 50
        # load param from excel
        cmd_set = test_case["cmd set"]
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        self.result_cntx = TCActionBase.ResultCheckContext(self, test_env, self.tc_name)
        pass

    def cleanup(self):
        checker_stings = []
        test_action_strings = []
        for i in range(self.sta_number + 1):
            checker_stings.append("R SSC%s C +RECVPRINT:1" % (i+1))
            test_action_strings.append("SSC SSC%s soc -R -o 1" % (i+1))
            fail_string = "Fail, Fail to turn on recv print"
            self.load_and_exe_one_step(checker_stings, test_action_strings, fail_string)
        pass

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        self.result_cntx.start()

        try:
            # configurable params
            send_len = self.send_len
            server_echo = self.server_echo
            sta_number = self.sta_number
            test_time = self.test_time * 60
            send_delay = self.send_delay
            ap_ip = self.get_parameter("target_ap_ip")
            # configurable params
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for TCPTransparent script, error is %s" % e)
            raise StandardError("Error configuration")

        # step0 reboot
        checker_stings = []
        test_action_string = []

        for i in range(sta_number+1):
            checker_stings.append("P SSC%d C !!!ready!!!" % (i+1))
            test_action_string.append("SSCC SSC%d reboot" % (i+1))

        fail_string = "Fail, Fail to reboot"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # switch off recv print
        checker_stings = []
        test_action_strings = []
        for i in range(self.sta_number + 1):
            checker_stings.append("R SSC%s C +RECVPRINT:0" % (i+1))
            test_action_strings.append("SSC SSC%s soc -R -o 0" % (i+1))
            fail_string = "Fail, Fail to turn off recv print"
            self.load_and_exe_one_step(checker_stings, test_action_strings, fail_string)

        # step1 set ap on SSC1, create server
        checker_stings = ["R SSC1 C +MODE:OK"]
        test_action_string = ["SSCC SSC1 op -S -o 2"]
        fail_string = "Fail, Fail set mode"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        ssid = "".join([random.choice(string.lowercase) for m in range(10)])
        password = "".join([random.choice(string.lowercase) for m in range(10)])
        tcp_port = random.randint(10000, 20000)

        checker_stings = ["R SSC1 C +SAP:OK"]
        test_action_string = ["SSCC SSC1 ap -S -s %s -p %s -t 3 -m 8" % (ssid, password)]
        fail_string = "Fail, Fail set ap"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = ["R SSC1 A <server_sock>:BIND:(\d+),OK"]
        test_action_string = ["SSCC SSC1 soc -B -t TCP -p %s" % tcp_port]
        fail_string = "Fail, Fail create server"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = ["R SSC1 RE LISTEN:(\d+),OK"]
        test_action_string = ["SSCC SSC1 soc -L -s <server_sock>"]
        fail_string = "Fail, Fail create server"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step 2, 8 SSC target(SSC2 - SSC9) join SSC1 soft AP
        checker_stings = []
        test_action_string = []
        for i in range(sta_number):
            checker_stings.append("P SSC%d C +MODE:OK" % (i+2))
            test_action_string.append("SSCC SSC%d op -S -o 1" % (i+2))
        fail_string = "Fail, Fail set mode"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = []
        test_action_string = []
        for i in range(sta_number):
            checker_stings.append("P SSC%d C +JAP:CONNECTED,%s" % (i+2, ssid))
            test_action_string.append("SSCC SSC%d ap -C -s %s -p %s" % (i+2, ssid, password))
        fail_string = "Fail, Fail to connect to server"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step 3, create client on SSC2 - SSC9
        checker_stings = []
        test_action_string = []
        for i in range(sta_number):
            checker_stings.append("P SSC%d A <client_sock%d>:BIND:(\d+),OK" % (i+2, i+2))
            test_action_string.append("SSCC SSC%d soc -B -t TCP" % (i+2))
        fail_string = "Fail, Fail to connect to server"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        for i in range(sta_number):
            checker_stings = ["P SSC%d RE CONNECT:(\d+),OK" % (i+2),
                              "P SSC1 A <accept_sock%d>:ACCEPT:(\d+),.+" % (i+2)]
            test_action_string = ["SSCC SSC%d soc -C -s <client_sock%d> -i %s -p %s" %
                                  (i+2, i+2, ap_ip, tcp_port)]
            fail_string = "Fail, Fail to connect to server"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return

        start_time = time.time()
        # step 4, do send/recv
        while time.time() - start_time < test_time:
            checker_stings = []
            test_action_string = []
            for i in range(sta_number):
                checker_stings.append("P SSC%d RE \+SEND:\d+,OK NC CLOSED" % (i+2))
                test_action_string.append("SSC SSC%d soc -S -s <client_sock%d> -l %d -n %d -j %d" %
                                          (i+2, i+2, send_len, TEST_COUNT_ONE_ROUND, send_delay))
                if server_echo is True:
                    test_action_string.append("SSC SSC1 soc -S -s <accept_sock%d> -l %d -n %d -j %d" %
                                              (i+2, send_len, TEST_COUNT_ONE_ROUND, send_delay))
                    checker_stings.append("P SSC1 RE \"\+SEND:%%%%s,OK\"%%%%(<accept_sock%d>) NC CLOSED)" %
                                          (i+2))

            fail_string = "Fail, Failed to send/recv data"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string,
                                          check_freq=1, check_time=300) is False:
                break

        NativeLog.add_prompt_trace("time escape: %s" % (time.time() - start_time))

        if (time.time() - start_time) >= test_time:
            self.result_cntx.set_result("Succeed")
        else:
            self.result_cntx.set_result("Failed")
            # TODO: create a function to create TCP connections. reuse not copy paste code
            checker_stings = []
            test_action_string = []
            for i in range(sta_number + 1):
                checker_stings.append("P SSC%d C CLOSEALL" % (i + 1))
                test_action_string.append("SSCC SSC%d soc -T" % (i + 1))
            fail_string = "Fail, Fail to close socket"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return

            server_port = random.randint(20000, 30000)
            checker_stings = ["R SSC1 A <server_sock>:BIND:(\d+),OK"]
            test_action_string = ["SSCC SSC1 soc -B -t TCP -p %s" % server_port]
            fail_string = "Fail, Fail to bind socket"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return

            checker_stings = ["R SSC1 RE LISTEN:(\d+),OK"]
            test_action_string = ["SSCC SSC1 soc -L -s <server_sock>"]
            fail_string = "Fail, Fail to listen"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return

            checker_stings = []
            test_action_string = []
            for i in range(sta_number):
                checker_stings.append("P SSC%d A <client_sock%d>:BIND:(\d+),OK" % (i + 2, i + 2))
                test_action_string.append("SSCC SSC%d soc -B -t TCP" % (i + 2))
            fail_string = "Fail, Fail to connect to server"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return

            for i in range(sta_number):
                checker_stings = ["P SSC%d RE CONNECT:(\d+),OK" % (i + 2),
                                  "P SSC1 A <accept_sock%d>:ACCEPT:(\d+),.+" % (i + 2)]
                test_action_string = ["SSCC SSC%d soc -C -s <client_sock%d> -i %s -p %s" %
                                      (i + 2, i + 2, ap_ip, server_port)]
                fail_string = "Fail, Fail to connect to server"
                if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                    return

        # finally, execute done

    def result_check(self, port_name, data):
        TCActionBase.CommonTCActionBase.result_check(self, port_name, data)
        self.result_cntx.append_data(port_name, data)


def main():
    pass

if __name__ == '__main__':
    main()

