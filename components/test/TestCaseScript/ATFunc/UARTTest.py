import socket
import serial

from TCAction import PerformanceTCBase
from TCAction import TCActionBase
from NativeLog import NativeLog


class UARTTest(PerformanceTCBase.PerformanceTCBase):
    def __init__(self, name, test_env, cmd_set, timeout=30, log_path=TCActionBase.LOG_PATH):
        PerformanceTCBase.PerformanceTCBase.__init__(self, name, test_env, cmd_set=cmd_set,
                                                     timeout=timeout, log_path=log_path)
        self.test_mode = "command"
        self.baudrate = None
        self.bytesize = None
        self.parity = None
        self.stopbits = None
        self.xonxoff = None
        self.rtscts = None

        # load param from excel
        for i in range(1, len(cmd_set)):
            if cmd_set[i][0] != "dummy":
                cmd_string = "self." + cmd_set[i][0]
                exec cmd_string
        self.result_cntx = TCActionBase.ResultCheckContext(self, test_env, self.tc_name)
        pass

    def cleanup(self):
        # restore UART config
        self.restore_serial_port("AT1")
        PerformanceTCBase.PerformanceTCBase.cleanup(self)

    STOP_BITS = {
        1: serial.STOPBITS_ONE,
        2: serial.STOPBITS_ONE_POINT_FIVE,
        3: serial.STOPBITS_TWO,
    }
    BYTE_SIZE = {
        5: serial.FIVEBITS,
        6: serial.SIXBITS,
        7: serial.SEVENBITS,
        8: serial.EIGHTBITS,
    }
    PARITY = {
        0: serial.PARITY_NONE,
        1: serial.PARITY_ODD,
        2: serial.PARITY_EVEN,
    }
    RTSCTS = {}

    def config_serial_port(self):
        port = self.test_env.get_port_by_name("AT1")
        kwargs = dict()
        if self.baudrate is not None:
            kwargs["baudrate"] = self.baudrate
        if self.bytesize is not None:
            kwargs["bytesize"] = self.BYTE_SIZE[self.bytesize]
        if self.parity is not None:
                kwargs["parity"] = self.PARITY[self.parity]
        if self.stopbits is not None:
            kwargs["stopbits"] = self.STOP_BITS[self.stopbits]
        if self.xonxoff is not None:
                kwargs["xonxoff"] = self.xonxoff
        if self.rtscts is not None:
            kwargs["rtscts"] = self.rtscts
        NativeLog.add_prompt_trace("[change PC UART config] %s" % kwargs)
        port.reconfig(**kwargs)

    def send_commands(self):
        # first change UART config
        self.config_serial_port()
        # do send commands
        for i in range(1, 256):
            cmd = bytes().join([chr(x % 256) for x in range(i)])
            try:
                self.serial_write_line("AT1", cmd)
            except StandardError, e:
                NativeLog.add_exception_log(e)
                pass
        self.flush_data("AT1")
        # restore UART config
        self.restore_serial_port("AT1")

    def send_data(self):
        # create TCP connection and enter send mode
        pc_ip = self.get_parameter("pc_ip")
        tcp_port = self.get_parameter("test_tcp_port1")
        server_sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
        server_sock.bind((pc_ip, tcp_port))
        server_sock.settimeout(10)
        server_sock.listen(5)
        self.serial_write_line("AT1", "AT+CIPSTART=\"TCP\",\"%s\",%s" % (pc_ip, tcp_port))
        self.check_response("AT1", "OK")
        sock, addr = server_sock.accept()
        server_sock.close()
        self.serial_write_line("AT1", "AT+CIPSEND=1460")
        self.check_response("AT1", ">")
        # change UART config
        self.config_serial_port()
        # send data
        try:
            self.serial_write("AT1", bytes().join([chr(x % 256) for x in range(146000)]))
        except StandardError, e:
            NativeLog.add_exception_log(e)
            pass
        sock.send("A"*1460)
        # restore UART config
        sock.close()
        self.restore_serial_port("AT1")

    def pass_through_mode(self):
        # create TCP connection and enter pass through mode
        pc_ip = self.get_parameter("pc_ip")
        tcp_port = self.get_parameter("test_tcp_port1")
        server_sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
        server_sock.bind((pc_ip, tcp_port))
        server_sock.settimeout(10)
        server_sock.listen(5)
        self.serial_write_line("AT1", "AT+CIPMODE=1")
        self.check_response("AT1", "OK")
        self.serial_write_line("AT1", "AT+CIPSTART=\"TCP\",\"%s\",%s" % (pc_ip, tcp_port))
        self.check_response("AT1", "OK")
        sock, addr = server_sock.accept()
        server_sock.close()
        self.serial_write_line("AT1", "AT+CIPSEND")
        self.check_response("AT1", ">")
        # change UART config
        self.config_serial_port()
        # send data
        try:
            self.serial_write("AT1", bytes().join([chr(x % 256) for x in range(146000)]))
        except StandardError, e:
            NativeLog.add_exception_log(e)
            pass
        sock.send("A" * 1460)
        # restore UART config
        sock.close()
        self.restore_serial_port("AT1")

    def execute(self):
        TCActionBase.TCActionBase.execute(self)
        # test sending command
        try:
            if self.test_mode == "command":
                self.send_commands()
            elif self.test_mode == "send_data":
                self.send_data()
            elif self.test_mode == "pass_through":
                self.pass_through_mode()
            else:
                raise StandardError("test mode not supported: %s" % self.test_mode)
            self.set_result("Succeed")
        except StandardError, e:
            NativeLog.add_exception_log(e)
            self.set_result("Failed")


def main():
    pass


if __name__ == '__main__':
    main()
