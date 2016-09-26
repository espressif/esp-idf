import time
import os
import socket
import ssl

from NativeLog import NativeLog
from TCAction import PerformanceTCBase
from Utility import MakeFolder


SEND_CMD = ("CIPSEND, CIPSENDBUF", "CIPSENDEX")

LOG_PATH = os.path.join("AT_LOG", "Performance", "AT_SEND")


class TCPSendPerf(PerformanceTCBase.PerformanceTCBase):
    def __init__(self, name, test_env, cmd_set, timeout=120, log_path=None):
        PerformanceTCBase.PerformanceTCBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                     timeout=timeout, log_path=log_path)
        # init value for ip and port
        self.pc_ip = "pc_ip"
        self.server_port = "test_tcp_port1"
        self.packet_len = 1
        self.test_count = 100
        self.send_cmd = "CIPSEND"
        self.baudrate = 115200
        self.rtscts = 0
        self.link_type = "TCP"
        # load param from excel
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        pass

    def process(self):
        pc_ip = self.get_parameter(self.pc_ip)
        server_port = self.get_parameter(self.server_port)
        packet_len = self.packet_len
        test_count = self.test_count
        send_cmd = self.send_cmd
        baudrate = self.baudrate
        rtscts = self.rtscts
        result = True
        link_type = self.link_type

        # create TCP connection
        sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
        sock.bind((pc_ip, server_port))
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.settimeout(10)
        sock.listen(1)

        self.serial_write_line("AT1", "AT+CIPSTART=0,\"%s\",\"%s\",%d" % (link_type, pc_ip, server_port))
        sock_client = sock.accept()[0]
        if link_type == "SSL":
            sock_client = ssl.wrap_socket(sock_client,
                                          server_side=True,
                                          certfile=os.path.join("Certificate", "default.cer"),
                                          keyfile=os.path.join("Certificate", "default.key"))
            pass
        if self.check_response("AT1", "OK") is False:
            result = False

        self.serial_write_line("AT1", "AT+UART_CUR=%d,8,1,0,%d" % (baudrate, rtscts))
        if self.check_response("AT1", "OK\r\n") is False:
            result = False

        self.reconfig_serial_port("AT1", baudrate, rtscts)

        # restore to read line mode
        self.test_env.uart_ports["AT1"].set_performance_flag(flag=True)

        sock_client.settimeout(0)

        for _dummy in range(1):
            if result is False:
                NativeLog.add_trace_critical("Fail to create TCP connection")
                break
            # send TCP packets
            data = "A" * packet_len
            time1 = time.time()

            i = 0
            data_recv_len = 0
            while i < test_count:
                self.serial_write_line("AT1", "AT+%s=0,%d" % (send_cmd, packet_len))
                if self.check_response("AT1", ">", 0.05) is False:
                    continue

                i += 1
                self.serial_write("AT1", data)
                if send_cmd == "CIPSENDBUF":
                    result = self.check_response("AT1", "Recv %d bytes" % packet_len, 3)
                else:
                    result = self.check_response("AT1", "SEND OK", 3)
                if result is False:
                    NativeLog.add_trace_critical("Fail during sending data")
                    break
                try:
                    if link_type == "TCP":
                        data_recv = sock_client.recv(10*1460)
                    elif link_type == "SSL":
                        data_recv = sock_client.read(10*1024)
                    else:
                        raise StandardError()
                    data_recv_len += len(data_recv)
                except socket.error, e:
                    if e.errno == 10035:
                        pass
                    elif e.message == "The read operation timed out":
                        pass
                    else:
                        NativeLog.add_exception_log(e)
            else:
                self.set_result("Succeed")

            time2 = time.time()

            folder_path = MakeFolder.make_folder(LOG_PATH)
            file_name = os.path.join(folder_path,
                                     "%s_%s_%s.log" % (send_cmd,
                                                       packet_len,
                                                       time.strftime("%d%H%M%S", time.localtime())))
            with open(file_name, "ab+") as f:
                f.write("\r\n[performance] %f packets per second "
                        "(including failed send operation)"
                        % (test_count/(time2-time1)))
                f.write("\r\n[performance] %f Kbps" % (data_recv_len/(125*(time2-time1))))

        self.serial_write_line("AT1", "AT+UART_CUR=115200,8,1,0,3")
        self.check_response("AT1", "OK\r\n")
        self.restore_serial_port("AT1")

        # restore to read line mode
        self.test_env.uart_ports["AT1"].set_performance_flag(flag=False)
        # close socket
        sock.close()
        sock_client.close()
        pass


def main():
    pass


if __name__ == '__main__':
    main()
