import time
import random
import threading

from TCAction import TCActionBase
from NativeLog import NativeLog


class StableCase1(TCActionBase.CommonTCActionBase):

    def __init__(self, name, test_env, cmd_set, timeout=30, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                 timeout=timeout, log_path=log_path)
        # load param from excel
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        self.exit_event = threading.Event()
        self.result_cntx = TCActionBase.ResultCheckContext(self, test_env, self.tc_name)
        pass

    def check_wifi_status(self, data):
        if data.find("+JAP:DISCONNECTED") != -1:
            self.exit_event.set()
            NativeLog.add_trace_critical("[Wifi] Disconnected")
        pass

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        self.result_cntx.start()

        try:
            # configurable params
            # target role
            target_role = self.target_role
            # enable tcp send/recv
            tcp_enable = self.tcp_enable
            # enable udp send/recv
            udp_enable = self.udp_enable
            # enable ping
            ping_enable = self.ping_enable
            # delay range
            delay_range = self.delay_range
            # test time in hours
            test_time = self.test_time * 3600
            # configurable params
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for TCPTransparent script, error is %s" % e)
            raise StandardError("Error configuration")

        if target_role == "AP":
            pc_ip = "<pc_ip_wifi>"
            target_ip = "<target_ap_ip>"
        elif target_role == "STA":
            pc_ip = "<pc_ip>"
            target_ip = "<target_ip>"
        else:
            raise StandardError("target role only support AP or STA")

        # step 1, create UDP socket and TCP server
        checker_stings = ["R SSC1 A <udp_socket>:BIND:(\d+),OK"]
        test_action_string = ["SSC SSC1 soc -B -t UDP -p <test_udp_port1>"]
        fail_string = "Fail, Fail to create UDP socket"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = ["R SSC1 A <tcp_server>:BIND:(\d+),OK"]
        test_action_string = ["SSC SSC1 soc -B -t TCP -p <random_port>"]
        fail_string = "Fail, Fail to create tcp server"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = ["R SSC1 RE LISTEN:(\d+),OK"]
        test_action_string = ["SSC SSC1 soc -L -s <tcp_server>"]
        fail_string = "Fail, Fail to listen"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step 2, PC connect to 8266 tcp server, PC create UDP socket
        checker_stings = ["R SOC_COM C OK"]
        test_action_string = ["SOC SOC1 BIND <test_udp_port1> %s" % pc_ip]
        fail_string = "Fail, Fail to create udp socket on PC"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = ["P SSC1 A <tcp_socket>:ACCEPT:(\d+),\d+", "P SOC_COM C OK"]
        test_action_string = ["SOC SOC2 CONNECT <random_port> %s 0 %s" % (target_ip, pc_ip)]
        fail_string = "Fail, Fail to create tcp socket on PC"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        start_time = time.time()
        total_test_count = ping_fail_count = tcp_fail_count = udp_fail_count = 0

        # step 3, start do tcp/udp/ping
        while time.time() - start_time < test_time and self.exit_event.isSet() is False:
            total_test_count += 1
            if ping_enable is True:
                # do ping
                checker_stings = ["P PC_COM RE \+PING:\d+ms"]
                test_action_string = ["PING %s -n 1 -w 1000" % target_ip]
                fail_string = "Fail, Fail to ping target"
                if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                    ping_fail_count += 1
                    NativeLog.add_prompt_trace("[ping fail] fail/total = %s/%s"
                                               % (ping_fail_count, total_test_count))
                pass

            data_len = random.randint(1, 1460)

            if tcp_enable is True:
                # do tcp send/recv
                checker_stings = ["P SSC1 SL <tcp_socket>+%s" % data_len, "P SOC2 RL %s" % data_len]
                test_action_string = ["SSC SSC1 soc -S -s <tcp_socket> -l %s" % data_len,
                                      "SOC SOC2 SEND %s" % data_len]
                fail_string = "Fail, Fail to send/recv tcp"
                if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                    tcp_fail_count += 1
                    NativeLog.add_prompt_trace("[tcp fail] fail/total = %s/%s"
                                               % (tcp_fail_count, total_test_count))
                    # tcp fail, break
                    self.exit_event.set()
                pass

            if udp_enable is True:
                # do udp send/recv
                checker_stings = ["P SSC1 SL <udp_socket>+%s" % data_len, "P SOC1 RL %s" % data_len]
                test_action_string = ["SSC SSC1 soc -S -s <udp_socket> "
                                      "-i %s -p <test_udp_port1> -l %s" % (pc_ip, data_len),
                                      "SOC SOC1 SENDTO %s <test_udp_port1> %s" % (data_len, target_ip)]
                fail_string = "Fail, Fail to sendto/recvfrom udp"
                if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string, check_time=20) is False:
                    udp_fail_count += 1
                    NativeLog.add_prompt_trace("[udp fail] fail/total = %s/%s"
                                               % (udp_fail_count, total_test_count))
                pass

            # sleep
            time.sleep(random.randint(delay_range[0], delay_range[1]))
            pass

        # finally, execute done
        if self.exit_event.isSet() is False:
            self.result_cntx.set_result("Succeed")

    def result_check(self, port_name, data):
        self.result_cntx.append_data(port_name, data)
        if port_name != "SOC1" and port_name != "SOC2":
            # socket received data do not need to be logged
            TCActionBase.CommonTCActionBase.result_check(self, port_name, data)
        if port_name == "SSC1":
            self.check_wifi_status(data)


def main():
    pass

if __name__ == '__main__':
    main()
