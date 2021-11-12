import http.server
import multiprocessing
import os
import re
import socket
import ssl

import ttfw_idf
from RangeHTTPServer import RangeRequestHandler
from tiny_test_fw import DUT, Utility


def get_my_ip():
    s1 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s1.connect(('8.8.8.8', 80))
    my_ip = s1.getsockname()[0]
    s1.close()
    return my_ip


def get_server_status(host_ip, port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_status = sock.connect_ex((host_ip, port))
    sock.close()
    if server_status == 0:
        return True
    return False


def https_request_handler():
    """
    Returns a request handler class that handles broken pipe exception
    """
    class RequestHandler(RangeRequestHandler):
        protocol_version = 'HTTP/1.1'

        def finish(self):
            try:
                if not self.wfile.closed:
                    self.wfile.flush()
                    self.wfile.close()
            except socket.error:
                pass
            self.rfile.close()

        def handle(self):
            try:
                RangeRequestHandler.handle(self)
            except socket.error:
                pass

        def do_GET(self):
            self.close_connection = True
            self.send_response(200)
            self.end_headers()

    return RequestHandler


def start_https_server(server_file, key_file, server_ip, server_port):

    requestHandler = https_request_handler()
    httpd = http.server.HTTPServer((server_ip, server_port), requestHandler)

    httpd.socket = ssl.wrap_socket(httpd.socket, keyfile=key_file,
                                   certfile=server_file, server_side=True)
    httpd.serve_forever()


@ttfw_idf.idf_example_test(env_tag='Example_EthKitV1')
def test_examples_protocol_https_request_cli_session_tickets(env, extra_data):
    Utility.console_log("Testing for \"esp_tls client session tickets\"")

    dut1 = env.get_dut('https_request_ses_tkt', 'examples/protocols/https_request', dut_class=ttfw_idf.ESP32DUT, app_config_name='cli_ses_tkt')
    Utility.console_log('[app_config_name] - {}'.format(dut1.app.config_name))
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, 'https_request.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('https_request_bin_size', '{}KB'.format(bin_size // 1024))
    # start test
    host_ip = get_my_ip()
    server_port = 8070
    server_file = os.path.join(os.path.dirname(__file__), 'main', 'local_server_cert.pem')
    key_file = os.path.join(os.path.dirname(__file__), 'main', 'local_server_key.pem')
    if (get_server_status(host_ip, server_port) is False):
        thread1 = multiprocessing.Process(target=start_https_server, args=(server_file, key_file, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    Utility.console_log('The server started on {}:{}'.format(host_ip, server_port))
    dut1.start_app()

    dut1.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' (sta|eth) ip: ([^,]+),'), timeout=60)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')

    dut1.expect('Start https_request example', timeout=30)

    print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port)))

    dut1.write('https://' + host_ip + ':' + str(server_port))
    Utility.console_log("Testing for \"https_request using saved session\"")

    # Check for connection using already saved client session
    try:
        dut1.expect(re.compile('https_request to local server'), timeout=30)
        dut1.expect_all('Connection established...',
                        'Reading HTTP response...',
                        'HTTP/1.1 200 OK',
                        re.compile('connection closed'))
    except Exception:
        Utility.console_log("Failed to connect to local https server\"")
        raise

    try:
        dut1.expect(re.compile('https_request using saved client session'), timeout=20)
        dut1.expect_all('Connection established...',
                        'Reading HTTP response...',
                        'HTTP/1.1 200 OK',
                        re.compile('connection closed'))
    except Exception:
        Utility.console_log("Failed the test for \"https_request using saved client session\"")
        raise

    Utility.console_log("Passed the test for \"https_request using saved client session\"")
    thread1.terminate()
    env.close_dut('https_request_ses_tkt')


@ttfw_idf.idf_example_test(env_tag='Example_EthKitV1')
def test_examples_protocol_https_request_dynamic_buffers(env, extra_data):
    # Check for connection using crt bundle with mbedtls dynamic resource enabled
    dut1 = env.get_dut('https_request_ssldyn', 'examples/protocols/https_request', dut_class=ttfw_idf.ESP32DUT, app_config_name='ssldyn')
    # check and log bin size
    Utility.console_log('[app_config_name] - {}'.format(dut1.app.config_name))
    binary_file = os.path.join(dut1.app.binary_path, 'https_request.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('https_request_bin_size', '{}KB'.format(bin_size // 1024))
    # start test
    dut1.start_app()

    dut1.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' (sta|eth) ip: ([^,]+),'), timeout=60)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')

    # only check if one connection is established
    Utility.console_log("Testing for \"https_request using crt bundle\" with mbedtls dynamic resource enabled")
    try:
        dut1.expect(re.compile('https_request using crt bundle'), timeout=30)
        dut1.expect_all('Connection established...',
                        'Reading HTTP response...',
                        'HTTP/1.1 200 OK',
                        re.compile('connection closed'))
    except Exception:
        Utility.console_log("Failed the test for \"https_request using crt bundle\" when mbedtls dynamic resource was enabled")
        raise
    Utility.console_log("Passed the test for \"https_request using crt bundle\" when mbedtls dynamic resource was enabled")

    # Read free heap size
    res = dut1.expect(ttfw_idf.MINIMUM_FREE_HEAP_SIZE_RE,timeout=20)
    if not res:
        raise ValueError('Maximum heap size info not found')
    ttfw_idf.print_heap_size('https_request', dut1.app.config_name, dut1.TARGET, res[0])

    env.close_dut('https_request_ssldyn')


@ttfw_idf.idf_example_test(env_tag='Example_EthKitV1')
def test_examples_protocol_https_request(env, extra_data):

    """
    steps: |
      1. join AP
      2. establish TLS connection to www.howsmyssl.com:443 with multiple
         certificate verification options
      3. send http request
    """
    dut1 = env.get_dut('https_request', 'examples/protocols/https_request', dut_class=ttfw_idf.ESP32DUT)
    # check and log bin size
    Utility.console_log('[app_config_name] - {}'.format(dut1.app.config_name))
    binary_file = os.path.join(dut1.app.binary_path, 'https_request.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('https_request_bin_size', '{}KB'.format(bin_size // 1024))
    # start tes
    Utility.console_log('Starting https_request simple test app')
    dut1.start_app()

    dut1.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' (sta|eth) ip: ([^,]+),'), timeout=60)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')

    # Check for connection using crt bundle
    Utility.console_log("Testing for \"https_request using crt bundle\"")
    try:
        dut1.expect(re.compile('https_request using crt bundle'), timeout=30)
        dut1.expect_all('Certificate validated',
                        'Connection established...',
                        'Reading HTTP response...',
                        'HTTP/1.1 200 OK',
                        re.compile('connection closed'))
    except Exception:
        Utility.console_log("Failed the test for \"https_request using crt bundle\"")
        raise
    Utility.console_log("Passed the test for \"https_request using crt bundle\"")

    # Read free heap size
    res = dut1.expect(ttfw_idf.MINIMUM_FREE_HEAP_SIZE_RE,timeout=20)
    if not res:
        raise ValueError('Maximum heap size info not found')
    ttfw_idf.print_heap_size('https_request', dut1.app.config_name, dut1.TARGET, res[0])

    # Check for connection using cacert_buf
    Utility.console_log("Testing for \"https_request using cacert_buf\"")
    try:
        dut1.expect(re.compile('https_request using cacert_buf'), timeout=20)
        dut1.expect_all('Connection established...',
                        'Reading HTTP response...',
                        'HTTP/1.1 200 OK',
                        re.compile('connection closed'))
    except Exception:
        Utility.console_log("Passed the test for \"https_request using cacert_buf\"")
        raise
    Utility.console_log("Passed the test for \"https_request using cacert_buf\"")

    # Check for connection using global ca_store
    Utility.console_log("Testing for \"https_request using global ca_store\"")
    try:
        dut1.expect(re.compile('https_request using global ca_store'), timeout=20)
        dut1.expect_all('Connection established...',
                        'Reading HTTP response...',
                        'HTTP/1.1 200 OK',
                        re.compile('connection closed'))
    except Exception:
        Utility.console_log("Failed the test for \"https_request using global ca_store\"")
        raise
    Utility.console_log("Passed the test for \"https_request using global ca_store\"")
    env.close_dut('https_request')


if __name__ == '__main__':
    test_examples_protocol_https_request()
    test_examples_protocol_https_request_cli_session_tickets()
    test_examples_protocol_https_request_dynamic_buffers()
