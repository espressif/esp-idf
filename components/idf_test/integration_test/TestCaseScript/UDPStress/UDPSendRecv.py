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
        checker_stings = ["R SSC1 C +RECVPRINT:1"]
        test_action_string = ["SSC SSC1 soc -R -o 1"]
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
            NativeLog.add_trace_critical("Error configuration for UDPSendRecv script, error is %s" % e)
            raise StandardError("Error configuration")

        ssid = "".join([random.choice(string.lowercase) for m in range(10)])
        password = "".join([random.choice(string.lowercase) for m in range(10)])

        # step 0 set ap
        checker_stings = ["R SSC1 C +SAP:OK"]
        test_action_string = ["SSC SSC1 ap -S -s %s -p %s -t 3" % (ssid, password)]
        fail_string = "Fail, Fail to set ap"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step 1 connect to ap and turn off recv print
        checker_stings = ["R SSC2 C +JAP:CONNECTED"]
        test_action_string = ["SSC SSC2 sta -C -s %s -p %s" % (ssid, password)]
        fail_string = "Fail, Fail to connect to server"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string, check_time=200) is False:
            return

        checker_stings = ["R SSC2 A <sta_ip>:\+STAIP:(\d+\.\d+\.\d+\.\d+)\r"]
        test_action_string = ["SSC SSC2 ip -Q -o 1"]
        fail_string = "Fail, Fail to connect to server"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string, check_time=200) is False:
            return

        checker_stings = ["P SSC1 C +RECVPRINT:0", "P SSC2 C +RECVPRINT:0"]
        test_action_string = ["SSC SSC1 soc -R -o 0", "SSC SSC2 soc -R -o 0"]
        fail_string = "Fail, Fail to turn off recv print"
        self.load_and_exe_one_step(checker_stings, test_action_string, fail_string)
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string, check_time=200) is False:
            return

        # step 2 create conn_num udp socket
        for i in range(1, conn_num+1):
            checker_stings = ["R SSC1 A <t1_sock%d>:\+BIND:(\d+),OK" % i,
                              "R SSC2 A <t2_sock%d>:\+BIND:(\d+),OK" % i]
            test_action_string = ["SSC SSC1 soc -B -t UDP -p <test_udp_port%i>" % i,
                                  "SSC SSC2 soc -B -t UDP -p <test_udp_port%i>" % i]
            fail_string = "Fail, Fail to create socket"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return

        start_time = time.time()
        # step 3, do send/recv
        while time.time()-start_time < test_time:

            checker_stings = ["P SSC1 NC ERROR NC CLOSE NC ERROR"]
            for i in range(1, conn_num+1):
                test_action_string = ["SSC SSC2 soc -S -s <t2_sock%d> -l %d -n %d -j %d "
                                      "-i <target_ap_ip> -p <test_udp_port%d>" %
                                      (i, send_len, TEST_COUNT_ONE_ROUND, send_delay, i)]
                checker_stings.append("P SSC2 RE \"\+SEND:%%%%s,OK\"%%%%(<t2_sock%d>) NC ERROR NC CLOSE NC ERROR" % i)
                if duplex is True:
                    test_action_string.append("SSC SSC1 soc -S -s <t1_sock%d> -l %d -n %d -j %d"
                                              " -i <sta_ip> -p <test_udp_port%d>" %
                                              (i, send_len, TEST_COUNT_ONE_ROUND, send_delay, i))
                    checker_stings.append("P SSC1 RE \"\+SEND:%%%%s,OK\"%%%%(<t1_sock%d>)" % i)

                fail_string = "Fail, Failed on send command"
                if self.load_and_exe_one_step([], test_action_string, fail_string) is False:
                    break
                time.sleep(1)

            fail_string = "Fail, Failed to send/recv data"
            if self.load_and_exe_one_step(checker_stings, ["DELAY 0.1"], fail_string,
                                          check_freq=1, check_time=300) is False:
                break
            pass

        NativeLog.add_prompt_trace("time escape: %s" % (time.time() - start_time))
        if time.time() - start_time >= test_time:
            self.result_cntx.set_result("Succeed")
        else:
            self.result_cntx.set_result("Fail")

        # finally, execute done

    def result_check(self, port_name, data):
        TCActionBase.CommonTCActionBase.result_check(self, port_name, data)
        self.result_cntx.append_data(port_name, data)


def main():
    pass

if __name__ == '__main__':
    main()


