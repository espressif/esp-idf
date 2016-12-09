from TCAction import TCActionBase
from NativeLog import NativeLog
import time
import random
import string

TEST_COUNT_ONE_ROUND = 500


class TestCase(TCActionBase.CommonTCActionBase):
    def __init__(self, test_case, test_env, timeout=45, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, test_case, test_env,
                                                 timeout=timeout, log_path=log_path)
        self.send_len = 1460
        self.server_port = random.randint(10000, 50000)
        self.server_port_2 = random.randint(10000, 50000)
        self.server_echo = True
        self.test_time = 12 * 60
        self.sta_number = 3
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
        for i in range(self.sta_number + 2):
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
            test_time = self.test_time * 60
            # server port
            server_port = self.server_port
            server_port_t = self.server_port_2
            # ap ip
            # ap_ip = self.ap_ip
            # server echo
            server_echo = self.server_echo
            # station number
            sta_number = self.sta_number
            # send delay
            send_delay = self.send_delay
            # configurable params
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for TCPTransparent script, error is %s" % e)
            raise StandardError("Error configuration")

        # step0 reboot
        checker_stings = []
        test_action_string = []

        for i in range(sta_number + 2):
            checker_stings.append("P SSC%d C !!!ready!!!" % (i + 1))
            test_action_string.append("SSCC SSC%d reboot" % (i + 1))

            fail_string = "Fail, Fail to reboot"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return

        # switch off recv print
        checker_stings = []
        test_action_strings = []
        for i in range(self.sta_number + 2):
            checker_stings.append("R SSC%s C +RECVPRINT:0" % (i+1))
            test_action_strings.append("SSC SSC%s soc -R -o 0" % (i+1))
            fail_string = "Fail, Fail to turn off recv print"
            self.load_and_exe_one_step(checker_stings, test_action_strings, fail_string)

        # step1, set ap/STA mode on all target
        for i in range(sta_number + 2):
            checker_stings = ["R SSC%d C +MODE:OK" % (i + 1)]
            test_action_string = ["SSCC SSC%d op -S -o 3" % (i + 1)]
            fail_string = "Fail, Fail to set mode on SSC%d" % (i + 1)
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return

        # set different getway for SSC1 softAP
        checker_stings = ["R SSC1 C +DHCP:AP,OK"]
        test_action_string = ["SSCC SSC1 dhcp -E -o 2"]
        fail_string = "Fail, SSC1 Fail to disable DHCP"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = ["R SSC1 C +IP:OK"]
        test_action_string = ["SSCC SSC1 ip -S -o 2 -i 192.168.6.1"]
        fail_string = "Fail, SSC1 Fail to set IP"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = ["R SSC1 C +DHCP:AP,OK"]
        test_action_string = ["SSCC SSC1 dhcp -S -o 2"]
        fail_string = "Fail, SSC1 Fail to enable DHCP"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # set different getway for SSC2 softAP
        checker_stings = ["R SSC2 C +DHCP:AP,OK"]
        test_action_string = ["SSCC SSC2 dhcp -E -o 2"]
        fail_string = "Fail, SSC2 Fail to disable DHCP"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = ["R SSC2 C +IP:OK"]
        test_action_string = ["SSCC SSC2 ip -S -o 2 -i 192.168.5.1"]
        fail_string = "Fail, SSC2 Fail to set IP"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = ["R SSC2 C +DHCP:AP,OK"]
        test_action_string = ["SSCC SSC2 dhcp -S -o 2"]
        fail_string = "Fail, SSC2 Fail to enable DHCP"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step2, set ssid/password on SSC1
        ssid = "".join([random.choice(string.lowercase) for m in range(10)])
        password = "".join([random.choice(string.lowercase) for m in range(10)])
        checker_stings = ["R SSC1 C +SAP:OK"]
        test_action_string = ["SSCC SSC1 ap -S -s %s -p %s -n 10 -t 0 -m 8" % (ssid, password)]
        fail_string = "Fail, Fail to set ssid/password on SSC1"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step3, set ssid/password on SSC2
        ssid_1 = "".join([random.choice(string.lowercase) for m in range(10)])
        password_1 = "".join([random.choice(string.lowercase) for m in range(10)])
        checker_stings = ["R SSC2 C +SAP:OK"]
        test_action_string = ["SSCC SSC2 ap -S -s %s -p %s -n 10 -t 0 -m 8" % (ssid_1, password_1)]
        fail_string = "Fail, Fail to set ap ssid/password on SSC2"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step4, SSC2 join SSC1(soft AP)
        checker_stings = []
        test_action_string = []
        checker_stings.append("P SSC2 C +JAP:CONNECTED,%s" % ssid)
        test_action_string.append("SSCC SSC2 ap -C -s %s -p %s" % (ssid, password))
        fail_string = "Fail, Fail to connect to SSC1 SoftAP"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return
        # step5, create server on SSC1
        checker_stings = ["R SSC1 A <server_sock>:BIND:(\d+),OK"]
        test_action_string = ["SSCC SSC1 soc -B -t TCP -p %s" % server_port]
        fail_string = "Fail, Fail to create server on SSC1 while binding"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = ["R SSC1 RE LISTEN:(\d+),OK"]
        test_action_string = ["SSCC SSC1 soc -L -s <server_sock>"]
        fail_string = "Fail, Fail to create server on SSC1 while listening"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step6, create client on SSC2
        checker_stings = []
        test_action_string = []
        checker_stings.append("P SSC2 A <client_sock>:BIND:(\d+),OK")
        test_action_string.append("SSCC SSC2 soc -B -t TCP")
        fail_string = "Fail, SSC2 Fail to connect to server while binding"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = ["P SSC2 RE CONNECT:(\d+),OK", "P SSC1 A <accept_sock>:ACCEPT:(\d+),.+"]
        test_action_string = ["SSCC SSC2 soc -C -s <client_sock> -i %s -p %s" % ("192.168.6.1", server_port)]
        fail_string = "Fail, SSC2 Fail to connect to server while connecting"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step7, SSC3 - SSC5 join SSC2
        checker_stings = []
        test_action_string = []
        for i in range(sta_number):
            checker_stings.append("P SSC%d C +JAP:CONNECTED,%s" % (i + 3, ssid_1))
            test_action_string.append("SSCC SSC%d ap -C -s %s -p %s" % (i + 3, ssid_1, password_1))
            fail_string = "Fail, SSC%d Fail to connect to SSC2" % (i + 3)
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string, check_time=450) is False:
                return

        # step8, create server on SSC2
        checker_stings = ["R SSC2 A <server_sock_t>:BIND:(\d+),OK"]
        test_action_string = ["SSCC SSC2 soc -B -t TCP -p %s -i 192.168.5.1" % server_port_t]
        fail_string = "Fail, Fail to create server one SSC2 while binding"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = ["R SSC2 RE LISTEN:(\d+),OK"]
        test_action_string = ["SSCC SSC2 soc -L -s <server_sock_t>"]
        fail_string = "Fail, Fail to create server one SSC2 while listening"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step9, create client on SSC3 - SSC5
        checker_stings = []
        test_action_string = []
        for i in range(sta_number):
            checker_stings.append("P SSC%d A <client_sock%d>:BIND:(\d+),OK" % (i + 3, i + 3))
            test_action_string.append("SSCC SSC%d soc -B -t TCP" % (i + 3))
        fail_string = "Fail, Fail to connect to SSC2 server while binding"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        for i in range(sta_number):
            checker_stings = ["P SSC%d RE CONNECT:(\d+),OK" % (i + 3),
                              "P SSC2 A <accept_sock%d>:ACCEPT:(\d+),.+" % (i + 3)]
            test_action_string = ["SSCC SSC%d soc -C -s <client_sock%d> -i %s -p %s" %
                                  (i + 3, i + 3, "192.168.5.1", server_port_t)]
            fail_string = "Fail, Fail to connect to SSC2 server while connecting"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return

        start_time = time.time()
        # step 10, do send/recv
        while time.time() - start_time < test_time:

            checker_stings = []
            test_action_string = []
            if server_echo is True:
                test_action_string.append("SSC SSC1 soc -S -s <accept_sock> -l %d -n %d -j %d" %
                                          (send_len, TEST_COUNT_ONE_ROUND, send_delay))
                checker_stings.append("P SSC1 RE \+SEND:\d+,OK NC CLOSED")
                test_action_string.append("SSC SSC2 soc -S -s <server_sock> -l %d -n %d -j %d" %
                                          (send_len, TEST_COUNT_ONE_ROUND, send_delay))
                checker_stings.append("P SSC2 RE \+SEND:\d+,OK NC CLOSED")

            for i in range(sta_number):
                checker_stings.append("P SSC%d RE \+SEND:\d+,OK NC CLOSED" % (i + 3))
                test_action_string.append("SSC SSC%d soc -S -s <client_sock%d> -l %d -n %d -j %d" %
                                          (i + 3, i + 3, send_len, TEST_COUNT_ONE_ROUND, send_delay))
            for i in range(sta_number):
                test_action_string.append("SSC SSC2 soc -S -s <accept_sock%d> -l %d -n %d -j %d" %
                                          (i + 3, send_len, TEST_COUNT_ONE_ROUND, send_delay))
                checker_stings.append("P SSC2 RE \+SEND:\d+,OK NC CLOSED")

            fail_string = "Fail, Failed to send/recv data"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string,
                                          check_freq=1, check_time=300) is False:
                break
            pass

        NativeLog.add_prompt_trace("time escape: %s" % (time.time() - start_time))

        if (time.time() - start_time) > test_time:
            self.result_cntx.set_result("Succeed")
        else:
            self.result_cntx.set_result("Failed")
            checker_stings = []
            test_action_string = []
            for i in range(sta_number + 2):
                checker_stings.append("P SSC%d C CLOSEALL" % (i + 1))
                test_action_string.append("SSCC SSC%d soc -T" % (i + 1))
            fail_string = "Fail, Fail to close socket"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return
            # re-set server on SSC1
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
            # SSC2 connnect SSC1
            checker_stings = []
            test_action_string = []
            checker_stings.append("P SSC2 A <client_sock>:BIND:(\d+),OK")
            test_action_string.append("SSCC SSC2 soc -B -t TCP")
            fail_string = "Fail, SSC2 Fail to bind sock"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return

            checker_stings = ["P SSC2 RE CONNECT:(\d+),OK", "P SSC1 A <accept_sock>:ACCEPT:(\d+),.+"]
            test_action_string = ["SSCC SSC2 soc -C -s <client_sock> -i %s -p %s" % ("192.168.6.1", server_port)]
            fail_string = "Fail, SSC2 Fail to connect to SSC1 server"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return
            # create server on SSC2
            checker_stings = []
            test_action_string = []
            checker_stings.append("P SSC2 A <server_sock>:BIND:(\d+),OK")
            test_action_string.append("SSCC SSC2 soc -B -t TCP -p %s -i 192.168.5.1" % server_port_t)
            fail_string = "Fail, SSC2 Fail to bind"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return
            checker_stings = ["R SSC2 RE LISTEN:(\d+),OK"]
            test_action_string = ["SSCC SSC2 soc -L -s <server_sock>"]
            fail_string = "Fail, SSC2 Fail to listen"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return
            # create client on SSC3-SSC5
            checker_stings = []
            test_action_string = []
            for i in range(sta_number):
                checker_stings.append("P SSC%d A <client_sock%d>:BIND:(\d+),OK" % (i + 3, i + 3))
                test_action_string.append("SSCC SSC%d soc -B -t TCP" % (i + 3))
            fail_string = "Fail, Fail to connect to SSC2 server while binding"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return

            for i in range(sta_number):
                checker_stings = ["P SSC%d RE CONNECT:(\d+),OK" % (i + 3),
                                  "P SSC2 A <accept_sock%d>:ACCEPT:(\d+),.+" % (i + 3)]
                test_action_string = ["SSCC SSC%d soc -C -s <client_sock%d> -i %s -p %s" %
                                      (i + 3, i + 3, "192.168.5.1", server_port_t)]
                fail_string = "Fail, Fail to connect to server"
                if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                    return

    def result_check(self, port_name, data):
        TCActionBase.CommonTCActionBase.result_check(self, port_name, data)
        self.result_cntx.append_data(port_name, data)


def main():
    pass


if __name__ == '__main__':
    main()
