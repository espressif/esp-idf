from __future__ import print_function, unicode_literals

import os
import re
import socket
import ssl

import ttfw_idf


def _path(f):
    return os.path.join(os.path.dirname(os.path.realpath(__file__)),f)


@ttfw_idf.idf_example_test(env_tag='Example_WIFI')
def test_example_protocol_openssl_server(env, extra_data):
    """
     steps:
       1. join AP
       2. connect to uri "xxxx.xxxx.xxxx.xxxx:port"
       3. send data
    """
    dut1 = env.get_dut('openssl_server', 'examples/protocols/openssl_server', dut_class=ttfw_idf.ESP32DUT)
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, 'openssl_server.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('openssl_server_bin_size', '{}KB'.format(bin_size // 1024))
    # start test
    dut1.start_app()
    ip = dut1.expect(re.compile(r' IPv4 address: ([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)'), timeout=30)[0]
    port = dut1.expect(re.compile(r' SSL server socket listen on ([0-9]+)'), timeout=30)[0]
    # create socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    sock.settimeout(10)
    addr = (ip, int(port))
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM, 0)
    # wrap socket
    wrappedSocket = ssl.wrap_socket(sock, ca_certs=_path('server_certs/ca.crt'), cert_reqs=ssl.CERT_REQUIRED)
    # connect and send data
    wrappedSocket.connect(addr)
    wrappedSocket.send('Some Data'.encode())
    # close socket connection
    wrappedSocket.close()


if __name__ == '__main__':
    test_example_protocol_openssl_server()
