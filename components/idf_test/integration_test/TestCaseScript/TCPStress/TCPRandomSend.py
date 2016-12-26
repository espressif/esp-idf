import os
import time
import random
import threading
import socket
from TCAction import TCActionBase
from NativeLog import NativeLog
from NativeLog import ThroughputResult


class TestCase(TCActionBase.CommonTCActionBase):

    def __init__(self, test_case, test_env, timeout=30, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, test_case, test_env, timeout=timeout, log_path=log_path)
        self.send_len_config = range(1460)
        self.delay_config = [0, 0.01, 0.1, 0.5, 1]
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
            send_len_config = self.send_len_config
            delay_config = self.delay_config
            send_count = self.send_count
            test_time = self.test_time * 60
            # configurable params
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for TCPThroughput script, error is %s" % e)
            raise StandardError("Error configuration")

        # disable recv print during random send test
        checker_stings = ["R SSC1 C +RECVPRINT"]
        test_action_string = ["SSC SSC1 soc -R -o 0"]
        fail_string = "Fail, Fail to disable recv print"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        pc_ip = self.get_parameter("pc_ip")
        tcp_port = random.randint(50000, 60000)

        # step 0 create tcp connection

        server_sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
        server_sock.bind((pc_ip, tcp_port))
        server_sock.settimeout(1)
        server_sock.listen(5)

        checker_stings = ["R SSC1 A <client_sock>:\+BIND:(\d+),OK"]
        test_action_string = ["SSC SSC1 soc -B -t TCP"]
        fail_string = "Fail, Fail bind"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = ["P SSC1 RE \+CONNECT:\d+,OK"]
        test_action_string = ["SSC SSC1 soc -C -s <client_sock> -i %s -p %s" % (pc_ip, tcp_port)]
        fail_string = "Fail, Fail to connect"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        sock, addr = server_sock.accept()
        sock.settimeout(10)
        # set no delay so that tcp segment will be send as soon as send called
        sock.setsockopt(socket.IPPROTO_TCP, socket.TCP_NODELAY, 1)

        # step 1 start send
        start_time = time.time()
        while time.time() - start_time < test_time:
            for delay in delay_config:
                for i in xrange(send_count):
                    send_len = random.choice(send_len_config)
                    data = "A" * (send_len+1)
                    try:
                        sock.send(data)
                    except socket.error, e:
                        NativeLog.add_exception_log(e)
                        NativeLog.add_trace_critical("Fail to send packets")
                        return
                        pass
                    time.sleep(delay)

        NativeLog.add_prompt_trace("time escape: %s" % (time.time() - start_time))

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
