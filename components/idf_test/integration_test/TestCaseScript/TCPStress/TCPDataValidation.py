import os
import random
import threading
import socket
import time
import re

from TCAction import TCActionBase
from TCAction import PerformanceTCBase
from NativeLog import NativeLog


def calc_hash(index):
    return (index & 0xffffffff) % 83 + (index & 0xffffffff) % 167


def verify_data(data, start_index):
    for i, c in enumerate(data):
        if ord(c) != calc_hash(start_index + i):
            NativeLog.add_trace_critical("[Data Validation Error] target sent data index %u is error."
                                         " Sent data is %x, should be %x"
                                         % (start_index + i, ord(c), calc_hash(start_index + i)))
            return False
    return True


def make_validation_data(length, start_index):
    return bytes().join([chr(calc_hash(start_index + i)) for i in range(length)])


class SendThread(threading.Thread):
    def __init__(self, sock, send_len):
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.sock = sock
        self.send_len = send_len
        self.exit_event = threading.Event()
        pass

    def exit(self):
        self.exit_event.set()

    def run(self):
        index = 0
        while self.exit_event.isSet() is False:
            data = make_validation_data(self.send_len, index)
            try:
                self.sock.send(data)
                index += self.send_len
            except StandardError:
                # pass but not exit thread
                time.sleep(1)
                continue
    pass


class RecvThread(threading.Thread):
    def __init__(self, sock):
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.sock = sock
        self.exit_event = threading.Event()

    def exit(self):
        self.exit_event.set()

    def run(self):
        index = 0
        while self.exit_event.isSet() is False:
            if self.sock is not None:
                try:
                    data = self.sock.recv(8*1024)
                except StandardError, e:
                    NativeLog.add_exception_log(e)
                    NativeLog.add_trace_critical("recv error, connection closed")
                    break
                if verify_data(data, index) is not True:
                    break
                index += len(data)
            else:
                time.sleep(1)
    pass


class ValidationThread(threading.Thread):
    def __init__(self, tc_action):
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.tc_action = tc_action
        self.exit_event = threading.Event()

    def exit(self):
        self.exit_event.set()

    def run(self):
        while self.exit_event.isSet() is False:
            if self.tc_action.check_response("SSC1", "DATA_ERROR", 5) is True:
                NativeLog.add_trace_critical("[Data Validation Error] target recv data error")
                break
    pass


class TestCase(PerformanceTCBase.PerformanceTCBase):

    def __init__(self, test_case, test_env, timeout=30, log_path=TCActionBase.LOG_PATH):
        PerformanceTCBase.PerformanceTCBase.__init__(self, test_case, test_env, timeout=timeout, log_path=log_path)
        self.send_len = 0
        self.tx_enable = None
        self.rx_enable = None
        self.conn_num = 0
        self.test_time = 0
        # load param from excel
        cmd_set = test_case["cmd set"]
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        pass

    def execute(self):
        TCActionBase.TCActionBase.execute(self)

        try:
            # configurable params
            send_len = self.send_len
            tx_enable = self.tx_enable
            rx_enable = self.rx_enable
            conn_num = self.conn_num
            test_time = self.test_time * 60  # convert minutes to seconds
            # configurable params
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for TCPThroughput script, error is %s" % e)
            raise StandardError("Error configuration")

        # init throughput result data
        test_item = ""
        if tx_enable is True:
            test_item += "Tx"
        if rx_enable is True:
            test_item += "Rx"
        if test_item == "":
            raise StandardError("no throughput test item")

        pc_ip = self.get_parameter("pc_ip")
        tcp_port = random.randint(10000, 50000)

        # disable recv print during throughput test
        self.serial_write_line("SSC1", "soc -R -o 0")
        if self.check_response("SSC1", "+RECVPRINT", 2) is False:
            NativeLog.add_trace_critical("Fail, Fail to disable recv print")

        # create server
        server_sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
        server_sock.bind((pc_ip, tcp_port))
        server_sock.settimeout(5)
        server_sock.listen(5)

        sock_id_list = []
        send_thread_list = []
        recv_thread_list = []

        # step 4 create tcp connection
        for i in range(conn_num):
            self.serial_write_line("SSC1", "soc -B -t TCP")
            match = self.check_regular_expression("SSC1", re.compile("\+BIND:(\d+),OK"), 2)
            if match is None:
                NativeLog.add_trace_critical("Fail, Fail to bind")
                return
            else:
                sock_id_list.append(int(match.group(1)))

            self.serial_write_line("SSC1", "soc -V -s %s -o 3" % sock_id_list[-1])
            if self.check_regular_expression("SSC1", re.compile("\+DATA_VALIDATION:\d+,OK"), 2) is None:
                NativeLog.add_trace_critical("Fail, Failed to enable validation")
                return

            self.serial_write_line("SSC1", "soc -C -s %s -i %s -p %s" % (sock_id_list[-1], pc_ip, tcp_port))
            try:
                sock, addr = server_sock.accept()
            except socket.error, e:
                NativeLog.add_trace_critical("%s" % e)
                raise e

            if self.check_regular_expression("SSC1", re.compile("\+CONNECT:\d+,OK"), 5) is None:
                NativeLog.add_trace_critical("Fail, Failed to connect")
                return

            sock.settimeout(10)

            send_thread_list.append(SendThread(sock if rx_enable is True else None, send_len))
            recv_thread_list.append(RecvThread(sock if tx_enable is True else None))
            recv_thread_list[-1].start()

        # step 5 do test
        validation_thread = ValidationThread(self)
        validation_thread.start()

        for send_thread in send_thread_list:
            send_thread.start()

        if tx_enable is True:
            # do send from target
            for sock_id in sock_id_list:
                self.serial_write_line("SSC1", "soc -S -s %s -l %s -n 10000000" % (sock_id, send_len))

        time1 = time.time()
        exit_flag = False

        while time.time() - time1 < test_time and exit_flag is False:
            for i in sock_id_list:
                send_thread_list[i].join(0.5)
                recv_thread_list[i].join(0.5)
                validation_thread.join(0.5)
                if send_thread_list[i].isAlive() is False \
                        or recv_thread_list[i].isAlive() is False \
                        or validation_thread.isAlive() is False:
                    NativeLog.add_trace_critical("validation error found")
                    exit_flag = True
                    break

        NativeLog.add_prompt_trace("time escape: %s" % (time.time() - time1))

        if time.time() - time1 >= test_time:
            self.set_result("Succeed")
        else:
             self.set_result("Failed")

        # exit all thread
        for i in sock_id_list:
            send_thread_list[i].exit()
            recv_thread_list[i].exit()
            send_thread_list[i].join()
            send_thread_list[i].join()

        validation_thread.exit()
        validation_thread.join()


def main():
    pass

if __name__ == '__main__':
    main()
