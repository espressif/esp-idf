import os
import time
import random
import threading
import socket

from TCAction import TCActionBase
from NativeLog import NativeLog

AP_PROP_KEY = ("ssid", "password", "apc")


class SendThread(threading.Thread):
    def __init__(self, sock, send_len, target_addr, delay):
        threading.Thread.__init__(self)
        self.sock = sock
        self.send_len = send_len
        self.target_addr = target_addr
        self.delay = delay
        self.count = 0
        self.exit_event = threading.Event()
        pass

    def exit(self):
        self.exit_event.set()

    def run(self):
        data = "A" * self.send_len
        if self.sock is None:
            return
        while True:
            if self.exit_event.isSet() is True:
                break
            try:
                self.sock.sendto(data, self.target_addr)
            except StandardError:
                break
            self.count += 1
            time.sleep(self.delay * 0.001)

    def calculate(self):
        return self.count


class RecvThread(threading.Thread):
    def __init__(self, sock):
        threading.Thread.__init__(self)
        self.sock = sock
        self.exit_event = threading.Event()
        self.calc_event = threading.Event()
        self.bytes_recv = 0

    def start_calc(self):
        self.calc_event.set()

    def stop_calc(self):
        self.calc_event.clear()
        self.exit_event.set()

    def run(self):
        if self.sock is None:
            return
        ret = True
        while ret:
            if self.exit_event.isSet() is True:
                break
            try:
                data, addr = self.sock.recvfrom(65535)
            except StandardError:
                break
            if self.calc_event.isSet() is True:
                self.bytes_recv += len(data)
            if len(data) == 0:
                start = time.time()
                while time.time() - start < 30:
                    try:
                        data, addr = self.sock.recvfrom(65535)
                    except StandardError:
                        break
                    if len(data) == 0:
                        break
                    else:
                        self.bytes_recv += len(data)
                else:
                    ret = False

    def get_bytes_recv(self):
        return self.bytes_recv

    pass


class TestCase(TCActionBase.CommonTCActionBase):
    def __init__(self, test_case, test_env, timeout=30, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, test_case, test_env, timeout, log_path)
        self.att_test_list = range(60)
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
            pc_send = self.pc_send
            target_send = self.target_send
            test_time = self.test_time
            delay = self.delay
            ap_ssid = self.get_parameter("ap_ssid")
            ap_password = self.get_parameter("ap_password")
            pc_ip = self.get_parameter("pc_ip")
            target_ip = self.get_parameter("target_ip")
            # configurable params
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for UDP script, error is %s" % e)
            raise StandardError("Error configuration")

        udp_port = random.randint(40000, 50000)

        # reboot before executing
        checker_stings = ["R SSC1 C ready!!!"]
        test_action_string = ["SSC SSC1 reboot"]
        fail_string = "Fail, Fail to reboot"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # disable recv print on target
        checker_stings = ["R SSC1 C +RECVPRINT"]
        test_action_string = ["SSC SSC1 soc -R -o 0"]
        fail_string = "Fail, Fail to disable recv print"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # create socket on pc
        udp_sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
        udp_sock.bind((pc_ip, udp_port))
        udp_sock.settimeout(1)

        # connect to AP
        checker_stings = ["R SSC1 C +JAP:CONNECTED"]
        test_action_string = ["SSC SSC1 sta -C -s %s -p %s" % (ap_ssid, ap_password)]
        fail_string = "Fail, Fail to JAP"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string,
                                      check_freq=1, check_time=30) is False:
            return

        # close all connection
        checker_stings = ["R SSC1 C +CLOSEALL"]
        test_action_string = ["SSC SSC1 soc -T"]
        fail_string = "Fail, Fail to create server"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # create UDP socket on target
        checker_stings = ["R SSC1 A <client_sock>:\+BIND:(\d+),OK"]
        test_action_string = ["SSC SSC1 soc -B -t UDP -p %s" % udp_port]
        fail_string = "Fail, Fail bind"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        send_thread = SendThread(udp_sock if pc_send is True else None,
                                 send_len, (target_ip, udp_port), delay)
        send_thread.start()

        recv_thread = RecvThread(udp_sock if target_send is True else None)
        recv_thread.start()

        # start calculate
        recv_thread.start_calc()
        send_count = 0
        if target_send is True:
            # do send from target
            start = time.time()
            while time.time() - start < test_time * 60:
                checker_stings = ["P SSC1 RE \+SEND:0,OK"]
                test_action_string = ["SSC SSC1 soc -S -s <client_sock> -l %s -n 1000 -i %s -p %s -j %s" % (
                    send_len, pc_ip, udp_port, delay)]
                fail_string = "Fail, Fail to send"
                if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string, check_freq=0.1,
                                              check_time=3000) is False:
                    return
                send_count += 1000
        else:
            time.sleep(test_time * 60)

        send_thread.exit()
        send_thread.join()

        # stop throughput calculate
        while True:
            if recv_thread.isAlive() is False:
                recv_thread.stop_calc()
                recv_thread.join()
                break

        recv_count = 0
        if pc_send is True:
            # get received data len from PC
            self.load_and_exe_one_step(["R SSC1 A <recv_len>:RECVLEN:(\d+)"],
                                       ["SSC SSC1 soc -Q -s <client_sock> -o 1"],
                                       "Fail, Fail to get recv data len")
            try:
                rx_data_len = int(self.get_parameter("recv_len"))
            except StandardError:
                rx_data_len = 0

            if (rx_data_len % send_len) > 0:
                recv_count = rx_data_len / send_len + 1
            else:
                recv_count = rx_data_len / send_len

            send_count = send_thread.calculate()

        if recv_thread.get_bytes_recv() > 0:
            if (recv_thread.get_bytes_recv() % send_len) > 0:
                recv_count = recv_thread.get_bytes_recv() / send_len + 1
            else:
                recv_count = recv_thread.get_bytes_recv() / send_len
        udp_sock.close()

        NativeLog.add_trace_critical("send_count is %s, recv_count is %s" % (send_count, recv_count))
        self.result_cntx.set_result("Succeed")
        NativeLog.add_trace_critical(
            "UDP Packet lose rate is %.2f%%" % (float(send_count - recv_count) / send_count * 100))

    # finally, execute done

    def result_check(self, port_name, data):
        TCActionBase.CommonTCActionBase.result_check(self, port_name, data)
        self.result_cntx.append_data(port_name, data)


def main():
    pass


if __name__ == '__main__':
    main()
