import random
import time

from TCAction import TCActionBase
from TCAction import PerformanceTCBase
from NativeLog import NativeLog
import ConfigUtility
import Capability
import SSLHandler


class SSLSendRecv(PerformanceTCBase.PerformanceTCBase):

    def __init__(self, name, test_env, cmd_set, timeout=15, log_path=TCActionBase.LOG_PATH):
        PerformanceTCBase.PerformanceTCBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                     timeout=timeout, log_path=log_path)
        self.target_role = "Client"
        self.max_send_len = 2048
        self.test_time = 120
        self.data_validation = False

        self.target_capability = {"version": ["SSLv23"],
                                  "cipher_suite": ["TLS_RSA_WITH_AES_256_CBC_SHA", "TLS_RSA_WITH_AES_128_CBC_SHA"],
                                  "fragment_size": [2048],
                                  "verify_server": False,
                                  "verify_client": False,
                                  "key_algorithm": ["RSA2048"],
                                  "key_encoding": ["PEM"],
                                  "pem_encryption": [None],
                                  "certificate_encoding": ["PEM"],
                                  "certificate_digest": ["sha1"],
                                  }
        # load param from excel
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string

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

    def cleanup(self):
        self.serial_write_line("SSC1", "ssl -D")
        self.check_response("SSC1", "SSL")

    def execute(self):
        TCActionBase.TCActionBase.execute(self)

        target_role = self.target_role
        max_send_len = self.max_send_len
        test_time = self.test_time * 60
        data_validation = self.data_validation

        ssl_port = random.randint(10000, 50000)
        NativeLog.add_prompt_trace("SSL port is %s" % ssl_port)

        # make sure ssl context deinit
        self.serial_write_line("SSC1", "ssl -D")
        self.check_response("SSC1", "SSL")

        # close all sockets and enlarge send buffer
        self.serial_write_line("SSC1", "soc -T")
        self.check_response("SSC1", "CLOSEALL")

        self.serial_write_line("SSC1", "soc -M -l %s" % max_send_len)
        self.check_response("SSC1", "+SOC_BUFFER:OK")

        # rewrite the following code
        if target_role == "Server":
            client_capability = Capability.PCSSLCapability("PCClient")
            client_handler = SSLHandler.PCSSLHandler
            client_ip = self.get_parameter("pc_ip")
            server_capability = Capability.TargetSSLCapability("TargetServer", **self.target_capability)
            server_handler = SSLHandler.TargetSSLHandler
            server_ip = self.get_parameter("target_ip")
        elif target_role == "Client":
            client_capability = Capability.TargetSSLCapability("TargetClient", **self.target_capability)
            client_handler = SSLHandler.TargetSSLHandler
            client_ip = self.get_parameter("target_ip")
            server_capability = Capability.PCSSLCapability("PCServer")
            server_handler = SSLHandler.PCSSLHandler
            server_ip = self.get_parameter("pc_ip")
        else:
            raise StandardError("Unsupported target role %s" % target_role)

        serial_port = SSLHandler.SerialPort(self, "SSC1")

        # generate one succeed config
        config_utility = ConfigUtility.ConfigUtility(client_capability, server_capability)
        config_list_dict = config_utility.get_all_test_config()

        for config in config_list_dict["succeed"]:
            try:
                # create connection
                NativeLog.add_prompt_trace(str(config))  # do print config
                client = client_handler("client", config, serial_port)
                server = server_handler("server", config, serial_port)
                client.init_context()
                server.init_context()
                server.listen(local_ip=server_ip, local_port=ssl_port)
                client.connect(server_ip, ssl_port, local_ip=client_ip)
            except StandardError, e:
                NativeLog.add_exception_log(e)
                return

            # set data validation
            client.set_data_validation(data_validation)
            server.set_data_validation(data_validation)

            # do send recv
            time_start = time.time()
            while time.time() - time_start < test_time:
                send_len = random.randint(1, max_send_len)
                try:
                    client.send(size=send_len)
                    client.send(size=send_len)
                    server.recv(send_len*2)
                except StandardError, e:
                    NativeLog.add_exception_log(e)
                    NativeLog.add_prompt_trace("client send / server recv fail")
                    break
                try:
                    # do send twice, try to create a tcp segment with 2 records
                    server.send(size=send_len)
                    server.send(size=send_len)
                    client.recv(send_len*2)
                except StandardError, e:
                    NativeLog.add_exception_log(e)
                    NativeLog.add_prompt_trace("server send / client recv fail")
                    break
            else:
                self.set_result("Succeed")


def main():
    pass

if __name__ == '__main__':
    main()
