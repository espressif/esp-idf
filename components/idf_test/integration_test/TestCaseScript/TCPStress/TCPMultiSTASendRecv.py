from TCAction import TCActionBase
from NativeLog import NativeLog
import time
import random
import string

TEST_COUNT_ONE_ROUND = 500


class TestCase(TCActionBase.CommonTCActionBase):
    def __init__(self, test_case, test_env, timeout=45, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, test_case, test_env, timeout=timeout, log_path=log_path)
        # load param from excel
        cmd_set = test_case["cmd set"]
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        self.result_cntx = TCActionBase.ResultCheckContext(self, test_env, self.tc_name)
        pass

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        self.result_cntx.start()

        try:
            # configurable params
            send_len = self.send_len
            test_time = self.test_time * 60
            server_echo = self.server_echo
            sta_number = self.sta_number
            send_delay = self.send_delay
            # configurable params
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for TCPTransparent script, error is %s" % e)
            raise StandardError("Error configuration")

        # step0 reboot
        for i in range(sta_number + 1):
            checker_stings = ["P SSC%d C ready!!!" % (i + 1)]
            test_action_string = ["SSCC SSC%d restore" % (i + 1)]
            fail_string = "Fail, Fail to restore"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return

        # turn off recv print
        for i in range(sta_number + 1):
            checker_stings = ["P SSC%d C +RECVPRINT:0" % (i + 1)]
            test_action_string = ["SSCC SSC%d soc -R -o 0" % (i + 1)]
            fail_string = "Fail, Fail to turn off recv print"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return

        # step1, set softap mode on SSC1
        checker_stings = ["R SSC1 C +MODE:OK"]
        test_action_string = ["SSCC SSC1 op -S -o 2"]
        fail_string = "Fail, Fail to set mode on SSC1"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step2, set STA mode on SSC2-SSCn
        for i in range(sta_number):
            checker_stings = ["R SSC%d C +MODE:OK" % (i + 2)]
            test_action_string = ["SSCC SSC%d op -S -o 1" % (i + 2)]
            fail_string = "Fail, Fail to set mode on SSC%d" % (i + 2)
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return

        ssid = "".join([random.choice(string.lowercase) for m in range(10)])
        password = "".join([random.choice(string.lowercase) for m in range(10)])
        tcp_port = random.randint(40000, 50000)

        # step3, set ssid/password on SSC1
        checker_stings = ["R SSC1 C +SAP:OK"]
        test_action_string = ["SSCC SSC1 ap -S -s %s -p %s -n 10 -t 0 -m 10" % (ssid, password)]
        fail_string = "Fail, Fail to set ssid/password on SSC1"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step4, SSC2-SSCn join SSC1(soft AP)
        for i in range(sta_number):
            checker_stings = ["P SSC%d C +JAP:CONNECTED,%s" % (i + 2, ssid)]
            test_action_string = ["SSCC SSC%d ap -C -s %s -p %s" % (i + 2, ssid, password)]
            fail_string = "Fail, SSC%d Fail to connect to SSC1" % (i + 2)
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string, check_time=450) is False:
                return

        # step5, create server on SSC2
        checker_stings = ["R SSC2 A <server_sock>:BIND:(\d+),OK"]
        test_action_string = ["SSCC SSC2 soc -B -t TCP -p %s" % tcp_port]
        fail_string = "Fail, Fail to create server on SSC2 while binding"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = ["R SSC2 RE LISTEN:(\d+),OK"]
        test_action_string = ["SSCC SSC2 soc -L -s <server_sock>"]
        fail_string = "Fail, Fail to create server on SSC2 while listening"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step6, create client on SSC3-SSCn
        for i in range(sta_number - 1):
            checker_stings = ["P SSC%d A <client_sock%d>:BIND:(\d+),OK" % (i + 3, i + 3)]
            test_action_string = ["SSCC SSC%d soc -B -t TCP" % (i + 3)]
            fail_string = "Fail, SSC%d Fail to connect to TCP server while binding" % (i + 3)
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return

        for i in range(sta_number - 1):
            checker_stings = ["P SSC%d RE CONNECT:(\d+),OK" % (i + 3), "P SSC2 A <accept_sock%d>:ACCEPT:(\d+),.+" % i]
            test_action_string = [
                "SSCC SSC%d soc -C -s <client_sock%d> -i 192.168.4.2 -p %s" % (i + 3, i + 3, tcp_port)]
            fail_string = "Fail, SSC%d Fail to connect to TCP server while connecting" % (i + 3)
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return

        start_time = time.time()
        # step7, do send/recv, SSC2<---->other STAs
        while time.time() - start_time < test_time:
            checker_stings = []
            test_action_string = []
            # SSC2 send packets to SSC3-SSCn
            if server_echo is True:
                for i in range(sta_number - 1):
                    test_action_string.append("SSC SSC2 soc -S -s <accept_sock%d> -l %d -n 1000 -j %d" %
                                              (i, send_len, send_delay))
                    checker_stings.append(
                        "P SSC2 RE \+SEND:%s,OK NC CLOSED NC ERROR" % self.get_parameter("accept_sock%d" % (i + 3)))

            # SSC3-SSCn send packets to SSC2
            for i in range(sta_number - 1):
                checker_stings.append(
                    "P SSC%d RE \+SEND:%s,OK NC CLOSED NC ERROR" % (i + 3, self.get_parameter("client_sock%d" % i)))
                test_action_string.append("SSC SSC%d soc -S -s <client_sock%d> -l %d -n 1000 -j %d" %
                                          (i + 3, i + 3, send_len, send_delay))

            fail_string = "Fail, Failed to send/recv data"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string,
                                          check_freq=1, check_time=900) is False:
                break

            # drop off the delay time if it's greater than 20ms
            if send_delay > 20:
                send_delay -= 10

        NativeLog.add_trace_critical("Time escape: %d" % (time.time() - start_time))

        if (time.time() - start_time) >= test_time:
            self.result_cntx.set_result("Succeed")
        else:
            self.result_cntx.set_result("Failed")

            # finally, execute done

    def result_check(self, port_name, data):
        TCActionBase.CommonTCActionBase.result_check(self, port_name, data)
        self.result_cntx.append_data(port_name, data)


def main():
    pass


if __name__ == '__main__':
    main()
