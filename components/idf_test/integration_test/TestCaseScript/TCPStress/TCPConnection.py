import random
import re
import socket
import threading
import time

import TCPConnectionUtility
from NativeLog import NativeLog
from TCAction import PerformanceTCBase

DELAY_RANGE = [10, 3000]
CONNECTION_STRUCTURE = ("Connection handler", "PC socket", "Target socket id",
                        "Target port", "PC port", "PC state", "Target state")

# max fail count for one connection during test
MAX_FAIL_COUNT = 10


class CheckerBase(threading.Thread):

    CHECK_ITEM = ("CONDITION", "NOTIFIER", "ID", "DATA")
    SLEEP_TIME = 0.1  # sleep 100ms between each check action

    def __init__(self):
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.exit_event = threading.Event()
        self.sync_lock = threading.Lock()
        self.check_item_list = []
        self.check_item_id = 0

    def run(self):
        while self.exit_event.isSet() is False:
            self.process()
            pass

    def process(self):
        pass

    def add_check_item(self, condition, notifier):
        with self.sync_lock:
            check_item_id = self.check_item_id
            self.check_item_id += 1
            self.check_item_list.append(dict(zip(self.CHECK_ITEM, (condition, notifier, check_item_id, str()))))
        return check_item_id

    def remove_check_item(self, check_item_id):
        ret = None
        with self.sync_lock:
            check_items = filter(lambda x: x["ID"] == check_item_id, self.check_item_list)
            if len(check_items) > 0:
                self.check_item_list.remove(check_items[0])
                ret = check_items[0]["DATA"]
        return ret

    def exit(self):
        self.exit_event.set()
    pass


# check on serial port
class SerialPortChecker(CheckerBase):
    def __init__(self, serial_reader):
        CheckerBase.__init__(self)
        self.serial_reader = serial_reader
        pass

    # check condition for serial is compiled regular expression pattern
    @staticmethod
    def do_check(check_item, data):
        match = check_item["CONDITION"].search(data)
        if match is not None:
            pos = data.find(match.group()) + len(match.group())
            # notify user
            check_item["NOTIFIER"]("serial", match)
        else:
            pos = -1
        return pos

    def process(self):
        # do check
        with self.sync_lock:
            # read data
            new_data = self.serial_reader()
            # NativeLog.add_trace_info("[debug][read data] %s" % new_data)
            # do check each item
            for check_item in self.check_item_list:
                # NativeLog.add_trace_info("[debug][read data][ID][%s]" % check_item["ID"])
                check_item["DATA"] += new_data
                self.do_check(check_item, check_item["DATA"])
        time.sleep(self.SLEEP_TIME)


# handle PC TCP server accept and notify user
class TCPServerChecker(CheckerBase):
    def __init__(self, server_sock):
        CheckerBase.__init__(self)
        self.server_sock = server_sock
        server_sock.settimeout(self.SLEEP_TIME)
        self.accepted_socket_list = []

    # check condition for tcp accepted sock is tcp source port
    @staticmethod
    def do_check(check_item, data):
        for sock_addr_pair in data:
            addr = sock_addr_pair[1]
            if addr[1] == check_item["CONDITION"]:
                # same port, so this is the socket that matched, notify and remove it from list
                check_item["NOTIFIER"]("tcp", sock_addr_pair[0])
                data.remove(sock_addr_pair)

    def process(self):
        # do accept
        try:
            client_sock, addr = self.server_sock.accept()
            self.accepted_socket_list.append((client_sock, addr))
        except socket.error:
            pass
        # do check
        with self.sync_lock:
            check_item_list = self.check_item_list
            for check_item in check_item_list:
                self.do_check(check_item, self.accepted_socket_list)
    pass


# this thread handles one tcp connection.
class ConnectionHandler(threading.Thread):
    CHECK_FREQ = CheckerBase.SLEEP_TIME/2

    def __init__(self, utility, serial_checker, tcp_checker, connect_method, disconnect_method, test_case):
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.utility = utility
        self.connect_method = connect_method
        self.disconnect_method = disconnect_method
        self.exit_event = threading.Event()
        # following members are used in communication with checker threads
        self.serial_checker = serial_checker
        self.tcp_checker = tcp_checker
        self.serial_notify_event = threading.Event()
        self.tcp_notify_event = threading.Event()
        self.serial_result = None
        self.tcp_result = None
        self.serial_check_item_id = None
        self.tcp_check_item_id = None
        self.data_cache = None
        self.fail_count = 0
        self.test_case = test_case
        pass

    def log_error(self):
        self.fail_count += 1
        if self.fail_count > MAX_FAIL_COUNT:
            self.test_case.error_detected()

    def new_connection_structure(self):
        connection = dict.fromkeys(CONNECTION_STRUCTURE, None)
        connection["Connection handler"] = self
        return connection

    def run(self):
        while self.exit_event.isSet() is False:
            connection = self.new_connection_structure()
            # do connect
            connect_method_choice = random.choice(self.connect_method)
            if self.utility.execute_tcp_method(connect_method_choice, connection) is False:
                self.log_error()
            # check if established
            if self.utility.is_established_state(connection) is True:
                time.sleep(float(random.randint(DELAY_RANGE[0], DELAY_RANGE[1]))/1000)
                # do disconnect if established
                disconnect_method_choice = random.choice(self.disconnect_method)
                if self.utility.execute_tcp_method(disconnect_method_choice, connection) is False:
                    self.log_error()
            # make sure target socket closed
            self.utility.close_connection(connection)
            time.sleep(float(random.randint(DELAY_RANGE[0], DELAY_RANGE[1]))/1000)
        pass

    # serial_condition: re string
    # tcp_condition: target local port
    def add_checkers(self, serial_condition=None, tcp_condition=None):
        # cleanup
        self.serial_result = None
        self.tcp_result = None
        self.serial_notify_event.clear()
        self.tcp_notify_event.clear()
        # serial_checker
        if serial_condition is not None:
            pattern = re.compile(serial_condition)
            self.serial_check_item_id = self.serial_checker.add_check_item(pattern, self.notifier)
        else:
            # set event so that serial check always pass
            self.serial_notify_event.set()
        if tcp_condition is not None:
            self.tcp_check_item_id = self.tcp_checker.add_check_item(tcp_condition, self.notifier)
        else:
            # set event so that tcp check always pass
            self.tcp_notify_event.set()
        # NativeLog.add_trace_info("[Debug] add check item %s, connection is %s" % (self.serial_check_item_id, self))
        pass

    def get_checker_results(self, timeout=5):
        time1 = time.time()
        while time.time() - time1 < timeout:
            # if one type of checker is not set, its event will be set in add_checkers
            if self.serial_notify_event.isSet() is True and self.tcp_notify_event.isSet() is True:
                break
            time.sleep(self.CHECK_FREQ)
        # do cleanup
        # NativeLog.add_trace_info("[Debug] remove check item %s, connection is %s" % (self.serial_check_item_id, self))
        self.data_cache = self.serial_checker.remove_check_item(self.serial_check_item_id)
        self.tcp_checker.remove_check_item(self.tcp_check_item_id)
        # self.serial_check_item_id = None
        # self.tcp_check_item_id = None
        return self.serial_result, self.tcp_result

    def notifier(self, typ, result):
        if typ == "serial":
            self.serial_notify_event.set()
            self.serial_result = result
        elif typ == "tcp":
            self.tcp_notify_event.set()
            self.tcp_result = result

    def exit(self):
        self.exit_event.set()
    pass


class TestCase(PerformanceTCBase.PerformanceTCBase):
    def __init__(self, test_case, test_env, timeout=120, log_path=None):
        PerformanceTCBase.PerformanceTCBase.__init__(self, test_case, test_env,
                                                     timeout=timeout, log_path=log_path)
        self.max_connection = 5
        self.execute_time = 120  # execute time default 120 minutes
        self.pc_ip = "pc_ip"
        self.target_ip = "target_ip"
        self.connect_method = ["C_01"]
        self.disconnect_method = ["D_05"]

        cmd_set = test_case["cmd set"]
        # load param from excel
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string

        self.error_event = threading.Event()
        self.serial_lock = threading.Lock()
        pass

    def serial_reader(self):
        return self.serial_read_data("SSC1")

    def send_ssc_command(self, data):
        with self.serial_lock:
            time.sleep(0.05)
            self.serial_write_line("SSC1", data)

    def error_detected(self):
        self.error_event.set()

    def process(self):
        # parameters
        max_connection = self.max_connection
        execute_time = self.execute_time * 60
        pc_ip = self.get_parameter(self.pc_ip)
        target_ip = self.get_parameter(self.target_ip)
        connect_method = self.connect_method
        disconnect_method = self.disconnect_method
        server_port = random.randint(30000, 50000)

        # step 1, create TCP server on target and PC
        # create TCP server on target
        self.serial_write_line("SSC1", "soc -B -t TCP -p %s" % server_port)
        match = self.check_regular_expression("SSC1", re.compile("BIND:(\d+),OK"))
        if match is None:
            NativeLog.add_prompt_trace("Failed to create TCP server on target")
            return
        target_sock_id = match.group(1)

        self.serial_write_line("SSC1", "soc -L -s %s" % target_sock_id)
        if self.check_response("SSC1", "+LISTEN:%s,OK" % target_sock_id) is False:
            NativeLog.add_prompt_trace("Failed to create TCP server on target")
            return

        # create TCP server on PC
        try:
            server_sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
            server_sock.bind((pc_ip, server_port))
            server_sock.listen(5)
        except StandardError:
            NativeLog.add_prompt_trace("Failed to create TCP server on PC")
            return

        # step 2, create checker
        serial_port_checker = SerialPortChecker(self.serial_reader)
        tcp_server_checker = TCPServerChecker(server_sock)
        serial_port_checker.start()
        tcp_server_checker.start()

        # step 3, create 5 thread and do connection
        utility = TCPConnectionUtility.Utility(self, server_port, server_port, pc_ip, target_ip)
        work_thread = []
        for i in range(max_connection):
            t = ConnectionHandler(utility, serial_port_checker, tcp_server_checker,
                                  connect_method, disconnect_method, self)
            work_thread.append(t)
            t.start()

        # step 4, wait and exit
        self.error_event.wait(execute_time)
        # close all threads
        for t in work_thread:
            t.exit()
            t.join()
        serial_port_checker.exit()
        tcp_server_checker.exit()
        serial_port_checker.join()
        tcp_server_checker.join()

        if self.error_event.isSet() is False:
            # no error detected
            self.set_result("Succeed")
        pass


def main():
    pass


if __name__ == '__main__':
    main()
