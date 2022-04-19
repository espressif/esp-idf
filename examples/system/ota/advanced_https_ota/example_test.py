import http.server
import multiprocessing
import os
import random
import re
import socket
import ssl
import struct
import subprocess

import ttfw_idf
from RangeHTTPServer import RangeRequestHandler
from tiny_test_fw import DUT, Utility

server_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'test_certs/server_cert.pem')
key_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'test_certs/server_key.pem')


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

    return RequestHandler


def start_https_server(ota_image_dir, server_ip, server_port):
    os.chdir(ota_image_dir)
    requestHandler = https_request_handler()
    httpd = http.server.HTTPServer((server_ip, server_port), requestHandler)

    httpd.socket = ssl.wrap_socket(httpd.socket,
                                   keyfile=key_file,
                                   certfile=server_file, server_side=True)
    httpd.serve_forever()


def start_chunked_server(ota_image_dir, server_port):
    os.chdir(ota_image_dir)
    chunked_server = subprocess.Popen(['openssl', 's_server', '-WWW', '-key', key_file, '-cert', server_file, '-port', str(server_port)])
    return chunked_server


def redirect_handler_factory(url):
    """
    Returns a request handler class that redirects to supplied `url`
    """
    class RedirectHandler(http.server.SimpleHTTPRequestHandler):
        def do_GET(self):
            print('Sending resp, URL: ' + url)
            self.send_response(301)
            self.send_header('Location', url)
            self.end_headers()

        def handle(self):
            try:
                http.server.BaseHTTPRequestHandler.handle(self)
            except socket.error:
                pass

    return RedirectHandler


def start_redirect_server(ota_image_dir, server_ip, server_port, redirection_port):
    os.chdir(ota_image_dir)
    redirectHandler = redirect_handler_factory('https://' + server_ip + ':' + str(redirection_port) + '/advanced_https_ota.bin')

    httpd = http.server.HTTPServer((server_ip, server_port), redirectHandler)

    httpd.socket = ssl.wrap_socket(httpd.socket,
                                   keyfile=key_file,
                                   certfile=server_file, server_side=True)
    httpd.serve_forever()


@ttfw_idf.idf_example_test(env_tag='EXAMPLE_ETH_OTA')
def test_examples_protocol_advanced_https_ota_example(env, extra_data):
    """
    This is a positive test case, which downloads complete binary file multiple number of times.
    Number of iterations can be specified in variable iterations.
    steps: |
      1. join AP
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    dut1 = env.get_dut('advanced_https_ota_example', 'examples/system/ota/advanced_https_ota', dut_class=ttfw_idf.ESP32DUT)
    # Number of iterations to validate OTA
    iterations = 3
    server_port = 8001
    # File to be downloaded. This file is generated after compilation
    bin_name = 'advanced_https_ota.bin'
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, bin_name)
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('advanced_https_ota_bin_size', '{}KB'.format(bin_size // 1024))
    # start test
    host_ip = get_my_ip()
    if (get_server_status(host_ip, server_port) is False):
        thread1 = multiprocessing.Process(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    dut1.start_app()
    for i in range(iterations):
        dut1.expect('Loaded app from partition at offset', timeout=30)
        try:
            ip_address = dut1.expect(re.compile(r' (sta|eth) ip: ([^,]+),'), timeout=30)
            print('Connected to AP with IP: {}'.format(ip_address))
        except DUT.ExpectTimeout:
            raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
            thread1.terminate()
        dut1.expect('Starting Advanced OTA example', timeout=30)

        print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + bin_name))
        dut1.write('https://' + host_ip + ':' + str(server_port) + '/' + bin_name)
        dut1.expect('Loaded app from partition at offset', timeout=60)
        dut1.expect('Starting Advanced OTA example', timeout=30)
        dut1.reset()
    thread1.terminate()


@ttfw_idf.idf_example_test(env_tag='EXAMPLE_ETH_OTA')
def test_examples_protocol_advanced_https_ota_example_truncated_bin(env, extra_data):
    """
    Working of OTA if binary file is truncated is validated in this test case.
    Application should return with error message in this case.
    steps: |
      1. join AP
      2. Generate truncated binary file
      3. Fetch OTA image over HTTPS
      4. Check working of code if bin is truncated
    """
    dut1 = env.get_dut('advanced_https_ota_example', 'examples/system/ota/advanced_https_ota', dut_class=ttfw_idf.ESP32DUT)
    server_port = 8001
    # Original binary file generated after compilation
    bin_name = 'advanced_https_ota.bin'
    # Truncated binary file to be generated from original binary file
    truncated_bin_name = 'truncated.bin'
    # Size of truncated file to be grnerated. This value can range from 288 bytes (Image header size) to size of original binary file
    # truncated_bin_size is set to 64000 to reduce consumed by the test case
    truncated_bin_size = 64000
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, bin_name)
    f = open(binary_file, 'rb+')
    fo = open(os.path.join(dut1.app.binary_path, truncated_bin_name), 'wb+')
    fo.write(f.read(truncated_bin_size))
    fo.close()
    f.close()
    binary_file = os.path.join(dut1.app.binary_path, truncated_bin_name)
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('advanced_https_ota_bin_size', '{}KB'.format(bin_size // 1024))
    # start test
    host_ip = get_my_ip()
    if (get_server_status(host_ip, server_port) is False):
        thread1 = multiprocessing.Process(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' (sta|eth) ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
        thread1.terminate()
    dut1.expect('Starting Advanced OTA example', timeout=30)

    print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + truncated_bin_name))
    dut1.write('https://' + host_ip + ':' + str(server_port) + '/' + truncated_bin_name)
    dut1.expect('Image validation failed, image is corrupted', timeout=30)
    os.remove(binary_file)
    thread1.terminate()


@ttfw_idf.idf_example_test(env_tag='EXAMPLE_ETH_OTA')
def test_examples_protocol_advanced_https_ota_example_truncated_header(env, extra_data):
    """
    Working of OTA if headers of binary file are truncated is vaildated in this test case.
    Application should return with error message in this case.
    steps: |
      1. join AP
      2. Generate binary file with truncated headers
      3. Fetch OTA image over HTTPS
      4. Check working of code if headers are not sent completely
    """
    dut1 = env.get_dut('advanced_https_ota_example', 'examples/system/ota/advanced_https_ota', dut_class=ttfw_idf.ESP32DUT)
    server_port = 8001
    # Original binary file generated after compilation
    bin_name = 'advanced_https_ota.bin'
    # Truncated binary file to be generated from original binary file
    truncated_bin_name = 'truncated_header.bin'
    # Size of truncated file to be grnerated. This value should be less than 288 bytes (Image header size)
    truncated_bin_size = 180
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, bin_name)
    f = open(binary_file, 'rb+')
    fo = open(os.path.join(dut1.app.binary_path, truncated_bin_name), 'wb+')
    fo.write(f.read(truncated_bin_size))
    fo.close()
    f.close()
    binary_file = os.path.join(dut1.app.binary_path, truncated_bin_name)
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('advanced_https_ota_bin_size', '{}KB'.format(bin_size // 1024))
    # start test
    host_ip = get_my_ip()
    if (get_server_status(host_ip, server_port) is False):
        thread1 = multiprocessing.Process(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' (sta|eth) ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
        thread1.terminate()
    dut1.expect('Starting Advanced OTA example', timeout=30)

    print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + truncated_bin_name))
    dut1.write('https://' + host_ip + ':' + str(server_port) + '/' + truncated_bin_name)
    dut1.expect('advanced_https_ota_example: esp_https_ota_read_img_desc failed', timeout=30)
    os.remove(binary_file)
    thread1.terminate()


@ttfw_idf.idf_example_test(env_tag='EXAMPLE_ETH_OTA')
def test_examples_protocol_advanced_https_ota_example_random(env, extra_data):
    """
    Working of OTA if random data is added in binary file are validated in this test case.
    Magic byte verification should fail in this case.
    steps: |
      1. join AP
      2. Generate random binary image
      3. Fetch OTA image over HTTPS
      4. Check working of code for random binary file
    """
    dut1 = env.get_dut('advanced_https_ota_example', 'examples/system/ota/advanced_https_ota', dut_class=ttfw_idf.ESP32DUT)
    server_port = 8001
    # Random binary file to be generated
    random_bin_name = 'random.bin'
    # Size of random binary file. 32000 is choosen, to reduce the time required to run the test-case
    random_bin_size = 32000
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, random_bin_name)
    fo = open(binary_file, 'wb+')
    # First byte of binary file is always set to zero. If first byte is generated randomly,
    # in some cases it may generate 0xE9 which will result in failure of testcase.
    fo.write(struct.pack('B', 0))
    for i in range(random_bin_size - 1):
        fo.write(struct.pack('B', random.randrange(0,255,1)))
    fo.close()
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('advanced_https_ota_bin_size', '{}KB'.format(bin_size // 1024))
    # start test
    host_ip = get_my_ip()
    if (get_server_status(host_ip, server_port) is False):
        thread1 = multiprocessing.Process(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' (sta|eth) ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
        thread1.terminate()
    dut1.expect('Starting Advanced OTA example', timeout=30)

    print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + random_bin_name))
    dut1.write('https://' + host_ip + ':' + str(server_port) + '/' + random_bin_name)
    dut1.expect(re.compile(r'esp_https_ota: Mismatch chip id, expected 0, found \d'), timeout=10)
    os.remove(binary_file)
    thread1.terminate()


@ttfw_idf.idf_example_test(env_tag='EXAMPLE_ETH_OTA')
def test_examples_protocol_advanced_https_ota_example_chunked(env, extra_data):
    """
    This is a positive test case, which downloads complete binary file multiple number of times.
    Number of iterations can be specified in variable iterations.
    steps: |
      1. join AP
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    dut1 = env.get_dut('advanced_https_ota_example', 'examples/system/ota/advanced_https_ota', dut_class=ttfw_idf.ESP32DUT)
    # File to be downloaded. This file is generated after compilation
    bin_name = 'advanced_https_ota.bin'
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, bin_name)
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('advanced_https_ota_bin_size', '{}KB'.format(bin_size // 1024))
    # start test
    host_ip = get_my_ip()
    chunked_server = start_chunked_server(dut1.app.binary_path, 8070)
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' (sta|eth) ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
    dut1.expect('Starting Advanced OTA example', timeout=30)

    print('writing to device: {}'.format('https://' + host_ip + ':8070/' + bin_name))
    dut1.write('https://' + host_ip + ':8070/' + bin_name)
    dut1.expect('Loaded app from partition at offset', timeout=60)
    dut1.expect('Starting Advanced OTA example', timeout=30)
    chunked_server.kill()


@ttfw_idf.idf_example_test(env_tag='EXAMPLE_ETH_OTA')
def test_examples_protocol_advanced_https_ota_example_redirect_url(env, extra_data):
    """
    This is a positive test case, which starts a server and a redirection server.
    Redirection server redirects http_request to different port
    Number of iterations can be specified in variable iterations.
    steps: |
      1. join AP
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    dut1 = env.get_dut('advanced_https_ota_example', 'examples/system/ota/advanced_https_ota', dut_class=ttfw_idf.ESP32DUT)
    server_port = 8001
    # Port to which the request should be redirected
    redirection_server_port = 8081
    redirection_server_port1 = 8082
    # File to be downloaded. This file is generated after compilation
    bin_name = 'advanced_https_ota.bin'
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, bin_name)
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('advanced_https_ota_bin_size', '{}KB'.format(bin_size // 1024))
    # start test
    host_ip = get_my_ip()
    if (get_server_status(host_ip, server_port) is False):
        thread1 = multiprocessing.Process(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    thread2 = multiprocessing.Process(target=start_redirect_server, args=(dut1.app.binary_path, host_ip, redirection_server_port, redirection_server_port1))
    thread2.daemon = True
    thread2.start()
    thread3 = multiprocessing.Process(target=start_redirect_server, args=(dut1.app.binary_path, host_ip, redirection_server_port1, server_port))
    thread3.daemon = True
    thread3.start()
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' (sta|eth) ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        thread1.terminate()
        thread2.terminate()
        thread3.terminate()
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
    dut1.expect('Starting Advanced OTA example', timeout=30)

    print('writing to device: {}'.format('https://' + host_ip + ':' + str(redirection_server_port) + '/' + bin_name))
    dut1.write('https://' + host_ip + ':' + str(redirection_server_port) + '/' + bin_name)
    dut1.expect('Loaded app from partition at offset', timeout=60)
    dut1.expect('Starting Advanced OTA example', timeout=30)
    dut1.reset()
    thread1.terminate()
    thread2.terminate()
    thread3.terminate()


@ttfw_idf.idf_example_test(env_tag='Example_8Mflash_Ethernet')
def test_examples_protocol_advanced_https_ota_example_anti_rollback(env, extra_data):
    """
    Working of OTA when anti_rollback is enabled and security version of new image is less than current one.
    Application should return with error message in this case.
    steps: |
      1. join AP
      2. Generate binary file with lower security version
      3. Fetch OTA image over HTTPS
      4. Check working of anti_rollback feature
    """
    dut1 = env.get_dut('advanced_https_ota_example', 'examples/system/ota/advanced_https_ota', dut_class=ttfw_idf.ESP32DUT, app_config_name='anti_rollback')
    Utility.console_log('Erasing the flash on the chip')
    # erase the flash
    dut1.erase_flash()
    server_port = 8001
    # Original binary file generated after compilation
    bin_name = 'advanced_https_ota.bin'
    # Modified firmware image to lower security version in its header. This is to enable negative test case
    anti_rollback_bin_name = 'advanced_https_ota_lower_sec_version.bin'
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, bin_name)
    file_size = os.path.getsize(binary_file)
    f = open(binary_file, 'rb+')
    fo = open(os.path.join(dut1.app.binary_path, anti_rollback_bin_name), 'wb+')
    fo.write(f.read(file_size))
    # Change security_version to 0 for negative test case
    fo.seek(36)
    fo.write(b'\x00')
    fo.close()
    f.close()
    binary_file = os.path.join(dut1.app.binary_path, anti_rollback_bin_name)
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('advanced_https_ota_bin_size', '{}KB'.format(bin_size // 1024))
    # start test
    host_ip = get_my_ip()
    if (get_server_status(host_ip, server_port) is False):
        thread1 = multiprocessing.Process(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    dut1.start_app()
    # Positive Case
    dut1.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' (sta|eth) ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
        thread1.terminate()
    dut1.expect('Starting Advanced OTA example', timeout=30)

    # Use originally generated image with secure_version=1
    print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + bin_name))
    dut1.write('https://' + host_ip + ':' + str(server_port) + '/' + bin_name)
    dut1.expect('Loaded app from partition at offset', timeout=60)
    dut1.expect(re.compile(r' (sta|eth) ip: ([^,]+),'), timeout=30)
    dut1.expect('App is valid, rollback cancelled successfully', 30)

    # Negative Case
    dut1.expect('Starting Advanced OTA example', timeout=30)
    # Use modified image with secure_version=0
    print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + anti_rollback_bin_name))
    dut1.write('https://' + host_ip + ':' + str(server_port) + '/' + anti_rollback_bin_name)
    dut1.expect('New firmware security version is less than eFuse programmed, 0 < 1', timeout=30)
    os.remove(binary_file)
    thread1.terminate()


@ttfw_idf.idf_example_test(env_tag='EXAMPLE_ETH_OTA')
def test_examples_protocol_advanced_https_ota_example_partial_request(env, extra_data):
    """
    This is a positive test case, to test OTA workflow with Range HTTP header.
    steps: |
      1. join AP
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    dut1 = env.get_dut('advanced_https_ota_example', 'examples/system/ota/advanced_https_ota', dut_class=ttfw_idf.ESP32DUT, app_config_name='partial_download')
    server_port = 8001
    # Size of partial HTTP request
    request_size = 16384
    # File to be downloaded. This file is generated after compilation
    bin_name = 'advanced_https_ota.bin'
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, bin_name)
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('advanced_https_ota_bin_size', '{}KB'.format(bin_size // 1024))
    http_requests = int((bin_size / request_size) - 1)
    # start test
    host_ip = get_my_ip()
    if (get_server_status(host_ip, server_port) is False):
        thread1 = multiprocessing.Process(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' (sta|eth) ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        Utility.console_log('ENV_TEST_FAILURE: Cannot connect to AP')
        raise
        thread1.terminate()
    dut1.expect('Starting Advanced OTA example', timeout=30)

    print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + bin_name))
    dut1.write('https://' + host_ip + ':' + str(server_port) + '/' + bin_name)
    for _ in range(http_requests):
        dut1.expect('Connection closed', timeout=60)
    dut1.expect('Loaded app from partition at offset', timeout=60)
    dut1.expect('Starting Advanced OTA example', timeout=30)
    dut1.reset()
    thread1.terminate()


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_OTA', nightly_run=True)
def test_examples_protocol_advanced_https_ota_example_nimble_gatts(env, extra_data):
    """
    Run an OTA image update while a BLE GATT Server is running in background. This GATT server will be using NimBLE Host stack.
    steps: |
      1. join AP
      2. Run BLE advertise and then GATT server.
      3. Fetch OTA image over HTTPS
      4. Reboot with the new OTA image
    """
    dut1 = env.get_dut('advanced_https_ota_example', 'examples/system/ota/advanced_https_ota', dut_class=ttfw_idf.ESP32DUT, app_config_name='nimble')
    server_port = 8001
    # File to be downloaded. This file is generated after compilation
    bin_name = 'advanced_https_ota.bin'
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, bin_name)
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('advanced_https_ota_bin_size', '{}KB'.format(bin_size // 1024))
    # start test
    host_ip = get_my_ip()
    if (get_server_status(host_ip, server_port) is False):
        thread1 = multiprocessing.Process(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' sta ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
        thread1.terminate()

    dut1.expect('Starting Advanced OTA example', timeout=30)
    print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + bin_name))
    dut1.expect('GAP procedure initiated: advertise', timeout=30)
    print('Started GAP advertising.')

    dut1.write('https://' + host_ip + ':' + str(server_port) + '/' + bin_name)
    dut1.expect('Loaded app from partition at offset', timeout=60)
    dut1.expect('Starting Advanced OTA example', timeout=30)
    dut1.reset()
    thread1.terminate()


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_OTA', nightly_run=True)
def test_examples_protocol_advanced_https_ota_example_bluedroid_gatts(env, extra_data):
    """
    Run an OTA image update while a BLE GATT Server is running in background. This GATT server will be using Bluedroid Host stack.
    steps: |
      1. join AP
      2. Run BLE advertise and then GATT server.
      3. Fetch OTA image over HTTPS
      4. Reboot with the new OTA image
    """
    dut1 = env.get_dut('advanced_https_ota_example', 'examples/system/ota/advanced_https_ota', dut_class=ttfw_idf.ESP32DUT, app_config_name='bluedroid')
    server_port = 8001
    # File to be downloaded. This file is generated after compilation
    bin_name = 'advanced_https_ota.bin'
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, bin_name)
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('advanced_https_ota_bin_size', '{}KB'.format(bin_size // 1024))
    # start test
    host_ip = get_my_ip()
    if (get_server_status(host_ip, server_port) is False):
        thread1 = multiprocessing.Process(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' sta ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
        thread1.terminate()

    dut1.expect('Starting Advanced OTA example', timeout=30)
    print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + bin_name))
    dut1.expect('Started advertising.', timeout=30)
    print('Started GAP advertising.')

    dut1.write('https://' + host_ip + ':' + str(server_port) + '/' + bin_name)
    dut1.expect('Loaded app from partition at offset', timeout=60)
    dut1.expect('Starting Advanced OTA example', timeout=30)
    dut1.reset()
    thread1.terminate()


@ttfw_idf.idf_example_test(env_tag='EXAMPLE_ETH_OTA')
def test_examples_protocol_advanced_https_ota_example_openssl_aligned_bin(env, extra_data):
    """
    This is a test case for esp_http_client_read with binary size multiple of 289 bytes
    steps: |
      1. join AP
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    dut1 = env.get_dut('advanced_https_ota_example', 'examples/system/ota/advanced_https_ota', dut_class=ttfw_idf.ESP32DUT)
    # Original binary file generated after compilation
    bin_name = 'advanced_https_ota.bin'
    # Binary file aligned to DEFAULT_OTA_BUF_SIZE(289 bytes) boundary
    aligned_bin_name = 'aligned.bin'
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, bin_name)
    # Original binary size
    bin_size = os.path.getsize(binary_file)
    # Dummy data required to align binary size to 289 bytes boundary
    dummy_data_size = 289 - (bin_size % 289)
    f = open(binary_file, 'rb+')
    fo = open(os.path.join(dut1.app.binary_path, aligned_bin_name), 'wb+')
    fo.write(f.read(bin_size))
    for _ in range(dummy_data_size):
        fo.write(struct.pack('B', random.randrange(0,255,1)))
    fo.close()
    f.close()
    # start test
    host_ip = get_my_ip()
    chunked_server = start_chunked_server(dut1.app.binary_path, 8070)
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' (sta|eth) ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
    dut1.expect('Starting Advanced OTA example', timeout=30)

    print('writing to device: {}'.format('https://' + host_ip + ':8070/' + aligned_bin_name))
    dut1.write('https://' + host_ip + ':8070/' + aligned_bin_name)
    dut1.expect('Loaded app from partition at offset', timeout=60)
    dut1.expect('Starting Advanced OTA example', timeout=30)
    chunked_server.kill()
    os.remove(aligned_bin_name)


if __name__ == '__main__':
    test_examples_protocol_advanced_https_ota_example()
    test_examples_protocol_advanced_https_ota_example_chunked()
    test_examples_protocol_advanced_https_ota_example_redirect_url()
    test_examples_protocol_advanced_https_ota_example_truncated_bin()
    test_examples_protocol_advanced_https_ota_example_truncated_header()
    test_examples_protocol_advanced_https_ota_example_random()
    test_examples_protocol_advanced_https_ota_example_anti_rollback()
    test_examples_protocol_advanced_https_ota_example_partial_request()
    test_examples_protocol_advanced_https_ota_example_nimble_gatts()
    test_examples_protocol_advanced_https_ota_example_bluedroid_gatts()
    test_examples_protocol_advanced_https_ota_example_openssl_aligned_bin()
