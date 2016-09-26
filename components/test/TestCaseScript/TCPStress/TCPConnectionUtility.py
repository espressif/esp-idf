import random
import socket
import threading

from NativeLog import NativeLog

# from NativeLog import NativeLog

# make sure target do not listen on this port
ERROR_PORT = 62685


class Utility(object):
    METHOD_RESULT = {"C_01": ("ESTABLISHED", "ESTABLISHED"),  # target TCP peer state, PC TCP peer state
                     "C_02": ("SYNC_SENT", "CLOSED"),
                     "C_03": ("CLOSED", "CLOSED"),
                     "C_04": ("SYN_RCVD", "ESTABLISHED"),
                     "C_05": ("ESTABLISHED", "ESTABLISHED"),
                     "C_06": ("CLOSED", "CLOSED"),
                     "C_07": ("CLOSED", "CLOSED"),
                     "C_08": ("CLOSED", "CLOSED"),
                     "D_01": ("TIME_WAIT", "CLOSED"),
                     "D_02": ("TIME_WAIT", "TIME_WAIT"),
                     "D_03": ("FIN_WAIT_2", "CLOSE_WAIT"),
                     "D_04": ("FIN_WAIT_1", "CLOSE_WAIT"),
                     "D_05": ("CLOSED", "TIME_WAIT"),
                     "D_06": ("CLOSED", "CLOSED"),
                     "D_07": ("CLOSE_WAIT", "FIN_WAIT2"),
                     "D_08": ("TIME_WAIT", "CLOSED"), }

    SOC_CLOSED_STATE = ("FIN_WAIT_1", "FIN_WAIT_2", "CLOSING", "TIME_WAIT", "LAST_ACK", "CLOSED")
    SOC_CREATED_STATE = ("SYNC_RCVD", "ESTABLISHED")
    SOC_SEND_DATA_STATE = ("ESTABLISHED", "CLOSE_WAIT")
    SOC_ESTABLISHED_STATE = ("ESTABLISHED", )

    def __init__(self, tc_action, pc_server_port, target_server_port, pc_ip, target_ip):
        self.tc_action = tc_action
        self.pc_server_port = pc_server_port
        self.target_server_port = target_server_port
        self.pc_ip = pc_ip
        self.target_ip = target_ip
        self.pc_close_wait_socket_list = []
        self.sync_lock = threading.Lock()
        pass

    # create a tcp socket, return True or False
    def __create_tcp_socket(self, connection):
        connection_handler = connection["Connection handler"]
        connection["Target port"] = random.randint(10000, 60000)
        connection_handler.add_checkers("BIND:(\d+),OK,%s,%s"
                                        % (self.target_ip, connection["Target port"]))
        self.tc_action.send_ssc_command("soc -B -t TCP -i %s -p %s" % (self.target_ip, connection["Target port"]))
        serial_result, tcp_result = connection_handler.get_checker_results()
        if serial_result is not None:
            connection["Target socket id"] = serial_result.group(1)
            return True
        else:
            return False

    # target do connect, return True or False
    def __target_do_connect(self, connection, dest_ip, dest_port, timeout=20):
        connection_handler = connection["Connection handler"]
        connection_handler.add_checkers("CONNECT:%s,OK" % connection["Target socket id"],
                                        connection["Target port"])
        self.tc_action.send_ssc_command("soc -C -s %s -i %s -p %s"
                                        % (connection["Target socket id"], dest_ip, dest_port))
        serial_result, tcp_result = connection_handler.get_checker_results(timeout)
        if serial_result is not None and tcp_result is not None:
            connection["PC socket"] = tcp_result
            return True
        else:
            return False
        pass

    # pc do connect, return True or False
    def __pc_do_connect(self, connection, dest_ip, dest_port, timeout=20):
        connection_handler = connection["Connection handler"]
        sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
        while True:
            connection["PC port"] = random.randint(10000, 60000)
            try:
                sock.bind((self.pc_ip, connection["PC port"]))
                break
            except socket.error, e:
                if e.errno == 10048:  # socket port reuse
                    continue
        sock.settimeout(timeout)
        connection["PC socket"] = sock
        connection_handler.add_checkers("ACCEPT:(\d+),\d+,%s,%s"
                                        % (self.pc_ip, connection["PC port"]))
        try:
            sock.connect((dest_ip, dest_port))
        except socket.error:
            pass
        serial_result, tcp_result = connection_handler.get_checker_results()
        if serial_result is not None:
            connection["Target socket id"] = serial_result.group(1)
            return True
        else:
            return False
        pass

    def connect_c_01(self, connection):
        if self.__create_tcp_socket(connection) is True:
            return self.__target_do_connect(connection, self.pc_ip, self.pc_server_port)
        else:
            return False

    def connect_c_02(self, connection):
        if self.__create_tcp_socket(connection) is True:
            return not self.__target_do_connect(connection, self.pc_ip, ERROR_PORT, timeout=5)
        else:
            return False

    def connect_c_03(self, connection):
        return False

    def connect_c_04(self, connection):
        return False

    def connect_c_05(self, connection):
        return self.__pc_do_connect(connection, self.target_ip, self.target_server_port)

    def connect_c_06(self, connection):
        return False

    def connect_c_07(self, connection):
        return not self.__pc_do_connect(connection, self.target_ip, ERROR_PORT)

    def connect_c_08(self, connection):
        return False

    def __target_socket_close(self, connection):
        connection_handler = connection["Connection handler"]
        if connection["Target socket id"] is not None:
            connection_handler.add_checkers("CLOSE:%s" % connection["Target socket id"])
            self.tc_action.send_ssc_command("soc -T -s %s" % connection["Target socket id"])
            serial_result, tcp_result = connection_handler.get_checker_results()
            connection["Target socket id"] = None
        else:
            serial_result = None
        return True if serial_result is not None else False

    @staticmethod
    def __pc_socket_close(connection):
        connection_handler = connection["Connection handler"]
        if connection["PC socket"] is not None:
            connection_handler.add_checkers("CLOSED:%s" % connection["Target socket id"])
            connection["PC socket"].close()
            serial_result, tcp_result = connection_handler.get_checker_results()
            connection["PC socket"] = None
        else:
            serial_result = None
        return True if serial_result is not None else False

    def close_d_01(self, connection):
        connection["PC socket"] = None
        return self.__target_socket_close(connection)

    def close_d_02(self, connection):
        pass

    def close_d_03(self, connection):
        with self.sync_lock:
            self.pc_close_wait_socket_list.append(connection["PC socket"])
        return self.__target_socket_close(connection)
        pass

    def close_d_04(self, connection):
        pass

    def close_d_05(self, connection):
        return self.__pc_socket_close(connection)

    def close_d_06(self, connection):
        # target send data to PC, PC don't recv and close socket
        connection_handler = connection["Connection handler"]
        connection_handler.add_checkers("SEND:%s,OK" % connection["Target socket id"])
        self.tc_action.send_ssc_command("soc -S -s %s -l 100" % connection["Target socket id"])
        serial_result, tcp_result = connection_handler.get_checker_results()
        if serial_result is None:
            return False
        return self.__pc_socket_close(connection)

    def close_d_07(self, connection):
        # PC shutdown WR
        result = False
        try:
            connection["PC socket"].shutdown(socket.SHUT_WR)
            result = True
        except StandardError:
            pass
        return result

    def close_d_08(self, connection):
        pass

    def close_connection(self, connection):
        self.__target_socket_close(connection)
        pass

    TCP_ACTION_DICT = {"C_01": connect_c_01,
                       "C_02": connect_c_02,
                       "C_03": connect_c_03,
                       "C_04": connect_c_04,
                       "C_05": connect_c_05,
                       "C_06": connect_c_06,
                       "C_07": connect_c_07,
                       "C_08": connect_c_08,
                       "D_01": close_d_01,
                       "D_02": close_d_02,
                       "D_03": close_d_03,
                       "D_04": close_d_04,
                       "D_05": close_d_05,
                       "D_06": close_d_06,
                       "D_07": close_d_07,
                       "D_08": close_d_08,
                       }

    def get_method_destination_state(self, method):
        return self.METHOD_RESULT[method]

    def execute_tcp_method(self, method, connection):
        if method in self.METHOD_RESULT:
            result = self.TCP_ACTION_DICT[method](self, connection)
            if result is True:
                state = self.get_method_destination_state(method)
                connection["Target state"] = state[0]
                connection["PC state"] = state[1]
            else:
                NativeLog.add_prompt_trace("[TCPConnection] tcp method %s fail, connection is %s"
                                           % (method, connection))
                NativeLog.add_trace_info("[TCPConnection][data cache][check item %s] %s"
                                         % (connection["Connection handler"].serial_check_item_id,
                                            connection["Connection handler"].data_cache))
        else:
            raise StandardError("Not TCP connection method")
        return result

    def is_established_state(self, connection):
        return True if connection["Target state"] in self.SOC_CREATED_STATE else False

    def is_closed_state(self, connection):
        return True if connection["Target state"] in self.SOC_CLOSED_STATE else False


def main():
    pass

if __name__ == '__main__':
    main()
