import random
import re
import sys
import threading
import time

import TCPConnUtility
from NativeLog import NativeLog
from TCAction import TCActionBase

reload(sys)
sys.setdefaultencoding('iso-8859-1')  # # use encoding that with 1 Byte length and contain 256 chars


DEFAULT_MAX_CONN_ALLOWED = 5


# complicated design because I want to make this script applied for all TCP connect/close test scenarios
# basic flow: try to create max connections, send/recv data if possible, close all connections
# connect:
# 1.  find available (target_link_id, socket_id) list,
#     notice that target_link_id maybe not correct if PC is client
#     (during that time, some link may timeout and got disconnected from FIN_WAIT or other state)
# 2.  choose one method from method set, try to connect
# 3.  update state table: a)check result and destination state, b)find real target_link_id, c)update
# send/recv data:
# 1.  find channels that are possible to send data on all connections
# 2.  send data on possible channels
# disconnect:
# 1.  find available connections
# 2.  choose one method from disconnect set, try to disconnect
# 3.  update state table (phase 1)
# async process:
# listen on AT UART port, record all "x,CONNECT" and "x,CLOSE" command
# for "x,CONNECT", append them to self.target_link_id_list, used when need to find real target_link_id
# for "x,CLOSE", update state table (phase 2), if matching connection is pending on wait state,
#      close them and remove from state table
class TCPConnStressTC(TCActionBase.CommonTCActionBase):

    def __init__(self, name, test_env, cmd_set, timeout=30, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                 timeout=timeout, log_path=log_path)
        self.__at1_buff = ""
        self.max_conn_allowed = test_env.get_variable_by_name("max_conn")
        self.enable_log = True
        # load param from excel
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string

        # connection_state_dict: {target_link_id: [socket_id, target_state, socket_state, is_closed]}
        # is_closed: found "x,CLOSE" in AT UART port
        self.connection_state_dict = dict(zip(range(self.max_conn_allowed), [None] * self.max_conn_allowed))
        self.created_link_id_list = []

        self.__available_soc_id = range(2, 2+self.max_conn_allowed)
        self.__available_link_id = range(self.max_conn_allowed)

        self.result_cntx = TCActionBase.ResultCheckContext(self, test_env, self.tc_name)
        self.utility = TCPConnUtility.TCPConnUtility(self)
        self.state_lock = threading.Lock()
        self.link_id_lock = threading.Lock()
        self.available_id_lock = threading.Lock()
        pass

    def __add_log(self, log_str):
        if self.enable_log is True:
            NativeLog.add_trace_info(log_str)

    def __get_created_target_link_id(self):
        self.link_id_lock.acquire()
        try:
            link_id = self.created_link_id_list[-1]
            self.created_link_id_list = []
        finally:
            self.link_id_lock.release()
        return link_id
        pass

    def __set_created_target_link_id(self, link_id):
        self.link_id_lock.acquire()
        try:
            self.created_link_id_list.append(link_id)
        finally:
            self.link_id_lock.release()
        pass

    def __find_channel_list(self):
        channel_list = []  # # [(socket_id, able_to_send, link_id, able_to_send), ]
        self.state_lock.acquire()
        try:
            for link_id in self.connection_state_dict:
                state = self.connection_state_dict[link_id]
                if state is not None:
                    channel_list.append([state[0], self.utility.is_able_to_send_data(state[2]),
                                         link_id, self.utility.is_able_to_send_data(state[1])])
        finally:
            self.state_lock.release()
        return channel_list
        pass

    def __established_connection_list(self):
        conn_list = []  # # [(socket_id, link_id), ]
        self.state_lock.acquire()
        try:
            for link_id in self.connection_state_dict:
                state = self.connection_state_dict[link_id]
                if state is not None:
                    if self.utility.is_established_connection([state[1], state[2]]) is True:
                        conn_list.append([state[0], link_id])
        finally:
            self.state_lock.release()
        return conn_list
        pass

    # find free socket_id, target_link_id pair
    def __get_available_id_list(self):
        self.available_id_lock.acquire()
        try:
            id_list = zip(self.__available_soc_id, self.__available_link_id)
        finally:
            self.available_id_lock.release()
        return id_list
        pass

    def __update_available_id_list(self, soc_id, link_id, action="ADD"):
        self.available_id_lock.acquire()
        try:
            if action == "ADD":
                self.__available_link_id.append(link_id)
                self.__available_soc_id.append(soc_id)
                self.__add_log("[AVAILABLE ID]soc %d link %d is available" % (soc_id, link_id))
            elif action == "REMOVE":
                self.__available_link_id.remove(link_id)
                self.__available_soc_id.remove(soc_id)
                self.__add_log("[AVAILABLE ID]soc %d link %d is used" % (soc_id, link_id))
        finally:
            self.available_id_lock.release()

    def __update_connection_state_item(self, target_link_id, socket_id=None,
                                       target_state=None, socket_state=None, is_closed=None):
        self.state_lock.acquire()
        try:
            state = self.connection_state_dict[target_link_id]
            if state is None:
                state = [None] * 4
            if socket_id is not None:
                state[0] = socket_id
            if target_state is not None:
                state[1] = target_state
            if socket_state is not None:
                state[2] = socket_state
            if is_closed is not None:
                state[3] = is_closed

            # remove closed connections
            closed = self.utility.is_closed_state(state[1]) and (state[3] is True)
            if closed is True:
                self.__update_available_id_list(state[0], target_link_id)
                state = None
            # if new connection created
            if self.connection_state_dict[target_link_id] is None:
                created = self.utility.is_created_state(state[1])
                if created is True:
                    self.__update_available_id_list(state[0], target_link_id, "REMOVE")
                else:
                    # connection did not created, do not add them to connection state table
                    state = None

            # set new connection_state
            self.connection_state_dict[target_link_id] = state
            self.__add_log("[STATE] link id is %d, state is %s" % (target_link_id, state))
        except StandardError, e:
            pass
        finally:
            self.state_lock.release()
        pass

    # update state table: if result is false, return, if result is true:
    # for connect, find real link id, update table according to destination state
    # for disconnect, if target in SOC_CLOSE_STATE && catch "x,CLOSE" from AT, remove the item
    def update_connection_state_table(self, conn_id, destination_state=None):
        if isinstance(conn_id, list) is True or isinstance(conn_id, tuple) is True:
            socket_id = conn_id[0]
            try:
                target_link_id = self.__get_created_target_link_id()
            except IndexError:
                target_link_id = conn_id[1]
                self.__add_log("[STATE]fail to get link id, state is %s, %s"
                               % (destination_state[0], destination_state[1]))
            self.__update_connection_state_item(target_link_id, socket_id,
                                                destination_state[0], destination_state[1])
            pass
        else:  # # called when recv CLOSED
            target_link_id = conn_id
            self.__update_connection_state_item(target_link_id, is_closed=True)
            pass
        pass

    def process_at_data(self, data):
        pos1 = 0
        pos2 = 0
        connect = re.compile("\d,CONNECT\r\n")
        close = re.compile("\d,CLOSED\r\n")
        connect_match = connect.findall(data)
        close_match = close.findall(data)
        close = re.compile("\d,CONNECT FAIL\r\n")
        close_match += close.findall(data)
        if len(connect_match) != 0:
            pos1 = data.find(connect_match[-1]) + 9
            # append last connected link id
            self.__set_created_target_link_id(int(connect_match[-1][:1]))
            pass
        if len(close_match) != 0:
            pos2 = data.find(close_match[-1]) + 7
            # update for all closed links
            for close_str in close_match:
                self.update_connection_state_table(int(close_str[:1]))
            pass
        pos = pos1 if pos1 > pos2 else pos2

        return data[pos:]

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        self.result_cntx.start()

        # configurable params
        # mandatory params
        try:
            connect_method_set = self.connect_method_set
            test_count = self.test_count
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for TCPConnSingleMode script, error is %s" % e)
            raise StandardError("Error configuration")
        # optional params
        try:
            disconn_method_set = self.disconn_method_set
        except StandardError:
            disconn_method_set = ["D_05"]
            pass
        try:
            delay = self.delay
        except StandardError:
            delay = 0
            pass
        try:
            check_data_len = self.check_data_len
        except StandardError:
            check_data_len = [0, 0]
            pass
        if isinstance(check_data_len, list) is False:
            check_data_len = [check_data_len] * 2
        # configurable params

        # step1 use <test_tcp_port1> to create server on both PC and target side
        checker_stings = ["SOCP SOC_COM L OK", "ATP AT1 L OK"]
        test_action_string = ["SOC SOC1 LISTEN <test_tcp_port1>", "ATC AT1 CIPSERVER 1 <test_tcp_port1>"]
        fail_string = "Fail, Fail on create server"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        for tested_count in xrange(test_count):
            # step2 do connect
            available_id_list = self.__get_available_id_list()

            for conn_id in available_id_list:
                connect_method = random.choice(connect_method_set)
                # ret, destination_state = self.utility.execute_tcp_method(connect_method, conn_id)
                try:
                    self.__add_log("[ACTION]connect method is %s, connect id is %s"
                                   % (connect_method, conn_id))
                    ret, destination_state = self.utility.execute_tcp_method(connect_method, conn_id)
                except StandardError, e:
                    NativeLog.add_trace_critical("Error in connect, error is %s" % e)
                    raise StandardError("Exception happen when connect")
                if ret is False:
                    # connect fail, should terminate TC and mark as fail
                    return
                else:
                    # succeed, append to table
                    self.update_connection_state_table(conn_id, destination_state)
                if delay != 0:
                    time.sleep(delay)

            # step 3 send/recv test data
            # # [(socket_id, able_to_send, link_id, able_to_send)]
            self.__add_log("[ACTION]SEND/RECV data")
            channel_list = self.__find_channel_list()
            for channel in channel_list:
                _check_data_len = [0, 0]
                if channel[1] is True:
                    _check_data_len[0] = check_data_len[0]
                if channel[3] is True:
                    _check_data_len[1] = check_data_len[1]
                ret = self.utility.send_test_data(channel[0],
                                                  channel[2],
                                                  _check_data_len)
                if ret is False:
                    # send/recv fail, should terminate TC and mark as fail
                    return
                if delay != 0:
                    time.sleep(delay)

            # step 4 close all established connections
            # (socket_id, link_id)
            conn_list = self.__established_connection_list()
            for conn_id in conn_list:
                disconn_method = random.choice(disconn_method_set)
                try:
                    self.__add_log("[ACTION]disconnect method is %s, connect id is %s"
                                   % (disconn_method, conn_id))
                    ret, destination_state = self.utility.execute_tcp_method(disconn_method, conn_id)
                except StandardError, e:
                    NativeLog.add_trace_critical("Error in disconnect, error is %s" % e)
                    raise StandardError("Exception happen when disconnect")
                if ret is False:
                    # connect fail, should terminate TC and mark as fail
                    return
                else:
                    # succeed, append to table
                    self.update_connection_state_table(conn_id, destination_state)
                if delay != 0:
                    time.sleep(delay)

        # finally, execute done
        self.result_cntx.set_result("Succeed")

    def result_check(self, port_name, data):
        TCActionBase.CommonTCActionBase.result_check(self, port_name, data)
        self.result_cntx.append_data(port_name, data)
        # find "x,CONNECT" and "x,CLOSE"
        if port_name.find("AT") != -1:
            self.__at1_buff += data
            self.__at1_buff = self.process_at_data(self.__at1_buff)


def main():
    at1_buff = ""
    pos1 = 0
    pos2 = 0
    data = "dafgajglajdfg0,CLOSEjdalghalksdg1,CONNECT\r\n\r\n3,CONNECT4,CLOSEadfaasdf"
    at1_buff += data
    connect = re.compile("\d,CONNECT")
    close = re.compile("\d,CLOSE")
    connect_match = connect.findall(at1_buff)
    close_match = close.findall(at1_buff)
    if len(connect_match) != 0:
        pos1 = at1_buff.find(connect_match[-1]) + 9
        pass
    if len(close_match) != 0:
        pos2 = at1_buff.find(close_match[-1]) + 7
        pass
    pos = pos1 if pos1 > pos2 else pos2

    at1_buff = at1_buff[pos:]

    pass

if __name__ == '__main__':
    main()

