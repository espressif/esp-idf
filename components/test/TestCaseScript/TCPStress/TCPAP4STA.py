from TCAction import TCActionBase
from NativeLog import NativeLog
import copy
import time
import random
import string


class TCPAP4STAResultCheckCntx(TCActionBase.ResultCheckContext):

    def __init__(self, test_action, test_env, name):
        TCActionBase.ResultCheckContext.__init__(self, test_action, test_env, name)
        self.failed_port = []
        pass

    def run(self):

        while True:
            exit_flag = self.wait_exit_event(1)
            # force exit
            if exit_flag is True:
                break
            try:
                self.lock_data()
                temp_cache = copy.deepcopy(self.data_cache)
                self.data_cache = []
            finally:
                self.unlock_data()

            for _cache in temp_cache:
                _data = _cache[1]
                if _data.find("user_test_tcpclient_recon_cb") != -1 or _data.find("discon") != -1 \
                        or _data.find("No heap available") != -1:
                    self.failed_port.append(_cache[0])
                    NativeLog.add_trace_critical("TCPAP4STA failed, failed on %s" % _cache[0])
                pass
            if len(self.failed_port) != 0:
                # disconnect happen
                break

    def get_test_results(self):
        return self.failed_port


class TCPAP4STA(TCActionBase.CommonTCActionBase):

    def __init__(self, name, test_env, cmd_set, timeout=30, log_path=TCActionBase.LOG_PATH):
        TCActionBase.CommonTCActionBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                 timeout=timeout, log_path=log_path)
        # load param from excel
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
            send_len = self.send_len
            # test count
            test_count = self.test_count
            # server port
            server_port = self.server_port
            # ap ip
            ap_ip = self.ap_ip
            # server echo
            server_echo = self.server_echo
            # station number
            sta_number = self.sta_number
            # pass standard
            pass_standard = self.pass_standard
            # send delay
            send_delay = self.send_delay
            # configurable params
        except StandardError, e:
            NativeLog.add_trace_critical("Error configuration for TCPTransparent script, error is %s" % e)
            raise StandardError("Error configuration")

        # step0 reboot
        checker_stings = []
        test_action_string = []

        for i in range(sta_number+1):
            checker_stings.append("P SSC%d C !!!ready!!!" % (i+1))
            test_action_string.append("SSCC SSC%d reboot" % (i+1))

        fail_string = "Fail, Fail to reboot"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step1 set ap on SSC1, create server
        ssid = "".join([random.choice(string.lowercase) for m in range(10)])
        password = "".join([random.choice(string.lowercase) for m in range(10)])
        checker_stings = ["R SSC1 C dhcp%20server%20start"]
        test_action_string = ["SSCC SSC1 ap -S -s %s -p %s -n 10 -t 0 -m 8" % (ssid, password)]
        fail_string = "Fail, Fail set ap"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        checker_stings = ["R SSC1 C server%20starts%20at%20port"]
        if server_echo is True:
            test_action_string = ["SSCC SSC1 tcp -S -p %s -b 1" % server_port]
        else:
            test_action_string = ["SSCC SSC1 tcp -S -p %s" % server_port]
        fail_string = "Fail, Fail create server"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # step 2, 4 SSC target(SSC2 - SSC5) join SSC1 soft AP
        checker_stings = []
        test_action_string = []

        for i in range(sta_number):
            checker_stings.append("P SSC%d C ip C mask C gw C get%%20ip%%20of" % (i+2))
            test_action_string.append("SSCC SSC%d ap -C -s %s -p %s" % (i+2, ssid, password))

        fail_string = "Fail, Fail to connect to server"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        start_time = time.time()

        # step 3, create client on SSC2 - SSC5
        checker_stings = []
        test_action_string = []

        for i in range(sta_number):
            checker_stings.append("P SSC%d C tcp%%20client%%20connect%%20with%%20server" % (i+2))
            test_action_string.append("SSCC SSC%d tcp -W -i %s -p %s -c %s -n 1 -l %s -d %d"
                                      % ((i+2), ap_ip, server_port, test_count, send_len, send_delay))

        fail_string = "Fail, Fail to connect to server"
        if self.load_and_exe_one_step(checker_stings, test_action_string, fail_string) is False:
            return

        # switch to new result context
        self.result_cntx.stop_thread()
        self.result_cntx.join()
        self.result_cntx = TCPAP4STAResultCheckCntx(self, self.test_env, self.tc_name)
        self.result_cntx.start()

        self.result_cntx.join()

        failed_port = self.result_cntx.get_test_results()
        if (time.time() - start_time) > pass_standard:
            self.result_cntx.set_result("Succeed")
        else:
            self.result_cntx.set_result("Failed")
            NativeLog.add_trace_critical("Failed port: %s" % failed_port)

        # finally, execute done

    def result_check(self, port_name, data):
        TCActionBase.CommonTCActionBase.result_check(self, port_name, data)
        self.result_cntx.append_data(port_name, data)


def main():
    pass

if __name__ == '__main__':
    main()

