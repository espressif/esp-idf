import socket
import ssl
import os
import re
import time
import threading

import Parameter
from PKI import PKIDict
from PKI import PKIItem
from NativeLog import NativeLog


class SerialPortCheckFail(StandardError):
    pass


class SSLHandlerFail(StandardError):
    pass


class PCFail(StandardError):
    pass


class TargetFail(StandardError):
    pass


def ssl_handler_wrapper(handler_type):
    if handler_type == "PC":
        exception_type = PCFail
    elif handler_type == "Target":
        exception_type = TargetFail
    else:
        exception_type = None

    def _handle_func(func):
        def _handle_args(*args, **kwargs):
            try:
                ret = func(*args, **kwargs)
            except StandardError, e:
                NativeLog.add_exception_log(e)
                raise exception_type(str(e))
            return ret
        return _handle_args
    return _handle_func


class SerialPort(object):
    def __init__(self, tc_action, port_name):
        self.tc_action = tc_action
        self.port_name = port_name

    def flush(self):
        self.tc_action.flush_data(self.port_name)

    def write_line(self, data):
        self.tc_action.serial_write_line(self.port_name, data)

    def check(self, condition, timeout=10):
        if self.tc_action.check_response(self.port_name, condition, timeout) is False:
            raise SerialPortCheckFail("serial port check fail, condition is %s" % condition)

    def read_data(self):
        return self.tc_action.serial_read_data(self.port_name)
    pass


class SSLHandler(object):
    # ssl operation timeout is 30 seconds
    TIMEOUT = 30

    def __init__(self, typ, config, serial_port):
        self.type = typ
        self.config = config
        self.timeout = self.TIMEOUT
        self.serial_port = serial_port
        self.accept_thread = None
        self.data_validation = False

    def set_timeout(self, timeout):
        self.timeout = timeout

    def init_context(self):
        pass

    def connect(self, remote_ip, remote_port, local_ip=0, local_port=0):
        pass

    def listen(self, local_port=0, local_ip=0):
        pass

    def send(self, size, data):
        pass

    def recv(self, length, timeout):
        pass

    def set_data_validation(self, validation):
        pass

    def close(self):
        if self.accept_thread is not None:
            self.accept_thread.exit()
            self.accept_thread.join(5)
    pass


class TargetSSLHandler(SSLHandler):
    def __init__(self, typ, config, serial_port):
        SSLHandler.__init__(self, typ, config, serial_port)
        self.ssl_id = None
        self.server_id = None

    @ssl_handler_wrapper("Target")
    def init_context(self):
        self.serial_port.flush()
        self.serial_port.write_line("soc -T")
        self.serial_port.check("+CLOSEALL")

        if self.type == "client":
            version = Parameter.VERSION[self.config["client_version"]]
            fragment = self.config["client_fragment_size"]
            ca = self.config["client_trust_anchor"]
            cert = self.config["client_certificate"]
            key = self.config["client_key"]
            verify_required = 0x01 if self.config["verify_server"] is True else 0x00
            context_type = 1
        else:
            version = Parameter.VERSION[self.config["server_version"]]
            fragment = self.config["server_fragment_size"]
            ca = self.config["server_trust_anchor"]
            cert = self.config["server_certificate"]
            key = self.config["server_key"]
            verify_required = 0x02 if self.config["verify_client"] is True else 0x00
            context_type = 2
        ssc_cmd = "ssl -I -t %u -r %u -v %u -o %u" % (context_type, fragment, version, verify_required)

        if ca is not None:
            _index = PKIDict.PKIDict.CERT_DICT[ca]
            ssc_cmd += " -a %d" % _index
        if cert is not None:
            _index = PKIDict.PKIDict.CERT_DICT[cert]
            ssc_cmd += " -c %d" % _index
        if key is not None:
            _index = PKIDict.PKIDict.KEY_DICT[key]
            ssc_cmd += " -k %d" % _index
        # write command and check result
        self.serial_port.flush()
        self.serial_port.write_line(ssc_cmd)
        self.serial_port.check(["+SSL:OK", "AND", "!+SSL:ERROR"])

    @ssl_handler_wrapper("Target")
    def connect(self, remote_ip, remote_port, local_ip=0, local_port=0):
        self.serial_port.flush()
        self.serial_port.write_line("soc -B -t SSL -i %s -p %s" % (local_ip, local_port))
        self.serial_port.check(["OK", "AND", "!ERROR"])
        self.serial_port.flush()
        self.serial_port.write_line("soc -C -s 0 -i %s -p %s" % (remote_ip, remote_port))
        self.serial_port.check(["OK", "AND", "!ERROR"], timeout=30)
        self.ssl_id = 0
        pass

    def accept_succeed(self):
        self.ssl_id = 1

    class Accept(threading.Thread):
        def __init__(self, serial_port, succeed_cb):
            threading.Thread.__init__(self)
            self.setDaemon(True)
            self.serial_port = serial_port
            self.succeed_cb = succeed_cb
            self.exit_flag = threading.Event()

        def run(self):
            while self.exit_flag.isSet() is False:
                try:
                    self.serial_port.check("+ACCEPT:", timeout=1)
                    self.succeed_cb()
                    break
                except StandardError:
                    pass

        def exit(self):
            self.exit_flag.set()

    @ssl_handler_wrapper("Target")
    def listen(self, local_port=0, local_ip=0):
        self.serial_port.flush()
        self.serial_port.write_line("soc -B -t SSL -i %s -p %s" % (local_ip, local_port))
        self.serial_port.check(["OK", "AND", "!ERROR"])
        self.serial_port.flush()
        self.serial_port.write_line("soc -L -s 0")
        self.serial_port.check(["OK", "AND", "!ERROR"])
        self.server_id = 0
        self.accept_thread = self.Accept(self.serial_port, self.accept_succeed)
        self.accept_thread.start()
        pass

    @ssl_handler_wrapper("Target")
    def send(self, size=10, data=None):
        if data is not None:
            size = len(data)
        self.serial_port.flush()
        self.serial_port.write_line("soc -S -s %s -l %s" % (self.ssl_id, size))
        self.serial_port.check(["OK", "AND", "!ERROR"])
        pass

    @ssl_handler_wrapper("Target")
    def recv(self, length, timeout=SSLHandler.TIMEOUT):
        pattern = re.compile("\+RECV:\d+,(\d+)\r\n")
        data_len = 0
        data = ""
        time1 = time.time()
        while time.time() - time1 < timeout:
            data += self.serial_port.read_data()
            if self.data_validation is True:
                if "+DATA_ERROR" in data:
                    raise SSLHandlerFail("target data validation fail")
            while True:
                match = pattern.search(data)
                if match is None:
                    break
                else:
                    data_len += int(match.group(1))
                    data = data[data.find(match.group())+len(match.group()):]
            if data_len >= length:
                result = True
                break
        else:
            result = False
        if result is False:
            raise SSLHandlerFail("Target recv fail")

    def set_data_validation(self, validation):
        self.data_validation = validation
        self.serial_port.flush()
        self.serial_port.write_line("soc -V -s %s -o %s" % (self.ssl_id, 1 if validation is True else 0))
        self.serial_port.check(["OK", "AND", "!ERROR"])

    @ssl_handler_wrapper("Target")
    def close(self):
        SSLHandler.close(self)
        self.serial_port.flush()
        self.serial_port.write_line("ssl -D")
        self.serial_port.check(["+SSL:OK", "OR", "+SSL:ERROR"])
        self.serial_port.write_line("soc -T")
        self.serial_port.check("+CLOSEALL")
        pass

    pass


def calc_hash(index):
    return (index & 0xffffffff) % 83 + (index & 0xffffffff) % 167


def verify_data(data, start_index):
    for i, c in enumerate(data):
        if ord(c) != calc_hash(start_index + i):
            NativeLog.add_trace_critical("[Data Validation Error] target sent data index %u is error."
                                         " Sent data is %x, should be %x"
                                         % (start_index + i, ord(c), calc_hash(start_index + i)))
            return False
    return True


def make_validation_data(length, start_index):
    return bytes().join([chr(calc_hash(start_index + i)) for i in range(length)])


class PCSSLHandler(SSLHandler):
    PROTOCOL_MAPPING = {
        "SSLv23": ssl.PROTOCOL_SSLv23,
        "SSLv23_2": ssl.PROTOCOL_SSLv23,
        "SSLv20": ssl.PROTOCOL_SSLv2,
        "SSLv30": ssl.PROTOCOL_SSLv3,
        "TLSv10": ssl.PROTOCOL_TLSv1,
        "TLSv11": ssl.PROTOCOL_TLSv1_1,
        "TLSv12": ssl.PROTOCOL_TLSv1_2,
    }
    CERT_FOLDER = os.path.join(".", "PKI", PKIDict.PKIDict.CERT_FOLDER)
    KEY_FOLDER = os.path.join(".", "PKI", PKIDict.PKIDict.KEY_FOLDER)

    def __init__(self, typ, config, serial_port):
        SSLHandler.__init__(self, typ, config, serial_port)
        self.ssl_context = None
        self.ssl = None
        self.server_sock = None
        self.send_index = 0
        self.recv_index = 0

    class InitContextThread(threading.Thread):
        def __init__(self, handler, version, cipher_suite, ca, cert, key, verify_required, remote_cert):
            threading.Thread.__init__(self)
            self.setDaemon(True)
            self.handler = handler
            self.version = version
            self.cipher_suite = cipher_suite
            self.ca = ca
            self.cert = cert
            self.key = key
            self.verify_required = verify_required
            self.remote_cert = remote_cert
            pass

        @staticmethod
        def handle_cert(cert_file, ca_file):
            cert = PKIItem.Certificate()
            cert.parse_file(cert_file)
            ca = PKIItem.Certificate()
            ca.parse_file(ca_file)
            if cert.file_encoding == "PEM" and ca.name in cert.cert_chain:
                cert_chain_t = cert.cert_chain[1:cert.cert_chain.index(ca.name)]
                ret = ["%s.pem" % c for c in cert_chain_t]
            else:
                ret = []
            return ret

        def run(self):
            try:
                ssl_context = ssl.SSLContext(self.version)
                # cipher suite
                ssl_context.set_ciphers(self.cipher_suite)
                if self.ca is not None:
                    ssl_context.load_verify_locations(cafile=os.path.join(self.handler.CERT_FOLDER, self.ca))
                    # python ssl can't verify cert chain, don't know why
                    # need to load cert between cert and ca for pem (pem cert contains cert chain)
                    if self.remote_cert is not None:
                        cert_chain = self.handle_cert(self.remote_cert, self.ca)
                        for c in cert_chain:
                            NativeLog.add_trace_info("load ca chain %s" % c)
                            ssl_context.load_verify_locations(cafile=os.path.join(self.handler.CERT_FOLDER, c))
                if self.cert is not None:
                    cert = os.path.join(self.handler.CERT_FOLDER, self.cert)
                    key = os.path.join(self.handler.KEY_FOLDER, self.key)
                    ssl_context.load_cert_chain(cert, keyfile=key)
                if self.verify_required is True:
                    ssl_context.verify_mode = ssl.CERT_REQUIRED
                else:
                    ssl_context.verify_mode = ssl.CERT_NONE
                self.handler.ssl_context = ssl_context
            except StandardError, e:
                NativeLog.add_exception_log(e)
                pass
        pass

    @ssl_handler_wrapper("PC")
    def init_context(self):
        if self.type == "client":
            version = self.PROTOCOL_MAPPING[self.config["client_version"]]
            cipher_suite = Parameter.CIPHER_SUITE[self.config["client_cipher_suite"]]
            ca = self.config["client_trust_anchor"]
            cert = self.config["client_certificate"]
            key = self.config["client_key"]
            verify_required = self.config["verify_server"]
            remote_cert = self.config["server_certificate"]
        else:
            version = self.PROTOCOL_MAPPING[self.config["server_version"]]
            cipher_suite = Parameter.CIPHER_SUITE[self.config["server_cipher_suite"]]
            ca = self.config["server_trust_anchor"]
            cert = self.config["server_certificate"]
            key = self.config["server_key"]
            verify_required = self.config["verify_client"]
            remote_cert = self.config["client_certificate"]

        _init_context = self.InitContextThread(self, version, cipher_suite, ca, cert, key, verify_required, remote_cert)
        _init_context.start()
        _init_context.join(5)
        if self.ssl_context is None:
            raise StandardError("Init Context Fail")

        pass

    @ssl_handler_wrapper("PC")
    def connect(self, remote_ip, remote_port, local_ip=0, local_port=0):
        sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
        # reuse socket in TIME_WAIT state
        sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sock.settimeout(self.timeout)
        sock.bind((local_ip, local_port))
        self.ssl = self.ssl_context.wrap_socket(sock)
        self.ssl.connect((remote_ip, remote_port))
        pass

    def accept_succeed(self, ssl_new):
        ssl_new.settimeout(self.timeout)
        self.ssl = ssl_new

    class Accept(threading.Thread):
        def __init__(self, server_sock, ssl_context, succeed_cb):
            threading.Thread.__init__(self)
            self.setDaemon(True)
            self.server_sock = server_sock
            self.ssl_context = ssl_context
            self.succeed_cb = succeed_cb
            self.exit_flag = threading.Event()

        def run(self):
            while self.exit_flag.isSet() is False:
                try:
                    new_socket, addr = self.server_sock.accept()
                    ssl_new = self.ssl_context.wrap_socket(new_socket, server_side=True)
                    self.succeed_cb(ssl_new)
                    break
                except StandardError:
                    pass
            pass

        def exit(self):
            self.exit_flag.set()

    @ssl_handler_wrapper("PC")
    def listen(self, local_port=0, local_ip=0):
        self.server_sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
        # reuse socket in TIME_WAIT state
        self.server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.server_sock.settimeout(1)
        self.server_sock.bind((local_ip, local_port))
        self.server_sock.listen(5)
        self.accept_thread = self.Accept(self.server_sock, self.ssl_context, self.accept_succeed)
        self.accept_thread.start()
        pass

    @ssl_handler_wrapper("PC")
    def send(self, size=10, data=None):
        if data is None:
            self.ssl.send(make_validation_data(size, self.send_index))
            if self.data_validation is True:
                self.send_index += size
        else:
            self.ssl.send(data)

    @ssl_handler_wrapper("PC")
    def recv(self, length, timeout=SSLHandler.TIMEOUT, data_validation=False):
        time1 = time.time()
        data_len = 0
        while time.time() - time1 < timeout:
            data = self.ssl.read()

            if data_validation is True and len(data) > 0:
                if verify_data(data, self.recv_index) is False:
                    raise SSLHandlerFail("PC data validation fail, index is %s" % self.recv_index)
                self.recv_index += len(data)
            data_len += len(data)
            if data_len >= length:
                result = True
                break
        else:
            result = False
        if result is False:
            raise SSLHandlerFail("PC recv fail")

    def set_data_validation(self, validation):
        self.data_validation = validation

    @ssl_handler_wrapper("PC")
    def close(self):
        SSLHandler.close(self)
        if self.ssl is not None:
            self.ssl.close()
            self.ssl = None
        if self.server_sock is not None:
            self.server_sock.close()
            self.server_sock = None
        del self.ssl_context


def main():
    ssl_context = ssl.SSLContext(ssl.PROTOCOL_SSLv23)
    # cipher suite
    ssl_context.set_ciphers("AES256-SHA")
    ssl_context.load_cert_chain("D:\workspace\\auto_test_script\PKI\Certificate\\"
                                "L2CertRSA512sha1_L1CertRSA512sha1_RootCertRSA512sha1.pem",
                                keyfile="D:\workspace\\auto_test_script\PKI\Key\PrivateKey2RSA512.pem")
    ssl_context.verify_mode = ssl.CERT_NONE
    server_sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
    # reuse socket in TIME_WAIT state
    server_sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    server_sock.settimeout(100)
    server_sock.bind(("192.168.111.5", 443))
    server_sock.listen(5)
    while True:
        try:
            new_socket, addr = server_sock.accept()
            ssl_new = ssl_context.wrap_socket(new_socket, server_side=True)
            print "server connected"
            break
        except StandardError:
            pass


pass


if __name__ == '__main__':
    main()
