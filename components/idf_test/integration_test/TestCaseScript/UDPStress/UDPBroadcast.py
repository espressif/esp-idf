import time
import random
import threading
import socket
import re

from TCAction import PerformanceTCBase
from NativeLog import NativeLog
from Utility import Encoding


class SendThread(threading.Thread):
    def __init__(self, sock, send_len, target_addr, delay):
        threading.Thread.__init__(self)
        self.sock = sock
        self.send_len = send_len
        self.target_addr = target_addr
        self.delay = delay
        self.exit_event = threading.Event()
        self.send_count = 0
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
            self.send_count += 1
            time.sleep(self.delay * 0.001)

    pass

    def get_send_count(self):
        return self.send_count


class RecvThread(threading.Thread):
    def __init__(self, sock):
        threading.Thread.__init__(self)
        self.sock = sock
        self.exit_event = threading.Event()
        self.calc_event = threading.Event()
        self.bytes_recv = 0
        self.Max = 0.0

    def start_calc(self):
        self.calc_event.set()

    def stop_calc(self):
        self.calc_event.clear()
        self.exit_event.set()

    def run(self):
        if self.sock is None:
            return
        while True:
            if self.exit_event.isSet() is True:
                break
            try:
                data, addr = self.sock.recvfrom(2048)
            except StandardError:
                break
            if self.calc_event.isSet() is True:
                self.bytes_recv += len(data)
            if len(data) == 0:
                start = time.time()
                while True:
                    try:
                        data, addr = self.sock.recvfrom(2048)
                    except StandardError:
                        break
                    if len(data) > 0:
                        if self.calc_event.isSet() is True:
                            self.bytes_recv += len(data)
                        end = time.time()
                        break
            if end - start > self.Max:
                self.Max = end - start

    def get_bytes_recv(self):
        return self.bytes_recv

    pass

    def get_Max_time(self):
        return self.Max

    pass


class device_check(threading.Thread):
    def __init__(self, port):
        threading.Thread.__init__(self)
        self.Max = 0.0
        self.port = port
        self.recv_data_cache = ""
        self.cache_lock = threading.Lock()
        self.exit_event = threading.Event()

    def data_recv_callback(self, data):
        with self.cache_lock:
            self.recv_data_cache += data
        pass

    def exit(self):
        self.exit_event.set()
        pass

    def run(self):
        while self.exit_event.isSet() is False:
            while True:
                if self.recv_data_cache:
                    match = re.search("\+RECVFROM:\d+,\d+,\d+\.\d+\.\d+\.\d+,\d+", self.recv_data_cache)
                    if match is not None:
                        self.recv_data_cache = self.recv_data_cache[len(match.group()):]
                    else:
                        start = time.time()
                        end = 0.0
                        while True:
                            res = re.search("\+RECVFROM:\d+,\d+,\d+\.\d+\.\d+\.\d+,\d+", self.recv_data_cache)
                            if res is not None:
                                self.recv_data_cache = self.recv_data_cache[len(res.group()):]
                                end = time.time()
                                break
                        if end - start > self.Max:
                            self.Max = end - start
            pass

    def get_max_time(self):
        return self.Max


class TestCase(PerformanceTCBase.PerformanceTCBase):
    def __init__(self, test_case, test_env, timeout, log_path):
        PerformanceTCBase.PerformanceTCBase.__init__(self, test_case, test_env, timeout, log_path)
        self.send_len = 0
        self.pc_send = 0
        self.target_send = 0
        self.test_time = 0
        self.delay = 0
        # load param from excel
        cmd_set = test_case["cmd set"]
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        self.recv_cb_lock = threading.Lock()
        self.recv_cb = dict.fromkeys(["SSC1"])
        pass

    def register_recv_callback(self, port_name, callback):
        with self.recv_cb_lock:
            if self.recv_cb[port_name] is None:
                self.recv_cb[port_name] = [callback]
            else:
                self.recv_cb[port_name].append(callback)
        pass

    def process(self):
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
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for UDP script, error is %s" % e)
            raise StandardError("Error configuration")

        udp_port = random.randint(40000, 50000)

        # reboot before executing
        self.flush_data("SSC1")
        self.serial_write_line("SSC1", "reboot")
        if self.check_response("SSC1", "ready!!!", 5) is False:
            NativeLog.add_trace_critical("Fail to reboot")
            return

        # set target as STA mode
        self.flush_data("SSC1")
        self.serial_write_line("SSC1", "op -S -o 1")
        if self.check_response("SSC1", "+MODE:OK", 5) is False:
            NativeLog.add_trace_critical("Fail to set mode")
            return

        # connect to AP
        self.flush_data("SSC1")
        self.serial_write_line("SSC1", "sta -C -s %s -p %s" % (ap_ssid, ap_password))
        if self.check_response("SSC1", "+JAP:CONNECTED", 30) is False:
            NativeLog.add_trace_critical("Fail to JAP")
            return

        # disable recv print on target
        self.flush_data("SSC1")
        self.serial_write_line("SSC1", "soc -R -o 0")
        if self.check_response("SSC1", "+RECVPRINT", 5) is False:
            NativeLog.add_trace_critical("Fail to disable recv print")
            return

        # get broadcast ip
        res = re.search("(\d+\.\d+\.\d+\.)\d+", pc_ip)
        if res is not None:
            udp = res.group(1)
            broadcast_ip = udp + "255"
        else:
            NativeLog.add_trace_critical("No ip addr found")
            return

        # close all connection on target
        self.flush_data("SSC1")
        self.serial_write_line("SSC1", "soc -T")
        if self.check_response("SSC1", "+CLOSEALL", 5) is False:
            NativeLog.add_trace_critical("Fail to close sock")
            return

        # create socket on pc
        pc_sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
        pc_sock.bind((pc_ip, udp_port))
        pc_sock.settimeout(1)

        # create socket on target
        self.flush_data("SSC1")
        self.serial_write_line("SSC1", "soc -B -t UDP -i %s -p %s" % (target_ip, udp_port))
        if self.check_response("SSC1", "+BIND:0,OK,", 5) is False:
            NativeLog.add_trace_critical("Fail to bind")
            return

        thread_dict = dict.fromkeys(["SSC1"])
        thread_dict["SSC1"] = dict(zip(["check"], [None]))
        thread_dict["SSC1"]["check"] = device_check(self.test_env.get_port_by_name("SSC1"))
        self.register_recv_callback("SSC1", thread_dict["SSC1"]["check"].data_recv_callback)

        send_thread = SendThread(pc_sock if pc_send is True else None, send_len, (broadcast_ip, udp_port), delay)
        send_thread.start()

        recv_thread = RecvThread(pc_sock if target_send is True else None)
        recv_thread.start()

        # start calculate
        recv_thread.start_calc()
        thread_dict["SSC1"]["check"].start()
        send_count = 0
        if target_send is True:
            # do send from target
            start = time.time()
            while time.time() - start < test_time * 60:
                self.flush_data("SSC1")
                self.serial_write_line("SSC1", "soc -S -s 0 -l %s -n 1000  -i %s -p %s -j %s" % (
                    send_len, broadcast_ip, udp_port, delay))
                if self.check_response("SSC1", "+SEND:0,OK", 300) is False:
                    NativeLog.add_trace_critical("Fail to send")
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

        Max = 0.0
        recv_count = 0
        if pc_send is True:
            send_count = send_thread.get_send_count()
            start = time.time()
            rx_data_len = 0
            suc_time = 0
            while time.time() - start < 30:
                self.flush_data("SSC1")
                self.serial_write_line("SSC1", "soc -Q -s 0 -o 1")
                time.sleep(0.05)
                data = self.serial_read_data("SSC1")
                if data is not None:
                    res = re.search("\+RECVLEN:(\d+)", data)
                    if res is not None:
                        if rx_data_len < int(res.group(1)):
                            rx_data_len = int(res.group(1))
                            time.sleep(0.5)
                        else:
                            suc_time += 1
                if suc_time > 5:
                    break

            if (rx_data_len * 8 % send_len) > 0:
                recv_count = rx_data_len / send_len + 1
            else:
                recv_count = rx_data_len / send_len

        if recv_thread.get_bytes_recv() > 0:
            if (recv_thread.get_bytes_recv() % send_len) > 0:
                recv_count = recv_thread.get_bytes_recv() / send_len + 1
            else:
                recv_count = recv_thread.get_bytes_recv() / send_len
            Max = recv_thread.get_Max_time()

        thread_dict["SSC1"]["check"].exit()
        pc_sock.close()

        self.set_result("Succeed")
        NativeLog.add_trace_critical("send_count is %s, recv_count is %s" % (send_count, recv_count))
        NativeLog.add_trace_critical(
            "UDP Broadcast lose rate is %.2f%%" % (float(send_count - recv_count) / send_count * 100))
        NativeLog.add_trace_critical("UDP Broadcast lose test MAX time is %.4f" % Max)

    @Encoding.encode_utf8(3)
    def result_check(self, port_name, data):
        PerformanceTCBase.PerformanceTCBase.result_check(self, port_name, data)
        if port_name in self.recv_cb:
            with self.recv_cb_lock:
                callback_list = self.recv_cb[port_name]
            if callback_list is not None:
                for callback in callback_list:
                    callback(data)
        pass


def main():
    pass


if __name__ == '__main__':
    main()
