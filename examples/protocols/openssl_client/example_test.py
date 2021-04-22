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
        ctx = ssl.SSLContext(self.negotiated_protocol)
        ctx.load_cert_chain(certfile=_path(SERVER_CERTS_DIR + 'ca.crt'), keyfile=_path(SERVER_CERTS_DIR + 'ca.key'))
        self.socket = ctx.wrap_socket(self.socket, server_side=True)
        try:
            print('Listening socket')
            self.conn, address = self.socket.accept()  # accept new connection
            self.socket.settimeout(20.0)
            print(' - connection from: {}'.format(address))
        except ssl.SSLError as e:
            self.conn = None
            self.ssl_error = str(e)
            print(' - SSLError: {}'.format(str(e)))


def test_echo(dut):
    dut.expect('SSL Connection Succeed')
    print('SSL Connection Succeed')


@ttfw_idf.idf_example_test(env_tag='Example_WIFI')
def test_example_protocol_openssl_client(env, extra_data):
    """
     steps:
       1. join AP
       2. connect to uri "xxxx.xxxx.xxxx.xxxx:port"
       3. send and receive data
    """
    dut1 = env.get_dut('openssl_client', 'examples/protocols/openssl_client', dut_class=ttfw_idf.ESP32DUT)
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, 'openssl_client.bin')
    binary_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('openssl_client_bin_size', '{}KB'.format(binary_size // 1024))

    try:
        if 'CONFIG_EXAMPLE_OPENSSL_CLIENT_URI_FROM_STDIN' in dut1.app.get_sdkconfig():
            uri_from_stdin = True
        else:
            uri = dut1.app.get_sdkconfig()['CONFIG_EXAMPLE_OPENSSL_CLIENT_TARGET_DOMAIN'].strip('"')
            uri_from_stdin = False
    except Exception:
        print('ENV_TEST_FAILURE: Cannot find target domain in sdkconfig')
        raise

    # start test
    dut1.start_app()
    dut1.expect(re.compile(r' IPv4 address: ([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)'), timeout=30)
    ip = get_my_ip()

    if uri_from_stdin:
        server_port = 2222
        with TlsServer(server_port, negotiated_protocol=ssl.PROTOCOL_TLSv1_1):
            print('Starting test')
            dut1.write('{} {}'.format(ip, server_port))
            dut1.expect(re.compile('SSL Connection Succeed'), timeout=10)
    else:
        print('DUT connecting to {}'.format(uri))
        test_echo(dut1)


if __name__ == '__main__':
    test_example_protocol_openssl_client()
