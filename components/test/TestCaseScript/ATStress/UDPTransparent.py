from TCAction import TCActionBase
from TCAction import CmdHandler
from NativeLog import NativeLog
import time
import random
import string
import os


class TransparentResultCheckCntx(TCActionBase.ResultCheckContext):

    def __init__(self, test_action, test_env, name):
        TCActionBase.ResultCheckContext.__init__(self, test_action, test_env, name)
        self.result_array = []
        self.at_data_recv_total = 0
        self.pc_data_recv_total = 0
        pass

    def run(self):
        validation_required = self.test_action.data_validation
        temp_data_at2wifi = ""
        temp_data_wifi2at = ""
        path = os.path.split(self.test_action.log_file_name)
        file_name_at2wifi = os.path.join(path[0], "%s_at2wifi_pc.bin" % self.test_action.timestamp)
        file_name_wifi2at = os.path.join(path[0], "%s_wifi2at_at.bin" % self.test_action.timestamp)

        while True:
            exit_flag = self.wait_exit_event(2)
            # force exit
            if exit_flag is True:
                break
            rx_len = 0
            tx_len = 0
            try:
                self.lock_data()
                rx_port = filter(lambda x: x[0] == "AT1", self.data_cache)
                tx_port = filter(lambda x: x[0] == "SOC1", self.data_cache)
                self.data_cache = []
            finally:
                self.unlock_data()

            if len(rx_port) == 1:
                rx_len = len(rx_port[0][1])
                self.at_data_recv_total += rx_len
                if validation_required is True:
                    temp_data_wifi2at += rx_port[0][1]
            if len(tx_port) == 1:
                tx_len = len(tx_port[0][1])
                self.pc_data_recv_total += tx_len
                if validation_required is True:
                    temp_data_at2wifi += tx_port[0][1]

            self.result_array.append(["TX  %8d  %s" %
                                      (tx_len/2, time.strftime("%m-%d %H:%M:%S", time.localtime()))])
            self.result_array.append(["RX  %8d  %s" %
                                      (rx_len/2, time.strftime("%m-%d %H:%M:%S", time.localtime()))])

        if validation_required is True:
            with open(file_name_at2wifi, "ab+") as f:
                    f.write(temp_data_at2wifi)
            with open(file_name_wifi2at, "ab+") as f:
                    f.write(temp_data_wifi2at)

    def get_test_results(self):
        return self.result_array, self.at_data_recv_total, self.pc_data_recv_total


class UDPTransparent(TCActionBase.CommonTCActionBase):

    def __init__(self, name, test_env, cmd_set, timeout=30, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                 timeout=timeout, log_path=log_path)
        # load param from excel
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        self.result_cntx = TCActionBase.ResultCheckContext(self, test_env, self.tc_name)
        self.timestamp = time.strftime("%d%H%M%S", time.localtime())

        pass

    def cleanup(self):
        # close current result check context
        self.result_cntx.stop_thread()
        self.result_cntx.join()
        # turn on logging
        self.test_env.uart_ports["AT1"].set_uart_logging_flag(True)
        # restore to read line mode
        self.test_env.uart_ports["AT1"].set_performance_flag(flag=False)

        # make sure enter condition that can respond to AT command
        self.result_cntx = TCActionBase.ResultCheckContext(self, self.test_env, self.tc_name)
        TCActionBase.TCActionBase.execute(self)
        self.result_cntx.start()

        checker_stings = ["R AT1 R *"]
        test_action_string = ["ATSO AT1 +++", "DELAY 0.1", "ATS AT1 AT"]
        fail_string = "Fail, Fail to reconfig UART"

        result = False

        while result is False:
            result = self.load_and_exe_one_step(checker_stings, test_action_string, fail_string)

        # reset baudrate

        checker_stings = ["R AT1 L OK"]
        test_action_string = ["ATS AT1 AT+UART_CUR=%d,8,1,0,3" % 115200]
        fail_string = "Fail, Fail to reconfig UART"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        test_action = CmdHandler.parse_action("UART AT1 %d" % 115200, self.test_env)
        CmdHandler.do_actions(test_action, self.test_env)
        TCActionBase.CommonTCActionBase.cleanup(self)

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        self.result_cntx.start()

        try:
            # configurable params
            # at send data len
            at_send_data_len = self.at_send_data_len
            # pc send data len
            pc_send_data_len = self.pc_send_data_len
            # sleep time between each send, test count
            test_dispatch = self.test_dispatch
            # enable target TCP TX
            tx_enable = self.tx_enable
            # enable target TCP RX
            rx_enable = self.rx_enable
            # if need to record tx/rx data to file
            data_validation = self.data_validation
            # UART baudrate
            baudrate = self.baudrate
            # HW flow control
            rtscts = self.rtscts
            # configurable params
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for TCPTransparent script, error is %s" % e)
            raise StandardError("Error configuration")

        # step0 reconfig baudrate
        if baudrate != 0:
            checker_stings = ["R AT1 L OK"]
            test_action_string = ["ATS AT1 AT+UART_CUR=%d,8,1,0,%d" % (baudrate, rtscts)]
            fail_string = "Fail, Fail to reconfig UART"
            if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                return
            test_action = CmdHandler.parse_action("UART AT1 %d %d" % (baudrate, rtscts), self.test_env)
            CmdHandler.do_actions(test_action, self.test_env)

        # step1 create PC server
        checker_stings = ["R SOC_COM L OK"]
        test_action_string = ["SOC SOC1 BIND <test_udp_port1> <pc_ip>"]
        fail_string = "Fail, Fail on create PC server"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step2 target connect, switch to transparent
        checker_stings = ["R AT1 NC CLOSE L OK"]
        test_action_strings = ["ATC AT1 CIPSTART \"UDP\" <pc_ip> <test_udp_port1> <test_udp_port1> 0"]
        fail_string = "Fail, Fail on connect to PC server"
        if self.load_and_exe_one_step(checker_stings, test_action_strings, fail_string) is False:
            return

        checker_stings = ["R AT1 L OK"]
        test_action_strings = ["ATS AT1 AT+CIPMODE=1"]
        if self.load_and_exe_one_step(checker_stings, test_action_strings, fail_string) is False:
            return

        checker_stings = ["R AT1 C >"]
        test_action_strings = ["ATS AT1 AT+CIPSEND"]
        if self.load_and_exe_one_step(checker_stings, test_action_strings, fail_string) is False:
            return

        # step 3
        # turn off AT UART logging
        self.test_env.uart_ports["AT1"].set_uart_logging_flag(False)
        # restore to read data asap
        self.test_env.uart_ports["AT1"].set_performance_flag(flag=True)

        # switch to new result check context
        self.result_cntx.stop_thread()
        self.result_cntx.join()
        self.result_cntx = TransparentResultCheckCntx(self, self.test_env, self.tc_name)
        self.result_cntx.start()

        at_data_sent_total = 0
        pc_data_sent_total = 0
        at2wifi_data = ""
        wifi2at_data = ""

        for j in range(0, len(at_send_data_len) * len(pc_send_data_len)):
            at_data_len = at_send_data_len[j / len(pc_send_data_len)]
            pc_data_len = pc_send_data_len[j % len(pc_send_data_len)]
            # data = "".join(["A"] * at_data_len)
            chars = string.ascii_lowercase
            data_list = ["".join([random.choice(chars) for m in range(at_data_len-16)])]
            chars = string.ascii_uppercase
            data_list.append("".join([random.choice(chars) for m in range(at_data_len-16)]))
            chars = string.digits
            data_list.append("".join([random.choice(chars) for m in range(at_data_len-16)]))

            for i in range(0, len(test_dispatch)):
                for k in range(0, test_dispatch[i][1]):
                    data_str = "%.2d%.2d%.10d%s\r\n" % (j, i, k, data_list[k % 3])
                    # time1 = time.time()
                    if tx_enable is True:
                        test_action = CmdHandler.parse_action("ATSO AT1 %s" % data_str, self.test_env)
                        CmdHandler.do_actions(test_action, self.test_env)
                        at_data_sent_total += at_data_len
                        if data_validation is True:
                            at2wifi_data += data_str
                    # time2 = time.time()
                    if rx_enable is True:
                        test_action = CmdHandler.parse_action("SOC SOC1 SENDTO %d %s %s %s"
                                                              % (pc_data_len, "<test_udp_port1>",
                                                                 "<target_ip>", data_str), self.test_env)
                        CmdHandler.do_actions(test_action, self.test_env)
                        pc_data_sent_total += pc_data_len
                        if data_validation is True:
                            wifi2at_data += data_str
                    # time3 = time.time()
                    # if time3-time2 > 0.1:
                    #     pass
                    if test_dispatch[i][0] != 0:
                        time.sleep(test_dispatch[i][0])
        time.sleep(3)  # wait 3 seconds

        # write send data to file for data validation
        if data_validation is True:
            path = os.path.split(self.log_file_name)
            with open(os.path.join(path[0], "%s_at2wifi_at.bin" % self.timestamp), "ab+") as f:
                f.write(at2wifi_data)
            with open(os.path.join(path[0], "%s_wifi2at_pc.bin" % self.timestamp), "ab+") as f:
                f.write(wifi2at_data)
        throughput_results, at_data_recv_total, pc_data_recv_total = self.result_cntx.get_test_results()
        result_str = "AT sent %15d\r\n" % at_data_sent_total
        result_str += "PC recv %15d\r\n" % pc_data_recv_total
        result_str += "PC sent %15d\r\n" % pc_data_sent_total
        result_str += "AT recv %15d\r\n" % at_data_recv_total
        for _result in throughput_results:
            result_str += "%s\r\n" % _result
        with open(self.log_file_name, "ab+") as f:
            f.write(result_str)

        # finally, execute done
        self.result_cntx.set_result("Succeed")

    def result_check(self, port_name, data):
        self.result_cntx.append_data(port_name, data)


def main():
    pass

if __name__ == '__main__':
    main()

