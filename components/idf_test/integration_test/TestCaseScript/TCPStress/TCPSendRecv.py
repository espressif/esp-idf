from TCAction import TCActionBase
from NativeLog import NativeLog
import time
import random
import string

TEST_COUNT_ONE_ROUND = 1000


class TestCase(TCActionBase.CommonTCActionBase):

    def __init__(self, test_case, test_env, timeout=30, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, test_case, test_env, timeout=timeout, log_path=log_path)
        # load param from excel
        cmd_set = test_case["cmd set"]
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        self.result_cntx = TCActionBase.ResultCheckContext(self, test_env, self.tc_name)
        pass

    def cleanup(self):
        # step 0 turn on recv print
        checker_stings = ["P SSC1 C +RECVPRINT:1", "P SSC2 C +RECVPRINT:1"]
        test_action_string = ["SSC SSC1 soc -R -o 1", "SSC SSC2 soc -R -o 1"]
        fail_string = "Fail, Fail to turn on recv print"
        self.load_and_exe_one_step(checker_stings, test_action_string, fail_string)
        pass

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        self.result_cntx.start()

        try:
            # configurable params
            send_len = self.send_len
            test_time = self.test_time * 60
            duplex = self.duplex
            conn_num = self.conn_num
            send_delay = self.send_delay
            # configurable params
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for TCPSendRecv script, error is %s" % e)
            raise StandardError("Error configuration")

        ssid = "".join([random.choice(string.lowercase) for m in range(10)])
        password = "".join([random.choice(string.lowercase) for m in range(10)])
        tcp_port = random.randint(10000, 50000)

        # step 0 set ap
        checker_stings = ["R SSC1 C +SAP:OK"]
        test_action_string = ["SSC SSC1 ap -S -s %s -p %s -t 3" % (ssid, password)]
        fail_string = "Fail, Fail to set ap"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            self.result_cntx.set_result("Fail")
            return

        # step 1 connect to ap and turn off recv print
        checker_stings = ["R SSC2 C +JAP:CONNECTED"]
        test_action_string = ["SSC SSC2 sta -C -s %s -p %s" % (ssid, password)]
        fail_string = "Fail, Fail to connect to server"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string, check_time=200) is False:
            self.result_cntx.set_result("Fail")
            return

        checker_stings = ["P SSC1 C +RECVPRINT:0", "P SSC2 C +RECVPRINT:0"]
        test_action_string = ["SSC SSC1 soc -R -o 0", "SSC SSC2 soc -R -o 0"]
        fail_string = "Fail, Fail to turn off recv print"
        self.load_and_exe_one_step(checker_stings, test_action_string, fail_string)
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string, check_time=200) is False:
            self.result_cntx.set_result("Fail")
            return

        # step 2 create server on AP
        checker_stings = ["R SSC1 A <server_sock>:\+BIND:(\d+),OK"]
        test_action_string = ["SSC SSC1 soc -B -t TCP -p %s" % tcp_port]
        fail_string = "Fail, Fail to create server"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            self.result_cntx.set_result("Fail")
            return

        checker_stings = ["R SSC1 A <server_sock>:\+LISTEN:(\d+),OK"]
        test_action_string = ["SSC SSC1 soc -L -s <server_sock>"]
        fail_string = "Fail, Fail to create server"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            self.result_cntx.set_result("Fail")
            return

        # step 3 create conn_num tcp connections
        for i in range(conn_num):
            checker_stings = ["R SSC2 A <client_sock%s>:\+BIND:(\d+),OK" % i]
            test_action_string = ["SSC SSC2 soc -B -t TCP"]
            fail_string = "Fail, Fail to bind"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                self.result_cntx.set_result("Fail")
                return

            checker_stings = ["P SSC1 A <accept_sock%s>:\+ACCEPT:(\d+),\d+" % i,
                              "P SSC2 RE \+CONNECT:\d+,OK"]
            test_action_string = ["SSC SSC2 soc -C -s <client_sock%s> -i <target_ap_ip> -p %s" % (i, tcp_port)]
            fail_string = "Fail, Fail to connect"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                self.result_cntx.set_result("Fail")
                return

        start_time = time.time()
        # step 4, do send/recv
        while time.time()-start_time < test_time:

            checker_stings = ["P SSC1 NC ERROR NC CLOSE NC ERROR"]
            for i in range(conn_num):
                test_action_string = ["SSC SSC2 soc -S -s <client_sock%d> -l %d -n %d -j %d" %
                                      (i, send_len, TEST_COUNT_ONE_ROUND, send_delay)]
                checker_stings.append("P SSC2 RE \"\+SEND:%%%%s,OK\"%%%%(<client_sock%d>) NC ERROR NC CLOSE" % i)

                if duplex is True:
                    checker_stings.append("P SSC1 RE \"\+SEND:%%%%s,OK\"%%%%(<accept_sock%d>)" % i)
                    test_action_string.append("SSC SSC1 soc -S -s <accept_sock%d> -l %d -n %d -j %d" %
                                              (i, send_len, TEST_COUNT_ONE_ROUND, send_delay))

                fail_string = "Fail, Failed on send command"
                if self.load_and_exe_one_step([], test_action_string, fail_string) is False:
                    self.result_cntx.set_result("Fail")
                    break

            fail_string = "Fail, Failed to send/recv data"
            if self.load_and_exe_one_step(checker_stings, ["DELAY 0.1"], fail_string,
                                          check_freq=1, check_time=300) is False:
                NativeLog.add_prompt_trace("time escape: %s" % (time.time() - start_time))
                self.result_cntx.set_result("Fail")
                return

        NativeLog.add_prompt_trace("time escape: %s" % (time.time() - start_time))

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


