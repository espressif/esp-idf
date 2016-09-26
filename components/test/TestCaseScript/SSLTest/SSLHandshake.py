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


class SSLHandshake(PerformanceTCBase.PerformanceTCBase):

    def __init__(self, name, test_env, cmd_set, timeout=15, log_path=TCActionBase.LOG_PATH):
        PerformanceTCBase.PerformanceTCBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                     timeout=timeout, log_path=log_path)
        self.client_type = None
        self.server_type = None
        self.client_capability = dict()
        self.server_capability = dict()
        # load param from excel
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        timestamp = time.strftime("%d%H%M%S", time.localtime())
        folder = MakeFolder.make_folder(os.path.join(LOG_FOLDER, "SSLHandshake_%s" % timestamp))
        self.tested_log = os.path.join(folder, "SSLHandshakeTested.log")
        self.failed_log = os.path.join(folder, "SSLHandshakeFailed.log")
        self.memory_track_log = os.path.join(folder, "SSLHandshakeMemTrack.log")
        self.succeed_log = os.path.join(folder, "SSLHandshakeSucceed.log")
        # store test result for failed config
        self.failed_log2 = os.path.join(folder, "SSLHandshakeFailed2.log")
        self.succeed_log2 = os.path.join(folder, "SSLHandshakeSucceed2.log")

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
            self.log_memory("[heap size][%s] %s" % (scenario, heap_size))
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
        self.query_heap_size()

    @staticmethod
    def log_data_to_file(file_name, data):
        with open(file_name, "ab+") as f:
            f.write(data+"\r\n")

    def log_test_config(self, data):
        # append to log
        with self.sync_lock:
            _formatted_data = HTMLGenerator.process_one_log_item(data)
        self.append_to_log_file(_formatted_data)
        self.log_data_to_file(self.tested_log, data)

    def log_memory(self, data):
        self.log_data_to_file(self.memory_track_log, data)

    def log_fail(self, data, log_type="succeed"):
        print data
        if log_type == "succeed":
            self.log_data_to_file(self.failed_log, data)
        else:
            self.log_data_to_file(self.failed_log2, data)

    def log_succeed(self, data, log_type="succeed"):
        if log_type == "succeed":
            self.log_data_to_file(self.succeed_log, data)
        else:
            self.log_data_to_file(self.succeed_log2, data)

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

        serial_port = SSLHandler.SerialPort(self, "SSC1")
        # generate config
        config_utility = ConfigUtility.ConfigUtility(client_capability, server_capability)
        config_list_dict = config_utility.get_all_test_config()

        # succeed
        for config in config_list_dict["succeed"]:
            self.prepare_handshake_test()
            self.log_test_config("[Succeed config] %s" % config)
            port = random.randint(500, 50000)
            client = client_handler("client", config, serial_port)
            server = server_handler("server", config, serial_port)
            try:
                client.init_context()
                server.init_context()
                server.listen(local_ip=server_ip, local_port=port)
                client.connect(server_ip, port, local_ip=client_ip)
                self.query_heap_size(scenario="connected")
                self.log_succeed("[Succeed config] %s" % config)
            except SSLHandler.TargetFail, e:
                NativeLog.add_exception_log(e)
                self.log_fail("[Target][%s]\r\n[Failed][Succeed config] %s" % (e, config))
            except SSLHandler.PCFail, e:
                NativeLog.add_exception_log(e)
                self.log_fail("[PC][%s]\r\n[Failed][Succeed config] %s" % (e, config))

            self.close(client, server)

        # init context fail
        for config in config_list_dict["context_fail"]:
            self.prepare_handshake_test()
            port = random.randint(500, 50000)
            self.log_test_config("[Init context fail config] %s" % config)
            client = client_handler("client", config, serial_port)
            server = server_handler("server", config, serial_port)
            try:
                client.init_context()
                server.init_context()
                server.listen(local_ip=server_ip, local_port=port)
                client.connect(server_ip, port, local_ip=client_ip)
                self.log_fail("[Target]\r\n[Failed][Init context fail config] %s" % config, log_type="failed")
            except StandardError, e:
                NativeLog.add_exception_log(e)
                self.log_succeed("[init context fail] %s" % config, log_type="failed")
            self.close(client, server)
            pass

        # negotiation fail
        for config in config_list_dict["negotiation_fail"]:
            self.prepare_handshake_test()
            self.log_test_config("[negotiation_fail] %s" % config)
            port = random.randint(500, 50000)
            client = client_handler("client", config, serial_port)
            server = server_handler("server", config, serial_port)
            try:
                client.init_context()
                server.init_context()
                server.listen(local_ip=server_ip, local_port=port)
            except SSLHandler.TargetFail, e:
                NativeLog.add_exception_log(e)
                self.log_fail("[Target][%s]\r\n[Failed][negotiation fail config] %s" % (e, config), log_type="failed")
                self.close(client, server)
                continue
            except SSLHandler.PCFail, e:
                NativeLog.add_exception_log(e)
                self.log_fail("[PC][%s]\r\n[Failed][negotiation fail config] %s" % (e, config), log_type="failed")
                self.close(client, server)
                continue
            try:
                client.connect(server_ip, port, local_ip=client_ip)
                self.log_fail("[Target]\r\n[Failed][negotiation fail config] %s" % config, log_type="failed")
            except StandardError, e:
                NativeLog.add_exception_log(e)
                self.log_succeed("[negotiation fail] %s" % config, log_type="failed")
            self.close(client, server)

        # cert key fail
        for config in config_list_dict["cert_key_fail"]:
            self.prepare_handshake_test()
            self.log_test_config("[cert_key_fail] %s" % config)
            port = random.randint(500, 50000)
            client = client_handler("client", config, serial_port)
            server = server_handler("server", config, serial_port)
            try:
                client.init_context()
                server.init_context()
                server.listen(local_ip=server_ip, local_port=port)
            except SSLHandler.TargetFail, e:
                NativeLog.add_exception_log(e)
                self.log_fail("[Target][%s]\r\n[Failed][cert_key fail config] %s" % (e, config), log_type="failed")
                self.close(client, server)
                continue
            except SSLHandler.PCFail, e:
                NativeLog.add_exception_log(e)
                self.log_fail("[PC][%s]\r\n[Failed][cert_key fail config] %s" % (e, config), log_type="failed")
                self.close(client, server)
                continue
            try:
                client.connect(server_ip, port, local_ip=client_ip)
                self.log_fail("[Target][Failed][cert_key fail config] %s" % config, log_type="failed")
            except StandardError, e:
                NativeLog.add_exception_log(e)
                self.log_succeed("[cert_key_fail] %s" % config, log_type="failed")
            self.close(client, server)


def main():
    pass

if __name__ == '__main__':
    main()
