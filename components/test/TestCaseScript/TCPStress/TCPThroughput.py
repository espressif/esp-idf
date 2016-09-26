import os
import time
import random
import threading
import socket

from TCAction import TCActionBase
from NativeLog import NativeLog
from NativeLog import ThroughputResult
from Utility import RSSICalibrator
from Utility import MakeFolder


LOG_FOLDER = os.path.join("AT_LOG", "Performance", "Throughput")


AP_PROP_KEY = ("ssid", "password", "apc")


class SendThread(threading.Thread):
    def __init__(self, sock, send_len):
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.sock = sock
        self.send_len = send_len
        self.exit_event = threading.Event()
        self.calc_event = threading.Event()
        self.bytes_sent = 0
        pass

    def start_calc(self):
        self.calc_event.set()

    def stop_calc(self):
        self.calc_event.clear()
        self.exit_event.set()

    def run(self):
        data = "A" * self.send_len
        if self.sock is None:
            return
        while True:
            if self.exit_event.isSet() is True:
                break
            try:
                self.sock.send(data)
            except StandardError:
                break
            if self.calc_event.isSet() is True:
                self.bytes_sent += self.send_len

    def get_bytes_sent(self):
        return self.bytes_sent
    pass


class RecvThread(threading.Thread):
    def __init__(self, sock):
        threading.Thread.__init__(self)
        self.setDaemon(True)
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
        while True:
            if self.exit_event.isSet() is True:
                break
            try:
                data = self.sock.recv(8*1024)
            except StandardError:
                break
            if self.calc_event.isSet() is True:
                self.bytes_recv += len(data)

    def get_bytes_recv(self):
        return self.bytes_recv
    pass


class TCPThroughput(TCActionBase.CommonTCActionBase):

    def __init__(self, name, test_env, cmd_set, timeout=30, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                 timeout=timeout, log_path=log_path)
        self.att_test_list = range(60)
        # load param from excel
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        # read AP list
        self.ap_list = []
        for i in range(1, len(cmd_set)):
            for j in range(len(cmd_set[i][1])):
                if cmd_set[i][1][j] != "":
                    cmd_string = "self.ap_list.append(dict(zip(AP_PROP_KEY, " + cmd_set[i][1][j] + ")))"
                    exec cmd_string
        self.result_cntx = TCActionBase.ResultCheckContext(self, test_env, self.tc_name)
        pass

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        self.result_cntx.start()

        try:
            # configurable params
            ap_list = self.ap_list
            send_len = self.send_len
            att_test_list = self.att_test_list
            tx_enable = self.tx_enable
            rx_enable = self.rx_enable
            measure_period = self.measure_period
            # configurable params
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for TCPThroughput script, error is %s" % e)
            raise StandardError("Error configuration")

        # find local ip and generate local port
        local_ip_list = socket.gethostbyname_ex(socket.gethostname())[2]
        for local_ip in local_ip_list:
            if local_ip.find("192.168.1.") != -1:
                pc_ip = local_ip
                break
        else:
            raise StandardError("Can't find local IP.")

        tcp_port = random.randint(40000, 50000)

        # init throughput result data
        test_item = ""
        if tx_enable is True:
            test_item += "Tx"
        if rx_enable is True:
            test_item += "Rx"
        if test_item == "":
            raise StandardError("no throughput test item")

        folder_path = MakeFolder.make_folder(LOG_FOLDER)
        file_name = os.path.join(folder_path,
                                 "TCPThroughput_%s_%s" % (test_item, time.strftime("%d%H%M%S", time.localtime())))

        result = ThroughputResult.ThroughputResult(file_name, standard_required=True)

        # restart before executing throughput
        checker_stings = ["R SSC1 C !!!ready!!!"]
        test_action_string = ["SSC SSC1 reboot"]
        fail_string = "Fail, Fail to reboot"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # disable recv print during throughput test
        checker_stings = ["R SSC1 C +RECVPRINT"]
        test_action_string = ["SSC SSC1 soc -R -o 0"]
        fail_string = "Fail, Fail to disable recv print"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        for ap_prop in ap_list:
            if ap_prop["password"] == "":
                # set a default string for open ap
                ap_prop["password"] = "1"

            # switch off all outlet, switch on AP outlet
            outlet_config_dict = dict.fromkeys(range(1, 9), "OFF")
            outlet_config_dict[ap_prop["apc"]] = "ON"
            apc_cmd = "APC <APC1>"
            for outlet in outlet_config_dict:
                apc_cmd += " %s %s" % (outlet_config_dict[outlet], outlet)
            checker_stings = ["P PC_COM L OK"]
            fail_string = "Fail, Fail to switch apc"
            if self.load_and_exe_one_step(checker_stings, [apc_cmd], fail_string) is False:
                return

            # wait AP ready
            time.sleep(20)

            # create server
            server_sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
            server_sock.bind((pc_ip, tcp_port))
            server_sock.settimeout(5)
            server_sock.listen(5)

            if tx_enable is True:
                result.add_test_item(ap_prop["ssid"] + "_tx")
            if rx_enable is True:
                result.add_test_item(ap_prop["ssid"] + "_rx")

            # create RSSI Calibrator
            calibrator = RSSICalibrator.Calibrator()

            for att_value in att_test_list:
                # step 0 set att value
                checker_stings = ["R PC_COM L OK"]
                test_action_string = ["ATT <att_port> %s" % att_value]
                fail_string = "Fail, Fail to set att value"
                if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                    continue
                # step 1 connect to AP
                checker_stings = ["R SSC1 C +JAP:CONNECTED"]
                test_action_string = ["SSC SSC1 sta -C -s %s -p %s" % (ap_prop["ssid"], ap_prop["password"])]
                fail_string = "Fail, Fail to JAP"
                if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string,
                                              check_freq=1, check_time=30) is False:
                    continue
                # step 2 get AP RSSI
                checker_stings = ["R SSC1 A <rssi>:\+SCAN:%s,[:\d\w]+,\d+,\d+,([-\d]+)" % ap_prop["ssid"]]
                test_action_string = ["SSC SSC1 sta -S -s %s" % ap_prop["ssid"]]
                fail_string = "Fail, Fail to scan"
                rssi = scan_count = 0
                for i in range(3):
                    if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                        continue
                    rssi += int(self.test_env.get_variable_by_name("rssi")[1])
                    scan_count += 1

                rssi = calibrator.calibrate_rssi(float(rssi)/scan_count if scan_count > 0 else 0, att_value)

                # step 3 close all connections
                checker_stings = ["R SSC1 C +CLOSEALL"]
                test_action_string = ["SSC SSC1 soc -T"]
                fail_string = "Fail, Fail to create server"
                if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                    continue

                # step 4 create tcp connection

                checker_stings = ["R SSC1 A <client_sock>:\+BIND:(\d+),OK"]
                test_action_string = ["SSC SSC1 soc -B -t TCP"]
                fail_string = "Fail, Fail bind"
                if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                    continue

                checker_stings = ["P SSC1 RE \+CONNECT:\d+,OK"]
                test_action_string = ["SSC SSC1 soc -C -s <client_sock> -i %s -p %s" % (pc_ip, tcp_port)]
                fail_string = "Fail, Fail to connect"
                if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                    continue

                try:
                    sock, addr = server_sock.accept()
                except socket.error, e:
                    NativeLog.add_trace_critical("%s" % e)
                    continue
                sock.settimeout(measure_period)

                # step 5 do throughput test
                send_thread = SendThread(sock if rx_enable is True else None, send_len)
                send_thread.start()

                recv_thread = RecvThread(sock if tx_enable is True else None)
                recv_thread.start()

                if tx_enable is True:
                    # do send from target
                    test_action_string = ["SSC SSC1 soc -S -s <client_sock> -l %s -n 10000000" % send_len]
                    fail_string = "Fail, Fail to send"
                    if self.load_and_exe_one_step([], test_action_string, fail_string) is False:
                        pass

                # start throughput calculate
                send_thread.start_calc()
                recv_thread.start_calc()

                # sleep for measure period
                time.sleep(measure_period)

                # stop throughput calculate
                send_thread.stop_calc()
                recv_thread.stop_calc()

                send_thread.join()
                recv_thread.join()

                sock.close()

                # output throughput result
                # in Mbps
                if send_thread.get_bytes_sent() > 0:
                    result.log_throughput(ap_prop["ssid"] + "_rx", rssi, att_value,
                                          float(send_thread.get_bytes_sent() * 8) / (measure_period * 1000000))

                if recv_thread.get_bytes_recv() > 0:
                    result.log_throughput(ap_prop["ssid"] + "_tx", rssi, att_value,
                                          float(recv_thread.get_bytes_recv() * 8) / (measure_period * 1000000))

                result.output_to_file()
                pass

            server_sock.close()

        self.result_cntx.set_result("Succeed")

        # finally, execute done

    def result_check(self, port_name, data):
        TCActionBase.CommonTCActionBase.result_check(self, port_name, data)
        self.result_cntx.append_data(port_name, data)


def main():
    pass

if __name__ == '__main__':
    main()
