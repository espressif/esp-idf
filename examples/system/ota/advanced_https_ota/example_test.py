import http.server
import os
import random
import re
import socket
import ssl
import struct
import subprocess
from threading import Thread

import ttfw_idf
from RangeHTTPServer import RangeRequestHandler
from tiny_test_fw import DUT, Utility

server_cert = '-----BEGIN CERTIFICATE-----\n' \
              'MIIDWDCCAkACCQCbF4+gVh/MLjANBgkqhkiG9w0BAQsFADBuMQswCQYDVQQGEwJJ\n'\
              'TjELMAkGA1UECAwCTUgxDDAKBgNVBAcMA1BVTjEMMAoGA1UECgwDRVNQMQwwCgYD\n'\
              'VQQLDANFU1AxDDAKBgNVBAMMA0VTUDEaMBgGCSqGSIb3DQEJARYLZXNwQGVzcC5j\n'\
              'b20wHhcNMjEwNzEyMTIzNjI3WhcNNDEwNzA3MTIzNjI3WjBuMQswCQYDVQQGEwJJ\n'\
              'TjELMAkGA1UECAwCTUgxDDAKBgNVBAcMA1BVTjEMMAoGA1UECgwDRVNQMQwwCgYD\n'\
              'VQQLDANFU1AxDDAKBgNVBAMMA0VTUDEaMBgGCSqGSIb3DQEJARYLZXNwQGVzcC5j\n'\
              'b20wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDhxF/y7bygndxPwiWL\n'\
              'SwS9LY3uBMaJgup0ufNKVhx+FhGQOu44SghuJAaH3KkPUnt6SOM8jC97/yQuc32W\n'\
              'ukI7eBZoA12kargSnzdv5m5rZZpd+NznSSpoDArOAONKVlzr25A1+aZbix2mKRbQ\n'\
              'S5w9o1N2BriQuSzd8gL0Y0zEk3VkOWXEL+0yFUT144HnErnD+xnJtHe11yPO2fEz\n'\
              'YaGiilh0ddL26PXTugXMZN/8fRVHP50P2OG0SvFpC7vghlLp4VFM1/r3UJnvL6Oz\n'\
              '3ALc6dhxZEKQucqlpj8l1UegszQToopemtIj0qXTHw2+uUnkUyWIPjPC+wdOAoap\n'\
              'rFTRAgMBAAEwDQYJKoZIhvcNAQELBQADggEBAItw24y565k3C/zENZlxyzto44ud\n'\
              'IYPQXN8Fa2pBlLe1zlSIyuaA/rWQ+i1daS8nPotkCbWZyf5N8DYaTE4B0OfvoUPk\n'\
              'B5uGDmbuk6akvlB5BGiYLfQjWHRsK9/4xjtIqN1H58yf3QNROuKsPAeywWS3Fn32\n'\
              '3//OpbWaClQePx6udRYMqAitKR+QxL7/BKZQsX+UyShuq8hjphvXvk0BW8ONzuw9\n'\
              'RcoORxM0FzySYjeQvm4LhzC/P3ZBhEq0xs55aL2a76SJhq5hJy7T/Xz6NFByvlrN\n'\
              'lFJJey33KFrAf5vnV9qcyWFIo7PYy2VsaaEjFeefr7q3sTFSMlJeadexW2Y=\n'\
              '-----END CERTIFICATE-----\n'

server_key = '-----BEGIN PRIVATE KEY-----\n'\
             'MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDhxF/y7bygndxP\n'\
             'wiWLSwS9LY3uBMaJgup0ufNKVhx+FhGQOu44SghuJAaH3KkPUnt6SOM8jC97/yQu\n'\
             'c32WukI7eBZoA12kargSnzdv5m5rZZpd+NznSSpoDArOAONKVlzr25A1+aZbix2m\n'\
             'KRbQS5w9o1N2BriQuSzd8gL0Y0zEk3VkOWXEL+0yFUT144HnErnD+xnJtHe11yPO\n'\
             '2fEzYaGiilh0ddL26PXTugXMZN/8fRVHP50P2OG0SvFpC7vghlLp4VFM1/r3UJnv\n'\
             'L6Oz3ALc6dhxZEKQucqlpj8l1UegszQToopemtIj0qXTHw2+uUnkUyWIPjPC+wdO\n'\
             'AoaprFTRAgMBAAECggEAE0HCxV/N1Q1h+1OeDDGL5+74yjKSFKyb/vTVcaPCrmaH\n'\
             'fPvp0ddOvMZJ4FDMAsiQS6/n4gQ7EKKEnYmwTqj4eUYW8yxGUn3f0YbPHbZT+Mkj\n'\
             'z5woi3nMKi/MxCGDQZX4Ow3xUQlITUqibsfWcFHis8c4mTqdh4qj7xJzehD2PVYF\n'\
             'gNHZsvVj6MltjBDAVwV1IlGoHjuElm6vuzkfX7phxcA1B4ZqdYY17yCXUnvui46z\n'\
             'Xn2kUTOOUCEgfgvGa9E+l4OtdXi5IxjaSraU+dlg2KsE4TpCuN2MEVkeR5Ms3Y7Q\n'\
             'jgJl8vlNFJDQpbFukLcYwG7rO5N5dQ6WWfVia/5XgQKBgQD74at/bXAPrh9NxPmz\n'\
             'i1oqCHMDoM9sz8xIMZLF9YVu3Jf8ux4xVpRSnNy5RU1gl7ZXbpdgeIQ4v04zy5aw\n'\
             '8T4tu9K3XnR3UXOy25AK0q+cnnxZg3kFQm+PhtOCKEFjPHrgo2MUfnj+EDddod7N\n'\
             'JQr9q5rEFbqHupFPpWlqCa3QmQKBgQDldWUGokNaEpmgHDMnHxiibXV5LQhzf8Rq\n'\
             'gJIQXb7R9EsTSXEvsDyqTBb7PHp2Ko7rZ5YQfyf8OogGGjGElnPoU/a+Jij1gVFv\n'\
             'kZ064uXAAISBkwHdcuobqc5EbG3ceyH46F+FBFhqM8KcbxJxx08objmh58+83InN\n'\
             'P9Qr25Xw+QKBgEGXMHuMWgQbSZeM1aFFhoMvlBO7yogBTKb4Ecpu9wI5e3Kan3Al\n'\
             'pZYltuyf+VhP6XG3IMBEYdoNJyYhu+nzyEdMg8CwXg+8LC7FMis/Ve+o7aS5scgG\n'\
             '1to/N9DK/swCsdTRdzmc/ZDbVC+TuVsebFBGYZTyO5KgqLpezqaIQrTxAoGALFCU\n'\
             '10glO9MVyl9H3clap5v+MQ3qcOv/EhaMnw6L2N6WVT481tnxjW4ujgzrFcE4YuxZ\n'\
             'hgwYu9TOCmeqopGwBvGYWLbj+C4mfSahOAs0FfXDoYazuIIGBpuv03UhbpB1Si4O\n'\
             'rJDfRnuCnVWyOTkl54gKJ2OusinhjztBjcrV1XkCgYEA3qNi4uBsPdyz9BZGb/3G\n'\
             'rOMSw0CaT4pEMTLZqURmDP/0hxvTk1polP7O/FYwxVuJnBb6mzDa0xpLFPTpIAnJ\n'\
             'YXB8xpXU69QVh+EBbemdJWOd+zp5UCfXvb2shAeG3Tn/Dz4cBBMEUutbzP+or0nG\n'\
             'vSXnRLaxQhooWm+IuX9SuBQ=\n'\
             '-----END PRIVATE KEY-----\n'


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


def create_file(server_file, file_data):
    with open(server_file, 'w+') as file:
        file.write(file_data)


def get_ca_cert(ota_image_dir):
    os.chdir(ota_image_dir)
    server_file = os.path.join(ota_image_dir, 'server_cert.pem')
    create_file(server_file, server_cert)

    key_file = os.path.join(ota_image_dir, 'server_key.pem')
    create_file(key_file, server_key)
    return server_file, key_file


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
    server_file, key_file = get_ca_cert(ota_image_dir)
    requestHandler = https_request_handler()
    httpd = http.server.HTTPServer((server_ip, server_port), requestHandler)

    httpd.socket = ssl.wrap_socket(httpd.socket,
                                   keyfile=key_file,
                                   certfile=server_file, server_side=True)
    httpd.serve_forever()


def start_chunked_server(ota_image_dir, server_port):
    server_file, key_file = get_ca_cert(ota_image_dir)
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
    server_file, key_file = get_ca_cert(ota_image_dir)
    redirectHandler = redirect_handler_factory('https://' + server_ip + ':' + str(redirection_port) + '/advanced_https_ota.bin')

    httpd = http.server.HTTPServer((server_ip, server_port), redirectHandler)

    httpd.socket = ssl.wrap_socket(httpd.socket,
                                   keyfile=key_file,
                                   certfile=server_file, server_side=True)
    httpd.serve_forever()


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_OTA')
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
        thread1 = Thread(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    dut1.start_app()
    for i in range(iterations):
        dut1.expect('Loaded app from partition at offset', timeout=30)
        try:
            ip_address = dut1.expect(re.compile(r' sta ip: ([^,]+),'), timeout=30)
            print('Connected to AP with IP: {}'.format(ip_address))
        except DUT.ExpectTimeout:
            raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
            thread1.close()
        dut1.expect('Starting Advanced OTA example', timeout=30)

        print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + bin_name))
        dut1.write('https://' + host_ip + ':' + str(server_port) + '/' + bin_name)
        dut1.expect('Loaded app from partition at offset', timeout=60)
        dut1.expect('Starting Advanced OTA example', timeout=30)
        dut1.reset()


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_OTA')
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
        thread1 = Thread(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' sta ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
    dut1.expect('Starting Advanced OTA example', timeout=30)

    print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + truncated_bin_name))
    dut1.write('https://' + host_ip + ':' + str(server_port) + '/' + truncated_bin_name)
    dut1.expect('Image validation failed, image is corrupted', timeout=30)
    os.remove(binary_file)


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_OTA')
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
        thread1 = Thread(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' sta ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
    dut1.expect('Starting Advanced OTA example', timeout=30)

    print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + truncated_bin_name))
    dut1.write('https://' + host_ip + ':' + str(server_port) + '/' + truncated_bin_name)
    dut1.expect('advanced_https_ota_example: esp_https_ota_read_img_desc failed', timeout=30)
    os.remove(binary_file)


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_OTA')
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
        thread1 = Thread(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' sta ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
    dut1.expect('Starting Advanced OTA example', timeout=30)

    print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + random_bin_name))
    dut1.write('https://' + host_ip + ':' + str(server_port) + '/' + random_bin_name)
    dut1.expect('esp_ota_ops: OTA image has invalid magic byte', timeout=10)
    os.remove(binary_file)


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_OTA')
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
        ip_address = dut1.expect(re.compile(r' sta ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
    dut1.expect('Starting Advanced OTA example', timeout=30)

    print('writing to device: {}'.format('https://' + host_ip + ':8070/' + bin_name))
    dut1.write('https://' + host_ip + ':8070/' + bin_name)
    dut1.expect('Loaded app from partition at offset', timeout=60)
    dut1.expect('Starting Advanced OTA example', timeout=30)
    chunked_server.kill()
    os.remove(os.path.join(dut1.app.binary_path, 'server_cert.pem'))
    os.remove(os.path.join(dut1.app.binary_path, 'server_key.pem'))


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_OTA')
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
    # Port to which the request should be redirecetd
    redirection_server_port = 8081
    # File to be downloaded. This file is generated after compilation
    bin_name = 'advanced_https_ota.bin'
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, bin_name)
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('advanced_https_ota_bin_size', '{}KB'.format(bin_size // 1024))
    # start test
    host_ip = get_my_ip()
    if (get_server_status(host_ip, server_port) is False):
        thread1 = Thread(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    thread2 = Thread(target=start_redirect_server, args=(dut1.app.binary_path, host_ip, redirection_server_port, server_port))
    thread2.daemon = True
    thread2.start()
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' sta ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
        thread1.close()
        thread2.close()
    dut1.expect('Starting Advanced OTA example', timeout=30)

    print('writing to device: {}'.format('https://' + host_ip + ':' + str(redirection_server_port) + '/' + bin_name))
    dut1.write('https://' + host_ip + ':' + str(redirection_server_port) + '/' + bin_name)
    dut1.expect('Loaded app from partition at offset', timeout=60)
    dut1.expect('Starting Advanced OTA example', timeout=30)
    dut1.reset()


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
        thread1 = Thread(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    dut1.start_app()
    # Positive Case
    dut1.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' eth ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
    dut1.expect('Starting Advanced OTA example', timeout=30)

    # Use originally generated image with secure_version=1
    print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + bin_name))
    dut1.write('https://' + host_ip + ':' + str(server_port) + '/' + bin_name)
    dut1.expect('Loaded app from partition at offset', timeout=60)
    dut1.expect(re.compile(r' eth ip: ([^,]+),'), timeout=30)
    dut1.expect('App is valid, rollback cancelled successfully', 30)

    # Negative Case
    dut1.expect('Starting Advanced OTA example', timeout=30)
    # Use modified image with secure_version=0
    print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + anti_rollback_bin_name))
    dut1.write('https://' + host_ip + ':' + str(server_port) + '/' + anti_rollback_bin_name)
    dut1.expect('New firmware security version is less than eFuse programmed, 0 < 1', timeout=30)
    os.remove(anti_rollback_bin_name)


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_OTA')
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
        thread1 = Thread(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' sta ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        Utility.console_log('ENV_TEST_FAILURE: Cannot connect to AP')
        raise
    dut1.expect('Starting Advanced OTA example', timeout=30)

    print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + bin_name))
    dut1.write('https://' + host_ip + ':' + str(server_port) + '/' + bin_name)
    for _ in range(http_requests):
        dut1.expect('Connection closed', timeout=60)
    dut1.expect('Loaded app from partition at offset', timeout=60)
    dut1.expect('Starting Advanced OTA example', timeout=30)
    dut1.reset()


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_OTA')
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
        thread1 = Thread(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' sta ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')

    dut1.expect('Starting Advanced OTA example', timeout=30)
    print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + bin_name))
    dut1.expect('GAP procedure initiated: advertise', timeout=30)
    print('Started GAP advertising.')

    dut1.write('https://' + host_ip + ':' + str(server_port) + '/' + bin_name)
    dut1.expect('Loaded app from partition at offset', timeout=60)
    dut1.expect('Starting Advanced OTA example', timeout=30)
    dut1.reset()


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_OTA')
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
        thread1 = Thread(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' sta ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')

    dut1.expect('Starting Advanced OTA example', timeout=30)
    print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + bin_name))
    dut1.expect('Started advertising.', timeout=30)
    print('Started GAP advertising.')

    dut1.write('https://' + host_ip + ':' + str(server_port) + '/' + bin_name)
    dut1.expect('Loaded app from partition at offset', timeout=60)
    dut1.expect('Starting Advanced OTA example', timeout=30)
    dut1.reset()


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_OTA')
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
        ip_address = dut1.expect(re.compile(r' sta ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
    dut1.expect('Starting Advanced OTA example', timeout=30)

    print('writing to device: {}'.format('https://' + host_ip + ':8070/' + aligned_bin_name))
    dut1.write('https://' + host_ip + ':8070/' + aligned_bin_name)
    dut1.expect('Loaded app from partition at offset', timeout=60)
    dut1.expect('Starting Advanced OTA example', timeout=30)
    chunked_server.kill()
    os.remove(os.path.join(dut1.app.binary_path, 'server_cert.pem'))
    os.remove(os.path.join(dut1.app.binary_path, 'server_key.pem'))
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
