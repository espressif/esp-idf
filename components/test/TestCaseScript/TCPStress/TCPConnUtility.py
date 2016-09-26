from NativeLog import NativeLog

# make sure target do not listen on this port
ERROR_PORT = 23333


def unused_param(param):
    return param


class TCPUtilError(StandardError):
    pass


class TCPConnUtility(object):
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
    SOC_CREATED_STATE = ("SYNC_RCVD", "SYNC_SENT", "ESTABLISHED")
    SOC_SEND_DATA_STATE = ("ESTABLISHED", "CLOSE_WAIT")
    SOC_ESTABLISHED_STATE = ("ESTABLISHED", )

    def __init__(self, tc_action):
        self.tc_action = tc_action
        self.pc_server_port = "<test_tcp_port1>"
        self.target_server_port = "<test_tcp_port1>"
        self.pc_ip = "<pc_ip>"
        self.target_ip = "<target_ip>"
        pass

    def config_parameters(self, pc_server_port=None, target_server_port=None, pc_ip=None, target_ip=None):
        if pc_ip is not None:
            self.pc_ip = pc_ip
        if target_ip is not None:
            self.target_ip = target_ip
        if pc_server_port is not None:
            self.pc_server_port = pc_server_port
        if target_server_port is not None:
            self.target_server_port = target_server_port
        pass

    def __connect_c_01(self, conn_id):
        checker_stings = ["SOCR SOC1 C +ACCEPT", "ATR AT1 NC CLOSE L OK"]
        test_action_strings = ["ATC AT1 CIPSTART %d \"TCP\" %s %s" %
                               (conn_id[1], self.pc_ip, self.pc_server_port)]
        fail_string = "Fail, Target failed on connect to PC server"
        ret = self.tc_action.load_and_exe_one_step(checker_stings, test_action_strings, fail_string)
        if ret is False:
            return ret

        checker_stings = ["SOCR SOC_COM L OK"]
        test_action_strings = ["SOC SOC1 ACCEPT SOC%d" % conn_id[0]]
        return self.tc_action.load_and_exe_one_step(checker_stings, test_action_strings, fail_string)
        pass

    def __connect_c_02(self, conn_id):
        checker_stings = ["ATR AT1 C ERROR"]
        test_action_strings = ["ATC AT1 CIPSTART %d \"TCP\" %s %s" %
                               (conn_id[1], self.pc_ip, ERROR_PORT)]
        fail_string = "Fail, Target fail on connect to port not listened"
        return self.tc_action.load_and_exe_one_step(checker_stings, test_action_strings, fail_string)
        pass

    def __connect_c_03(self, conn_id):
        pass

    def __connect_c_04(self, conn_id):
        pass

    def __connect_c_05(self, conn_id):
        checker_stings = ["SOCP SOC_COM OK", "ATP AT1 C CONNECT"]
        test_action_strings = ["SOC SOC%d CONNECT %s %s" %
                               (conn_id[0], self.target_server_port, self.target_ip)]
        fail_string = "Fail, PC fail on connect to target server"
        return self.tc_action.load_and_exe_one_step(checker_stings, test_action_strings,
                                                    fail_string, check_time=200, check_freq=0.01)
        pass

    def __connect_c_06(self, conn_id):
        pass

    def __connect_c_07(self, conn_id):
        # no checker strings, only try to create
        # while connect is a blocking function, will return till target reply RST
        checker_stings = ["SOCR SOC_COM C CLOSE"]
        test_action_strings = ["SOC SOC%d CONNECT %s %s" %
                               (conn_id[0], ERROR_PORT, self.target_ip)]
        fail_string = "Fail, PC fail on connect to target server"
        return self.tc_action.load_and_exe_one_step(checker_stings, test_action_strings,
                                                    fail_string, check_time=2000, check_freq=0.01)
        pass

    def __connect_c_08(self, conn_id):
        pass

    def __close_d_01(self, conn_id):
        checker_stings = ["ATP AT1 C %d,CLOSED" % conn_id[1], "SOCP SOC_COM C CLOSE"]
        test_action_strings = ["SOC SOC%d SETOPT CLOSE_OPT IMM_SEND_FIN" % conn_id[0],
                               "ATS AT1 AT+CIPCLOSE=%d" % conn_id[1]]
        fail_string = "Fail, Fail to close socket using D_01"
        return self.tc_action.load_and_exe_one_step(checker_stings, test_action_strings,
                                                    fail_string, check_time=200, check_freq=0.01)
        pass

    def __close_d_02(self, conn_id):
        pass

    def __close_d_03(self, conn_id):
        checker_stings = []
        test_action_strings = ["SOC SOC%d SETOPT CLOSE_OPT WAIT_TO" % conn_id[0],
                               "ATS AT1 AT+CIPCLOSE=%d" % conn_id[1]]
        fail_string = "Fail, Fail to close socket using D_01"
        return self.tc_action.load_and_exe_one_step(checker_stings, test_action_strings,
                                                    fail_string, check_time=500, check_freq=0.01)
        pass

    def __close_d_04(self, conn_id):
        pass

    def __close_d_05(self, conn_id):
        checker_stings = ["ATP AT1 C %d,CLOSED" % conn_id[1]]
        test_action_strings = ["SOC SOC%d SETOPT CLOSE_OPT IMM_SEND_FIN" % conn_id[0],
                               "SOC SOC%d CLOSE" % conn_id[0]]
        fail_string = "Fail, Fail to close socket using D_05"
        return self.tc_action.load_and_exe_one_step(checker_stings, test_action_strings,
                                                    fail_string, check_time=200, check_freq=0.01)
        pass

    def __close_d_06(self, conn_id):
        # 1. set PC socket close option, stop calling recv; send in target
        checker_stings = ["ATP AT1 C >"]
        test_action_strings = ["SOC SOC%d STOPRECV" % conn_id[0],
                               "SOC SOC%d SETOPT CLOSE_OPT IMM_SEND_RST" % conn_id[0],
                               "ATS AT1 AT+CIPSEND=%d,5" % conn_id[1]]
        fail_string = "Fail, Fail to close socket using D_06"
        ret = self.tc_action.load_and_exe_one_step(checker_stings, test_action_strings,
                                                   fail_string, check_time=200, check_freq=0.01)
        if ret is False:
            return ret

        # 2. send 5 bytes to socket
        checker_stings = ["ATP AT1 C OK"]
        test_action_strings = ["ATSN AT1 5"]
        fail_string = "Fail, Fail to close socket using D_06"
        ret = self.tc_action.load_and_exe_one_step(checker_stings, test_action_strings,
                                                   fail_string, check_time=200, check_freq=0.01)
        if ret is False:
            return ret

        # 3. close socket
        checker_stings = ["ATP AT1 OR 2 C %d,CONNECT C %d,CLOSED" % (conn_id[1], conn_id[1])]
        test_action_strings = ["SOC SOC%d CLOSE" % conn_id[0]]
        fail_string = "Fail, Fail to close socket using D_06"
        return self.tc_action.load_and_exe_one_step(checker_stings, test_action_strings,
                                                    fail_string, check_time=200, check_freq=0.01)
        pass

    def __close_d_07(self, conn_id):
        pass

    def __close_d_08(self, conn_id):
        pass

    def send_test_data(self, socket_id, target_link_id, check_data_len):
        # check_data_len[0] for socket data len, check_data_len[1] for target link data len
        fail_string = "Fail, Fail on send and recv data"

        ret = True

        for i in range(1):
            if check_data_len[1] != 0:
                checker_stings = ["ATP AT1 C >"]
                test_action_strings = ["ATS AT1 AT+CIPSEND=%d,%d" % (target_link_id, check_data_len[1])]
                if self.tc_action.load_and_exe_one_step(checker_stings, test_action_strings,
                                                        fail_string, check_time=20) is False:
                    NativeLog.add_trace_critical("Fail on target send command for link %d" % target_link_id)
                    ret = False
                    break
                checker_stings = ["SOCP SOC%d RL %d" % (socket_id, check_data_len[1]), "ATP AT1 C OK"]
                test_action_strings = ["ATSN AT1 %d" % check_data_len[1]]
                if self.tc_action.load_and_exe_one_step(checker_stings, test_action_strings,
                                                        fail_string, check_time=20) is False:
                    NativeLog.add_trace_critical("Fail on target send for link %d, send or recv error" % target_link_id)
                    ret = False
                    break

            if check_data_len[0] != 0:
                checker_stings = ["ATP AT1 DL %d+%d" % (target_link_id, check_data_len[0])]
                test_action_strings = ["SOC SOC%d SEND %d" % (socket_id, check_data_len[0])]

                if self.tc_action.load_and_exe_one_step(checker_stings, test_action_strings,
                                                        fail_string, check_time=20) is False:
                    NativeLog.add_trace_critical("Fail to receive PC SOC%d sent data" % socket_id)
                    ret = False
                    break

        # return ret
        # for now do not validate data
        return True

    TCP_ACTION_DICT = {"C_01": __connect_c_01,
                       "C_02": __connect_c_02,
                       "C_03": __connect_c_03,
                       "C_04": __connect_c_04,
                       "C_05": __connect_c_05,
                       "C_06": __connect_c_06,
                       "C_07": __connect_c_07,
                       "C_08": __connect_c_08,
                       "D_01": __close_d_01,
                       "D_02": __close_d_02,
                       "D_03": __close_d_03,
                       "D_04": __close_d_04,
                       "D_05": __close_d_05,
                       "D_06": __close_d_06,
                       "D_07": __close_d_07,
                       "D_08": __close_d_08,
                       }

    def get_method_destination_state(self, method):
        return self.METHOD_RESULT[method]

    def execute_tcp_method(self, method, conn_id):
        if method in self.METHOD_RESULT:
            return self.TCP_ACTION_DICT[method](self, conn_id), self.get_method_destination_state(method)
        else:
            raise TCPUtilError("Not TCP connection method")
        pass

    def is_created_state(self, state):
        if state in self.SOC_CREATED_STATE:
            return True
        else:
            return False

    def is_closed_state(self, state):
        if state in self.SOC_CLOSED_STATE:
            return True
        else:
            return False

    def is_able_to_send_data(self, state):
        if state in self.SOC_SEND_DATA_STATE:
            return True
        else:
            return False

    def is_established_connection(self, state):
        if state[0] in self.SOC_ESTABLISHED_STATE and state[1] in self.SOC_ESTABLISHED_STATE:
            return True
        else:
            return False


def main():
    pass

if __name__ == '__main__':
    main()
