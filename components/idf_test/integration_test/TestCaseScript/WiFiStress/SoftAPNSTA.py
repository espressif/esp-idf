import random
import time
import string
import threading

from TCAction import TCActionBase
from NativeLog import NativeLog
from TCAction import PerformanceTCBase
from Utility import Encoding


class STAJAPThread(threading.Thread):
    def __init__(self, test_action, port_name, ssid, password, delay1, delay2, change_mac):
        threading.Thread.__init__(self)
        self.setDaemon(True)
        self.test_action = test_action
        self.port_name = port_name
        self.ssid = ssid
        self.password = password
        self.delay1 = delay1
        self.delay2 = delay2
        self.change_mac = change_mac
        self.exit_flag = threading.Event()
        pass

    def exit(self):
        self.exit_flag.set()
        pass

    def run(self):
        total_test_count = 0
        fail_count = 0
        while self.exit_flag.isSet() is False:
            # change mac
            if self.change_mac is True:
                mac = Encoding.generate_random_mac()
                self.test_action.serial_write_line(self.port_name, "mac -S -o 1 -m %s" % mac)
                self.test_action.check_response(self.port_name, "+MAC:STA,OK")

            time.sleep(1)

            # JAP
            total_test_count += 1
            # flush current port data
            self.test_action.flush_data(self.port_name)
            self.test_action.serial_write_line(self.port_name, "sta -C -s %s -p %s" % (self.ssid, self.password))
            if self.test_action.check_response(self.port_name, "+JAP:CONNECTED", 45) is False:
                fail_count += 1
                NativeLog.add_trace_critical("[%s] Failed to JAP, Failed/Total : %d/%d"
                                             % (self.port_name, fail_count, total_test_count))
                continue
            time.sleep(random.randint(self.delay1[0], self.delay1[1]))

            # QAP
            self.test_action.serial_write_line(self.port_name, "sta -D")
            if self.test_action.check_response(self.port_name, "+QAP:OK", 5) is False:
                NativeLog.add_trace_critical("[%s] Failed to QAP" % self.port_name)
            time.sleep(random.randint(self.delay2[0], self.delay2[1]))

        # make sure quit AP
        self.test_action.serial_write_line(self.port_name, "sta -D")
        pass
    pass


class TestCase(PerformanceTCBase.PerformanceTCBase):

    def __init__(self, test_case, test_env, timeout=30, log_path=TCActionBase.LOG_PATH):
        PerformanceTCBase.PerformanceTCBase.__init__(self, test_case, test_env, timeout=timeout, log_path=log_path)
        self.sta_num = 0
        self.max_sta = 4
        self.test_time = 60
        self.delay1 = [5, 30]
        self.delay2 = [5, 10]
        self.change_mac = True
        self.channel = 11
        # load param from excel
        cmd_set = test_case["cmd set"]
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy" and cmd_set[i][0] != "":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        pass

    def process(self):
        # configurable parameters
        try:
            sta_num = self.sta_num
            max_sta = self.max_sta
            test_time = self.test_time
            # delay between JAP succeed and QAP
            delay1 = self.delay1
            # delay between QAP and JAP
            delay2 = self.delay2
            # if change mac each time before JAP
            change_mac = self.change_mac
            # channel
            channel = self.channel
        except StandardError, e:
            raise StandardError("miss mandatory parameters")

        # step 0, set op mode and enable dhcp
        self.serial_write_line("SSC1", "op -S -o 2")
        if self.check_response("SSC1", "+MODE:OK", 2) is False:
            NativeLog.add_trace_critical("Failed to set ap mode")
            return
        self.serial_write_line("SSC1", "dhcp -E -o 2")
        if self.check_response("SSC1", "+DHCP:AP,OK", 2) is False:
            NativeLog.add_trace_critical("Failed to enable ap dhcp")
            return
        self.serial_write_line("SSC1", "dhcp -L -s 192.168.4.2 -e 192.168.4.100 -t 1")
        if self.check_response("SSC1", "+DHCP:LEASE,OK", 2) is False:
            NativeLog.add_trace_critical("Failed to enable ap dhcp")
            return
        self.serial_write_line("SSC1", "dhcp -S -o 2")
        if self.check_response("SSC1", "+DHCP:AP,OK", 2) is False:
            NativeLog.add_trace_critical("Failed to enable ap dhcp")
            return

        for i in range(sta_num):
            self.serial_write_line("SSC%d" % (i+2), "op -S -o 1")
            if self.check_response("SSC%d" % (i+2), "+MODE:OK", 2) is False:
                NativeLog.add_trace_critical("Failed to set sta mode")
                return
            self.serial_write_line("SSC%d" % (i+2), "dhcp -S -o 1")
            if self.check_response("SSC%d" % (i+2), "+DHCP:STA,OK", 2) is False:
                NativeLog.add_trace_critical("Failed to enable sta dhcp")

        # step 1, set ap config and load
        ap_ssid = "".join([random.choice(string.uppercase) for m in range(15)])
        ap_password = "".join([random.choice(string.lowercase) for m in range(15)])

        self.serial_write_line("SSC1", "ap -S -s %s -p %s -t 3 -m %s -n %s"
                               % (ap_ssid, ap_password, max_sta, channel))
        if self.check_response("SSC1", "+SAP:OK", 2) is False:
            NativeLog.add_trace_critical("Failed to set AP")
            return

        # step 2, start thread to let STA JAP
        sta_thread_list = []
        for i in range(sta_num):
            sta_thread_list.append(STAJAPThread(self, "SSC%d" % (i+2),
                                                ap_ssid, ap_password, delay1, delay2, change_mac))
        for sta_thread in sta_thread_list:
            sta_thread.start()

        # step 3, sleep for test time
        for i in range(test_time):
            self.flush_data("SSC1")
            time.sleep(60)

        # step 4, close all thread, will disconnect when exit thread
        for sta_thread in sta_thread_list:
            sta_thread.exit()
        for sta_thread in sta_thread_list:
            sta_thread.join()
        # wait and make sure disconnect done
        time.sleep(1)

        # step 5, join AP and check
        sta_num_temp = max_sta if sta_num > max_sta else sta_num

        for i in range(sta_num_temp):
            self.serial_write_line("SSC%d" % (i+2), "sta -C -s %s -p %s" % (ap_ssid, ap_password))
            if self.check_response("SSC%d" % (i+2), "+JAP:CONNECTED", 45) is False:
                self.set_result("Fail")
                break
            pass
        else:
            self.set_result("Succeed")


def main():
    pass

if __name__ == '__main__':
    main()

