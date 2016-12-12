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


LOG_FOLDER = os.path.join("Performance", "Throughput")


AP_PROP_KEY = ("ssid", "password", "apc")


class SendThread(threading.Thread):
    def __init__(self, sock, send_len, target_addr):
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.sock = sock
        self.send_len = send_len
        self.target_addr = target_addr
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
                data, addr = self.sock.recvfrom(65535)
            except StandardError:
                break
            if self.calc_event.isSet() is True:
                self.bytes_recv += len(data)

    def get_bytes_recv(self):
        return self.bytes_recv
    pass


class TestCase(TCActionBase.CommonTCActionBase):

    def __init__(self, test_case, test_env, timeout=30, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, test_case, test_env, timeout=timeout, log_path=log_path)
        self.performance_folder_path = log_path
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
            ap_list = self.get_parameter("shield_box_ap_list")
            pc_ip = self.get_parameter("pc_ip")
            send_len = self.send_len
            att_test_list = self.att_test_list
            tx_enable = self.tx_enable
            rx_enable = self.rx_enable
            measure_period = self.measure_period
            # configurable params
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for TCPThroughput script, error is %s" % e)
            raise StandardError("Error configuration")

        udp_port = random.randint(40000, 50000)

        # init throughput result data
        test_item = ""
        if tx_enable is True:
            test_item += "Tx"
        if rx_enable is True:
            test_item += "Rx"
        if test_item == "":
            raise StandardError("no throughput test item")

        folder_path = os.path.join(self.performance_folder_path, LOG_FOLDER)
        file_name = os.path.join(folder_path,
                                 "UDPThroughput_%s_%s" % (test_item, time.strftime("%d%H%M%S", time.localtime())))

        result = ThroughputResult.ThroughputResult(file_name)

        # restart before executing throughput
        checker_stings = ["R SSC1 C !!!ready!!!"]
        test_action_string = ["SSC SSC1 reboot"]
        fail_string = "Fail, Fail to reboot"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            self.result_cntx.set_result("Fail")
            return

        # disable recv print during throughput test
        checker_stings = ["R SSC1 C +RECVPRINT"]
        test_action_string = ["SSC SSC1 soc -R -o 0"]
        fail_string = "Fail, Fail to disable recv print"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            self.result_cntx.set_result("Fail")
            return

        ret = True
        for ap_prop in ap_list:
            ssid = ap_prop[0]
            password = ap_prop[1]
            apc = ap_prop[2]
            if ap_prop[1] == "":
                # set a default string for open ap
                password = "1"

            # switch off all outlet, switch on AP outlet
            outlet_config_dict = dict.fromkeys(range(1, 9), "OFF")
            outlet_config_dict[apc] = "ON"
            apc_cmd = "APC <APC1>"
            for outlet in outlet_config_dict:
                apc_cmd += " %s %s" % (outlet_config_dict[outlet], outlet)
            checker_stings = ["P PC_COM L OK"]
            fail_string = "Fail, Fail to switch apc"
            if self.load_and_exe_one_step(checker_stings, [apc_cmd], fail_string) is False:
                ret = False
                break

            # wait AP ready
            time.sleep(20)

            # create server
            udp_sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_DGRAM)
            udp_sock.bind((pc_ip, udp_port))
            udp_sock.settimeout(1)

            if tx_enable is True:
                result.add_test_item(ssid + "_tx")
            if rx_enable is True:
                result.add_test_item(ssid + "_rx")

            # create RSSI Calibrator
            calibrator = RSSICalibrator.Calibrator()

            for att_value in att_test_list:
                # step 0 set att value
                checker_stings = ["R PC_COM L OK"]
                test_action_string = ["ATT <att_port> %s" % att_value]
                fail_string = "Fail, Fail to set att value"
                if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                    ret = False
                    break
                    # continue
                # step 1 get AP RSSI
                checker_stings = ["R SSC1 A <rssi>:\+SCAN:%s,[:\d\w]+,\d+,\d+,([-\d]+)\r" % ssid]
                test_action_string = ["SSC SSC1 sta -S -s %s" % ssid]
                fail_string = "Fail, Fail to scan"
                rssi = scan_count = 0
                for i in range(3):
                    if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                        continue
                    rssi += int(self.test_env.get_variable_by_name("rssi")[1])
                    scan_count += 1

                rssi = calibrator.calibrate_rssi(float(rssi)/scan_count if scan_count > 0 else 0, att_value)
                # step 2 connect to AP
                checker_stings = ["R SSC1 C +JAP:CONNECTED"]
                test_action_string = ["SSC SSC1 sta -C -s %s -p %s" % (ssid, password)]
                fail_string = "Fail, Fail to JAP"
                if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string,
                                              check_freq=1, check_time=30) is False:
                    if rssi < -89:
                        continue
                    else:
                        ret = False
                        break
                    # continue
                checker_stings = ["R SSC1 A <target_ip>:STAIP:(\d+\.\d+\.\d+\.\d+)"]
                test_action_string = ["SSC SSC1 ip -Q"]
                fail_string = "Fail, Fail to get ip"
                if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string,
                                              check_freq=1, check_time=30) is False:
                    if rssi < -89:
                        continue
                    else:
                        ret = False
                        break
                    # continue
                target_ip = self.get_parameter("target_ip")

                # step 3 close all connections
                checker_stings = ["R SSC1 C +CLOSEALL"]
                test_action_string = ["SSC SSC1 soc -T"]
                fail_string = "Fail, Fail to close socket"
                if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                    ret = False
                    break
                    # continue

                # step 4 create UDP socket
                checker_stings = ["R SSC1 A <client_sock>:\+BIND:(\d+),OK"]
                test_action_string = ["SSC SSC1 soc -B -t UDP -i %s -p %s" % (target_ip, udp_port)]
                fail_string = "Fail, Fail to bind"
                if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
                    ret = False
                    break
                    # continue

                # step 5 do throughput test
                send_thread = SendThread(udp_sock if rx_enable is True else None,
                                         send_len, (target_ip, udp_port))
                send_thread.start()

                recv_thread = RecvThread(udp_sock if tx_enable is True else None)
                recv_thread.start()

                if tx_enable is True:
                    # do send from target
                    test_action_string = ["SSC SSC1 soc -S -s <client_sock> -l %s -n 10000000 -i %s -p %s"
                                          % (send_len, pc_ip, udp_port)]
                    fail_string = "Fail, Fail to send"
                    if self.load_and_exe_one_step([], test_action_string, fail_string) is False:
                        pass

                # start throughput calculate
                recv_thread.start_calc()

                # sleep for measure period
                time.sleep(measure_period)

                # stop throughput calculate
                recv_thread.stop_calc()
                send_thread.exit()

                send_thread.join()
                recv_thread.join()

                # output throughput result
                # in Mbps
                if rx_enable is True:
                    # get received data len from PC
                    self.load_and_exe_one_step(["R SSC1 A <recv_len>:RECVLEN:(\d+)"],
                                               ["SSC SSC1 soc -Q -s <client_sock> -o 1"],
                                               "Fail, Fail to get recv data len")
                    try:
                        rx_data_len = int(self.get_parameter("recv_len"))
                    except StandardError:
                        rx_data_len = 0

                    result.log_throughput(ssid + "_rx", rssi, att_value,
                                          float(rx_data_len * 8) / (measure_period * 1000000))

                if recv_thread.get_bytes_recv() > 0:
                    result.log_throughput(ssid + "_tx", rssi, att_value,
                                          float(recv_thread.get_bytes_recv() * 8) / (measure_period * 1000000))

                result.output_to_file()
                pass

            udp_sock.close()
            if not ret:
                NativeLog.add_trace_critical("Test SUC for %s" % ssid)
            elif ret:
                NativeLog.add_trace_critical("Test FAIL for %s!!!" % ssid)

        if ret:
            self.result_cntx.set_result("Succeed")
        else:
            self.result_cntx.set_result("Fail")

        # finally, execute done

    def result_check(self, port_name, data):
        TCActionBase.CommonTCActionBase.result_check(self, port_name, data)
        self.result_cntx.append_data(port_name, data)


def main():
    pass

if __name__ == '__main__':
    main()
