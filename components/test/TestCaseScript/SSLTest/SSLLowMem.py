import os
import random
import time
import re

from TCAction import TCActionBase
from TCAction import PerformanceTCBase
from NativeLog import NativeLog, HTMLGenerator
from Utility import MakeFolder

import ConfigUtility
import Capability
import SSLHandler

LOG_FOLDER = os.path.join("AT_LOG", "TEMP")

HEAP_SIZE_LIMIT = 30000


class SSLLowMem(PerformanceTCBase.PerformanceTCBase):

    def __init__(self, name, test_env, cmd_set, timeout=15, log_path=TCActionBase.LOG_PATH):
        PerformanceTCBase.PerformanceTCBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                     timeout=timeout, log_path=log_path)
        self.client_type = None
        self.server_type = None
        self.client_capability = dict()
        self.server_capability = dict()
        self.heap_usage_range = (10000, 30000)
        self.test_time = 120
        # load param from excel
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        timestamp = time.strftime("%d%H%M%S", time.localtime())
        self.heap_size_pattern = re.compile("\+FREEHEAP:(\d+)\r\n")

    @staticmethod
    def close(client, server):
        try:
            client.close()
        except StandardError:
            pass
        try:
            server.close()
        except StandardError:
            pass

    def query_heap_size(self, scenario="idle"):
        self.flush_data("SSC1")
        self.serial_write_line("SSC1", "ram -H")
        match = self.check_regular_expression("SSC1", self.heap_size_pattern)
        if match is None:
            NativeLog.add_trace_critical("No response for SSC ram command")
        else:
            heap_size = int(match.group(1))
            if heap_size < HEAP_SIZE_LIMIT and scenario == "idle":
                NativeLog.add_trace_critical("[HeapSize] %s" % heap_size)

        pass

    def prepare_handshake_test(self):
        # check if connected
        self.flush_data("SSC1")
        self.serial_write_line("SSC1", "sta -Q")
        if self.check_response("SSC1", "+JAP:CONNECTED,") is False:
            ap_ssid = self.get_parameter("ap_ssid")
            ap_password = self.get_parameter("ap_password")
            self.serial_write_line("SSC1", "sta -C -s %s -p %s" % (ap_ssid, ap_password))
            self.check_response("SSC1", "+JAP:CONNECTED,")
        # random alloc memory
        while True:
            memory_size = random.randint(self.heap_usage_range[0], self.heap_usage_range[1])
            self.serial_write_line("SSC1", "soc -M -l %s" % memory_size)
            if self.check_response("SSC1", "+SOC_BUFFER:OK", timeout=1) is True:
                break
        # query size
        self.query_heap_size()

    @staticmethod
    def log_data_to_file(file_name, data):
        with open(file_name, "ab+") as f:
            f.write(data+"\r\n")

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        # rewrite the following code
        if self.client_type == "PC":
            client_capability = Capability.PCSSLCapability("PCClient")
            client_handler = SSLHandler.PCSSLHandler
            client_ip = self.get_parameter("pc_ip")
        else:
            client_capability = Capability.TargetSSLCapability("TargetClient", **self.client_capability)
            client_handler = SSLHandler.TargetSSLHandler
            client_ip = self.get_parameter("target_ip")
        if self.server_type == "PC":
            server_capability = Capability.PCSSLCapability("PCServer")
            server_handler = SSLHandler.PCSSLHandler
            server_ip = self.get_parameter("pc_ip")
        else:
            server_capability = Capability.TargetSSLCapability("TargetServer", **self.server_capability)
            server_handler = SSLHandler.TargetSSLHandler
            server_ip = self.get_parameter("target_ip")

        test_time = self.test_time * 60  # convert test time from minutes to seconds

        serial_port = SSLHandler.SerialPort(self, "SSC1")
        # generate config
        config_utility = ConfigUtility.ConfigUtility(client_capability, server_capability)
        config_list_dict = config_utility.get_all_test_config()

        start_time = time.time()

        # succeed
        for config in config_list_dict["succeed"]:
            if time.time() - start_time > test_time:
                break
            self.prepare_handshake_test()
            port = random.randint(500, 50000)
            client = client_handler("client", config, serial_port)
            server = server_handler("server", config, serial_port)
            try:
                client.init_context()
                server.init_context()
                server.listen(local_ip=server_ip, local_port=port)
                client.connect(server_ip, port, local_ip=client_ip)
                self.query_heap_size(scenario="connected")
            except SSLHandler.TargetFail, e:
                NativeLog.add_exception_log(e)
            except SSLHandler.PCFail, e:
                NativeLog.add_exception_log(e)
            self.close(client, server)


def main():
    pass

if __name__ == '__main__':
    main()
