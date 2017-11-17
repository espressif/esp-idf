import os
import time

from TCAction import TCActionBase
from TCAction import PerformanceTCBase
from NativeLog import NativeLog


SOFTAP_SSID = "SoftAPSSID4NVSCompatibleTest"
SOFTAP_PASSWORD = "SoftAPPassword4NVSCompatibleTest"

NVS_FILE_NAME = "nvs_wifi.bin"


class TestCase(PerformanceTCBase.PerformanceTCBase):

    def __init__(self, test_case, test_env, timeout=30, log_path=TCActionBase.LOG_PATH):
        PerformanceTCBase.PerformanceTCBase.__init__(self, test_case, test_env, timeout=timeout, log_path=log_path)
        self.nvs_path = ""
        # load param from excel
        cmd_set = test_case["cmd set"]
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string

    def get_nvs_bins(self):
        if os.path.exists(self.nvs_path) is False:
            NativeLog.add_trace_critical("NVS path is not correct")
            files = []
        else:
            files = [os.path.join(self.nvs_path, x) for x in os.listdir(self.nvs_path)]
        return filter(lambda f: NVS_FILE_NAME in f, files)

    def check_nvs(self, nvs_bin):
        branch_name = os.path.basename(nvs_bin)
        branch_name.replace("___", "/")
        result = True

        ssc1_port = self.test_env.get_port_by_name("SSC1")
        ap_ssid = self.get_parameter("ap_ssid")

        self.flush_data("SSC1")

        # first download
        ssc1_port.flash_nvs(nvs_bin)
        self.check_response("SSC1", "ready!!!")
        # set to sta mode and join ap
        if self.check_response("SSC1", "+JAP:CONNECTED,%s" % ap_ssid, timeout=15) is False:
            NativeLog.add_trace_critical("Failed to join AP on: " + branch_name)
            result = False

        self.serial_write_line("SSC1", "op -Q")
        if self.check_response("SSC1", "+CURMODE:3") is False:
            NativeLog.add_trace_critical("Failed on verifying WIFI mode on: " + branch_name)
            result = False

        self.serial_write_line("SSC1", "ap -Q")
        if self.check_response("SSC1", "+APCONFIG:%s,%s" % (SOFTAP_SSID, SOFTAP_PASSWORD)) is False:
            NativeLog.add_trace_critical("Failed on verifying SoftAP config on: " + branch_name)
            result = False
        return result

    def dump_nvs(self):
        ssc1_port = self.test_env.get_port_by_name("SSC1")
        ap_ssid = self.get_parameter("ap_ssid")
        ap_password = self.get_parameter("ap_password")

        # first erase NVS
        ssc1_port.flash_nvs(None)

        self.check_response("SSC1", "ready!!!")

        self.serial_write_line("SSC1", "op -S -o 3")
        self.check_response("SSC1", "+MODE:OK")
        self.serial_write_line("SSC1", "sta -C -s %s -p %s" % (ap_ssid, ap_password))
        self.check_response("SSC1", "+JAP:CONNECTED,%s" % ap_ssid, timeout=20)
        self.serial_write_line("SSC1", "ap -S -s %s -p %s -t 3" % (SOFTAP_SSID, SOFTAP_PASSWORD))
        self.check_response("SSC1", "+SAP:OK")
        time.sleep(1)

        idf_path = os.getenv("IDF_PATH")

        ssc1_port.dump_nvs(os.path.join(idf_path, NVS_FILE_NAME))

    def cleanup(self):
        # make sure dump nvs will be executed
        self.dump_nvs()

    def process(self):
        result = True
        nvs_bins = self.get_nvs_bins()

        for nvs_bin in nvs_bins:
            result = result and self.check_nvs(nvs_bin)

        if result is True:
            self.set_result("Succeed")
