from __future__ import print_function, unicode_literals

import os
import re
import socket
import ssl
from threading import Event, Thread

import ttfw_idf

SERVER_CERTS_DIR = 'server_certs/'


def _path(f):
    return os.path.join(os.path.dirname(os.path.realpath(__file__)),f)


def get_my_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        # doesn't even have to be reachable
        s.connect(('10.255.255.255', 1))
        IP = s.getsockname()[0]
    except socket.error:
        IP = '127.0.0.1'
    finally:
        s.close()
    return IP


# Simple TLS server
class TlsServer:

    def __init__(self, port, negotiated_protocol=ssl.PROTOCOL_TLSv1):
        self.port = port
        self.socket = socket.socket()
        self.socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.socket.settimeout(20.0)
        self.shutdown = Event()
        self.negotiated_protocol = negotiated_protocol
        self.conn = None
        self.ssl_error = None
        self.server_thread = None

    def __enter__(self):
        try:
            self.socket.bind(('', self.port))
        except socket.error as e:
            print('Bind failed:{}'.format(e))
            raise

        self.socket.listen(1)
        self.server_thread = Thread(target=self.run_server)
        self.server_thread.start()

        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.shutdown.set()
        self.server_thread.join()
        self.socket.close()
        if (self.conn is not None):
            self.conn.close()

    def run_server(self):
        context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
        context.load_verify_locations(cafile=_path(SERVER_CERTS_DIR + 'ca.crt'))
        context.load_cert_chain(certfile=_path(SERVER_CERTS_DIR + 'server.crt'), keyfile=_path(SERVER_CERTS_DIR + 'server.key'))
        context.verify_flags = self.negotiated_protocol
        self.socket = context.wrap_socket(self.socket, server_side=True)
        try:
            print('Listening socket')
            self.conn, address = self.socket.accept()  # accept new connection
            self.socket.settimeout(20.0)
            print(' - connection from: {}'.format(address))
        except ssl.SSLError as e:
            self.conn = None
            self.ssl_error = str(e)
            print(' - SSLError: {}'.format(str(e)))


@ttfw_idf.idf_custom_test(env_tag='Example_WIFI', group='test-apps')
def test_app_esp_openssl(env, extra_data):
    dut1 = env.get_dut('openssl_connect_test', 'tools/test_apps/protocols/openssl', dut_class=ttfw_idf.ESP32DUT)
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, 'openssl_connect_test.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('openssl_connect_test_bin_size', '{}KB'.format(bin_size // 1024))
    dut1.start_app()
    esp_ip = dut1.expect(re.compile(r' IPv4 address: ([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)'), timeout=30)
    print('Got IP={}'.format(esp_ip[0]))
    ip = get_my_ip()
    server_port = 2222

    def start_case(case, desc, negotiated_protocol, result):
        with TlsServer(server_port, negotiated_protocol=negotiated_protocol):
            print('Starting {}: {}'.format(case, desc))
            dut1.write('conn {} {} {}'.format(ip, server_port, case))
            dut1.expect(re.compile(result), timeout=10)
            return case

    # start test cases
    start_case(
        case='CONFIG_TLSV1_1_CONNECT_WRONG_CERT_VERIFY_NONE',
        desc='Connect with verify_none mode using wrong certs',
        negotiated_protocol=ssl.PROTOCOL_TLSv1_1,
        result='SSL Connection Succeed')
    start_case(
        case='CONFIG_TLSV1_1_CONNECT_WRONG_CERT_VERIFY_PEER',
        desc='Connect with verify_peer mode using wrong certs',
        negotiated_protocol=ssl.PROTOCOL_TLSv1_1,
        result='SSL Connection Failed')
    start_case(
        case='CONFIG_TLSV1_2_CONNECT_WRONG_CERT_VERIFY_NONE',
        desc='Connect with verify_none mode using wrong certs',
        negotiated_protocol=ssl.PROTOCOL_TLSv1_2,
        result='SSL Connection Succeed')
    start_case(
        case='CONFIG_TLSV1_1_CONNECT_WRONG_CERT_VERIFY_PEER',
        desc='Connect with verify_peer mode using wrong certs',
        negotiated_protocol=ssl.PROTOCOL_TLSv1_2,
        result='SSL Connection Failed')


if __name__ == '__main__':
    test_app_esp_openssl()
