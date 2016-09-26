from TCAction import PerformanceTCBase
import time
import socket
import threading
import Queue
import re
import random
from NativeLog import NativeLog


SEND_CMD = ("CIPSEND, CIPSENDBUF", "CIPSENDEX")


class RecvThread(threading.Thread):
    def __init__(self, test_action):
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.test_action = test_action
        self.exit_flag = threading.Event()
        pass

    def run(self):
        data = ""
        ipd_line = re.compile("IPD,\d,\d+:")
        recv_bytes_line = re.compile("Recv \d+ bytes")
        allow_send_line = re.compile("OK\r\n>")
        send_ok_line = re.compile("SEND OK")
        while self.exit_flag.is_set() is False:
            flush_pos = 0
            data += self.test_action.serial_read_data("AT1")
            # do process IPD data
            match_set = ipd_line.findall(data)
            for match_line in match_set:
                link_id = match_line[4]
                flush_pos = data.find(match_line) + len(match_line)
                self.test_action.send_queue.put(link_id, 1)
                pass
            # do process send >
            match = allow_send_line.search(data)
            if match is not None:
                match_line = match.group()
                self.test_action.add_info_log("find OK >")
                self.test_action.send_allow_evt.set()
                pos = data.find(match_line) + len(match_line)
                flush_pos = pos if pos > flush_pos else flush_pos
            # do process Recv xx bytes
            match = recv_bytes_line.search(data)
            if match is not None:
                match_line = match.group()
                self.test_action.add_info_log("find Recv xx bytes")
                self.test_action.recv_data_evt.set()
                pos = data.find(match_line) + len(match_line)
                flush_pos = pos if pos > flush_pos else flush_pos

            match = send_ok_line.search(data)
            if match is not None:
                match_line = match.group()
                self.test_action.add_info_log("find send ok")
                self.test_action.send_ok_evt.set()
                pos = data.find(match_line) + len(match_line)
                flush_pos = pos if pos > flush_pos else flush_pos
            # pass

            # flush processed data
            if flush_pos > 0:
                data = data[flush_pos:]

            pass

    def exit(self):
        self.exit_flag.set()
    pass


class TCPClientThread(threading.Thread):
    send_char = "A"
    sync_lock = threading.Lock()

    def __init__(self, test_action, pc_ip, target_ip, target_port, request_len, response_len, client_id,
                 connect_timeout, recv_timeout):
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.exit_flag = threading.Event()
        self.test_action = test_action
        self.pc_ip = pc_ip
        self.target_ip = target_ip
        self.target_port = target_port
        self.request_len = request_len
        self.response_len = response_len
        self.client_id = client_id
        self.connect_timeout = connect_timeout
        self.recv_timeout = recv_timeout
        pass

    @classmethod
    def get_send_char(cls):
        with cls.sync_lock:
            send_char = cls.send_char
            cls.send_char = chr(ord(send_char) + 1) if ord(send_char) < ord("Z") else "A"
        return send_char
        pass

    def run(self):
        while self.exit_flag.is_set() is False:
            exception_occurred = False
            client_sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
            client_sock.bind((self.pc_ip, 0))
            client_sock.settimeout(20)
            time1 = time.time()
            name = client_sock.getsockname()

            try:
                client_sock.connect((self.target_ip, self.target_port))
            except StandardError, e:
                exception_occurred = True
                self.test_action.add_critical_log("failed to connect succeed within 2 seconds %s, %d"
                                                  % (name[0], name[1]))
                client_sock.close()

            time2 = time.time() - time1
            if exception_occurred is True:
                self.test_action.add_critical_log("connect timeout %f; ip is %s, port is %d"
                                                  % (time2, name[0], name[1]))
                continue
            if time2 > self.connect_timeout:
                self.test_action.add_critical_log("connect time too long %f; ip is %s, port is %d"
                                                  % (time2, name[0], name[1]))

            time.sleep(float(random.randint(0, 30))/100)
            send_char = self.get_send_char()
            data = send_char * self.request_len
            try:
                client_sock.send(data)
            except StandardError:
                NativeLog.add_trace_critical("send fail")
            # try:
            #     data = client_sock.recv(1)
            # except socket.error, e:
            #     self.handle_processing_fail("failed to receive data within 2 seconds")
            data_received = 0
            time1 = time.time()
            while data_received < self.response_len:
                try:
                    data = client_sock.recv(4*1024)
                except StandardError, e:
                    exception_occurred = True
                    break
                data_received += len(data)

            time2 = time.time() - time1
            if exception_occurred is True or time2 > self.recv_timeout:
                self.test_action.add_critical_log("receive time too long %f; ip is %s, port is %d"\
                                                  % (time2, name[0], name[1]))
            client_sock.close()
            time.sleep(float(random.randint(0, 30))/100)
            pass
        pass

    def exit(self):
        self.exit_flag.set()
    pass


class SendThread(threading.Thread):
    def __init__(self, test_action, test_count, send_cmd, response_len, check_send_ok):
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.test_action = test_action
        self.test_count = test_count
        self.send_cmd = send_cmd
        self.response_len = response_len
        self.check_send_ok = check_send_ok
        pass

    def run(self):
        send_char = "a"
        for i in xrange(self.test_count):
            link_id = self.test_action.send_queue.get(1)

            self.test_action.send_allow_evt.clear()
            self.test_action.serial_write_line("AT1", "AT+%s=%s,%d" % (self.send_cmd, link_id, self.response_len))
            self.test_action.add_info_log("write CIPSEND cmd")

            self.test_action.send_allow_evt.wait(10)
            if self.test_action.send_allow_evt.is_set() is False:
                self.test_action.add_critical_log("Failed to find OK > in 10s, test break")
                break
            self.test_action.send_allow_evt.clear()

            data = send_char * self.response_len
            send_char = chr(ord(send_char) + 1) if ord(send_char) < ord("z") else "a"
            self.test_action.recv_data_evt.clear()
            self.test_action.send_ok_evt.clear()
            self.test_action.serial_write("AT1", data)
            self.test_action.add_info_log("data write done")
            self.test_action.recv_data_evt.wait(10)
            if self.test_action.recv_data_evt.is_set() is False:
                self.test_action.add_critical_log("Failed to find Recv xx bytes in 10s, test break")
                break
            self.test_action.recv_data_evt.clear()
            # if self.test_action.send_cmd == "CIPSEND":
            if self.check_send_ok is True:
                self.test_action.send_ok_evt.wait(10)
                if self.test_action.send_ok_evt.is_set() is False:
                    self.test_action.add_critical_log("Failed to find SEND OK in 10s, test break")
                    break
                self.test_action.add_info_log("send ok")
                self.test_action.send_ok_evt.clear()
            pass
    pass


class SoftAPServer(PerformanceTCBase.PerformanceTCBase):
    def __init__(self, name, test_env, cmd_set, timeout=120, log_path=None):
        PerformanceTCBase.PerformanceTCBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                     timeout=timeout, log_path=log_path)
        # init value for ip and port
        self.pc_ip = "pc_ip"
        self.server_port = "test_tcp_port1"
        self.send_cmd = "CIPSEND"
        self.baudrate = 115200
        self.rtscts = 3
        self.test_count = 1000
        self.request_len = 500
        self.response_len = 1600
        self.check_send_ok = True
        self.concurrent_connections = 5
        self.connect_timeout = 3
        self.receive_timeout = 2
        # load param from excel
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        self.send_queue = Queue.Queue(maxsize=100)
        self.send_allow_evt = threading.Event()
        self.recv_data_evt = threading.Event()
        self.send_ok_evt = threading.Event()

        pass

    @staticmethod
    def add_critical_log(data):
        NativeLog.add_trace_critical(data+"\r\n")
        pass

    @staticmethod
    def add_info_log(data):
        NativeLog.add_trace_info(data)

    def process(self):
        # step0, use initial condition AP3 (8266 as AP, PC connected to 8266, multiple connection)
        pc_ip = self.get_parameter(self.pc_ip)
        target_ip = self.get_parameter("target_ip")
        server_port = self.get_parameter(self.server_port)
        send_cmd = self.send_cmd
        test_count = self.test_count
        baudrate = self.baudrate
        rtscts = self.rtscts
        concurrent_connections = self.concurrent_connections
        check_send_ok = self.check_send_ok
        connect_timeout = self.connect_timeout
        receive_timeout = self.receive_timeout

        self.serial_write_line("AT1", "AT+UART_CUR=%d,8,1,0,%d" % (baudrate, rtscts))
        self.check_response("AT1", "OK\r\n")
        self.reconfig_serial_port("AT1", baudrate, rtscts)
        # step1, create server on 8266, create client thread
        self.serial_write_line("AT1", "AT+CIPSERVER=1,%d" % server_port)
        self.check_response("AT1", "OK")

        recv_thread = RecvThread(self)
        send_thread = SendThread(self, test_count, send_cmd, self.response_len, check_send_ok)
        send_thread.start()
        recv_thread.start()
        client_thread_list = [None] * concurrent_connections
        for i in range(concurrent_connections):
            client_thread_list[i] = TCPClientThread(self, pc_ip, target_ip, server_port,
                                                    self.request_len, self.response_len, i,
                                                    connect_timeout, receive_timeout)
            client_thread_list[i].start()
            pass

        # step3, wait sending thread join
        send_thread.join()

        recv_thread.exit()
        recv_thread.join()

        for i in range(concurrent_connections):
            client_thread_list[i].exit()
            client_thread_list[i].join()
            pass

        self.serial_write_line("AT1", "AT+UART_CUR=115200,8,1,0,3")
        self.check_response("AT1", "OK\r\n")
        self.restore_serial_port("AT1")
        self.set_result("Succeed")
        pass
    pass


def main():
    pass


if __name__ == '__main__':
    main()
