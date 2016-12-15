from TCAction import TCActionBase
from NativeLog import NativeLog
import time
import random
import string


class TestCase(TCActionBase.CommonTCActionBase):
    def __init__(self, test_case, test_env, timeout=45, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, test_case, test_env, timeout, log_path)
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
            server_echo = self.server_echo
            conn_number = self.conn_number
            sta_number = self.sta_number
            test_time = self.test_time * 60
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

        # step1, set ap mode on SSC1, set STA mode on SSC2-SSC3
        checker_stings = ["R SSC1 C +MODE:OK"]
        test_action_string = ["SSCC SSC1 op -S -o 2"]
        fail_string = "Fail, Fail to set mode on SSC1"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        for i in range(sta_number):
            checker_stings = ["R SSC%d C +MODE:OK" % (i + 2)]
            test_action_string = ["SSCC SSC%d op -S -o 1" % (i + 2)]
            fail_string = "Fail, Fail to set mode on SSC%d" % (i + 2)
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return

        # turn off recv print
        for i in range(sta_number + 1):
            checker_stings = ["P SSC%d C +RECVPRINT:0" % (i + 1)]
            test_action_string = ["SSCC SSC%d soc -R -o 0" % (i + 1)]
            fail_string = "Fail, Fail to turn off recv print"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return

        # step2, set ssid/password on SSC1
        ssid = "".join([random.choice(string.lowercase) for m in range(10)])
        password = "".join([random.choice(string.lowercase) for m in range(10)])
        tcp_port = random.randint(10000, 20000)

        checker_stings = ["R SSC1 C +SAP:OK"]
        test_action_string = ["SSCC SSC1 ap -S -s %s -p %s -n 10 -t 0 -m 10" % (ssid, password)]
        fail_string = "Fail, Fail to set ssid/password on SSC1"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step3, SSC2-SSC3 connect to SSC1
        checker_stings = []
        test_action_string = []
        for i in range(sta_number):
            checker_stings.append("P SSC%d C +JAP:CONNECTED,%s" % (i + 2, ssid))
            test_action_string.append("SSCC SSC%d ap -C -s %s -p %s" % (i + 2, ssid, password))
            fail_string = "Fail, SSC%d Fail to connect to SoftAP" % (i + 2)
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string, check_time=450) is False:
                return

        # step4, create tcp server on STA SSC2
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

        # step5, create multi client on SSC3
        for i in range(conn_number):
            checker_stings = ["P SSC3 A <client_sock%d>:BIND:(\d+),OK" % i]
            test_action_string = ["SSCC SSC3 soc -B -t TCP"]
            fail_string = "Fail, Fail to create client on SSC3"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return

            checker_stings = ["P SSC3 RE CONNECT:(\d+),OK", "P SSC2 A <accept_sock%d>:ACCEPT:(\d+),.+ NC ERROR" % i]
            test_action_string = ["SSCC SSC3 soc -C -s <client_sock%d> -i %s -p %s" % (i, "192.168.4.2", tcp_port)]
            fail_string = "Fail, Fail to connect to SSC2 server while connecting"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return

        start_time = time.time()
        # step6, do send/recv
        while time.time() - start_time < test_time:
            checker_stings = []
            test_action_string = []
            # SSC2 send packets to SSC3
            if server_echo is True:
                for i in range(conn_number):
                    checker_stings.append(
                        "P SSC2 RE \+SEND:%s,OK NC CLOSED NC ERROR NC unexpected" % self.get_parameter(
                            "accept_sock%d" % i))
                    test_action_string.append("SSC SSC2 soc -S -s <accept_sock%d> -l %d -n 1000 -j %d" %
                                              (i, send_len, send_delay))
            # SSC3 send packets to SSC2
            for i in range(conn_number):
                test_action_string.append("SSC SSC3 soc -S -s <client_sock%d> -l %d -n 1000 -j %d" %
                                          (i, send_len, send_delay))
                checker_stings.append(
                    "P SSC3 RE \+SEND:%s,OK NC CLOSED NC ERROR" % self.get_parameter("client_sock%d" % i))

            fail_string = "Fail, Failed to send/recv data"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string,
                                          check_freq=1, check_time=900) is False:
                # checker_stings = ["R SSC1 C +LSTADONE"]
                # test_action_string = ["SSCC SSC1 ap -L"]
                # fail_string = ""
                # if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                #     pass
                break

            if send_delay > 20:
                send_delay -= 10

        NativeLog.add_trace_critical("Time escape: %d" % (time.time() - start_time))

        if (time.time() - start_time) > test_time:
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
