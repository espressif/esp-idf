import multiprocessing
import os
import re
import struct
import socket
import ssl

from tiny_test_fw import DUT
import ttfw_idf
import random
import subprocess

try:
    import BaseHTTPServer
    from SimpleHTTPServer import SimpleHTTPRequestHandler
except ImportError:
    import http.server as BaseHTTPServer
    from http.server import SimpleHTTPRequestHandler

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
    s1.connect(("8.8.8.8", 80))
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
    with open(server_file, "w+") as file:
        file.write(file_data)


def get_ca_cert(ota_image_dir):
    os.chdir(ota_image_dir)
    server_file = os.path.join(ota_image_dir, "server_cert.pem")
    create_file(server_file, server_cert)

    key_file = os.path.join(ota_image_dir, "server_key.pem")
    create_file(key_file, server_key)
    return server_file, key_file


def https_request_handler():
    """
    Returns a request handler class that handles broken pipe exception
    """
    class RequestHandler(SimpleHTTPRequestHandler):
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
                BaseHTTPServer.BaseHTTPRequestHandler.handle(self)
            except socket.error:
                pass

    return RequestHandler


def start_https_server(ota_image_dir, server_ip, server_port):
    server_file, key_file = get_ca_cert(ota_image_dir)
    requestHandler = https_request_handler()
    httpd = BaseHTTPServer.HTTPServer((server_ip, server_port), requestHandler)

    httpd.socket = ssl.wrap_socket(httpd.socket,
                                   keyfile=key_file,
                                   certfile=server_file, server_side=True)
    httpd.serve_forever()


def start_chunked_server(ota_image_dir, server_port):
    server_file, key_file = get_ca_cert(ota_image_dir)
    chunked_server = subprocess.Popen(["openssl", "s_server", "-WWW", "-key", key_file, "-cert", server_file, "-port", str(server_port)])
    return chunked_server


@ttfw_idf.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_native_ota_example(env, extra_data):
    """
    This is a positive test case, which downloads complete binary file multiple number of times.
    Number of iterations can be specified in variable iterations.
    steps: |
      1. join AP
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    dut1 = env.get_dut("native_ota_example", "examples/system/ota/native_ota_example", dut_class=ttfw_idf.ESP32DUT)
    server_port = 8002
    # No. of times working of application to be validated
    iterations = 3
    # File to be downloaded. This file is generated after compilation
    bin_name = "native_ota.bin"
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, bin_name)
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance("native_ota_bin_size", "{}KB".format(bin_size // 1024))
    ttfw_idf.check_performance("native_ota_bin_size", bin_size // 1024, dut1.TARGET)
    # start test
    host_ip = get_my_ip()
    if (get_server_status(host_ip, server_port) is False):
        thread1 = multiprocessing.Process(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    dut1.start_app()
    for i in range(iterations):
        dut1.expect("Loaded app from partition at offset", timeout=30)
        try:
            ip_address = dut1.expect(re.compile(r" sta ip: ([^,]+),"), timeout=30)
            print("Connected to AP with IP: {}".format(ip_address))
        except DUT.ExpectTimeout:
            raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
            thread1.terminate()
        dut1.expect('Starting OTA example', timeout=30)

        print("writing to device: {}".format("https://" + host_ip + ":" + str(server_port) + "/" + bin_name))
        dut1.write("https://" + host_ip + ":" + str(server_port) + "/" + bin_name)
        dut1.expect("Loaded app from partition at offset", timeout=60)
        dut1.expect("Starting OTA example", timeout=30)
        dut1.reset()
    thread1.terminate()


@ttfw_idf.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_native_ota_example_truncated_bin(env, extra_data):
    """
    Working of OTA if binary file is truncated is validated in this test case.
    Application should return with error message in this case.
    steps: |
      1. join AP
      2. Generate truncated binary file
      3. Fetch OTA image over HTTPS
      4. Check working of code if bin is truncated
    """
    dut1 = env.get_dut("native_ota_example", "examples/system/ota/native_ota_example", dut_class=ttfw_idf.ESP32DUT)
    server_port = 8002
    # Original binary file generated after compilation
    bin_name = "native_ota.bin"
    # Truncated binary file to be generated from original binary file
    truncated_bin_name = "truncated.bin"
    # Size of truncated file to be grnerated. This value can range from 288 bytes (Image header size) to size of original binary file
    # truncated_bin_size is set to 64000 to reduce consumed by the test case
    truncated_bin_size = 64000
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, bin_name)
    f = open(binary_file, "rb+")
    fo = open(os.path.join(dut1.app.binary_path, truncated_bin_name), "wb+")
    fo.write(f.read(truncated_bin_size))
    fo.close()
    f.close()
    binary_file = os.path.join(dut1.app.binary_path, truncated_bin_name)
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance("native_ota_bin_size", "{}KB".format(bin_size // 1024))
    ttfw_idf.check_performance("native_ota_bin_size", bin_size // 1024, dut1.TARGET)
    # start test
    host_ip = get_my_ip()
    if (get_server_status(host_ip, server_port) is False):
        thread1 = multiprocessing.Process(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    dut1.start_app()
    dut1.expect("Loaded app from partition at offset", timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r" sta ip: ([^,]+),"), timeout=60)
        print("Connected to AP with IP: {}".format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
        thread1.terminate()
    dut1.expect('Starting OTA example', timeout=30)

    print("writing to device: {}".format("https://" + host_ip + ":" + str(server_port) + "/" + truncated_bin_name))
    dut1.write("https://" + host_ip + ":" + str(server_port) + "/" + truncated_bin_name)
    dut1.expect("native_ota_example: Image validation failed, image is corrupted", timeout=20)
    os.remove(binary_file)
    thread1.terminate()


@ttfw_idf.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_native_ota_example_truncated_header(env, extra_data):
    """
    Working of OTA if headers of binary file are truncated is vaildated in this test case.
    Application should return with error message in this case.
    steps: |
      1. join AP
      2. Generate binary file with truncated headers
      3. Fetch OTA image over HTTPS
      4. Check working of code if headers are not sent completely
    """
    dut1 = env.get_dut("native_ota_example", "examples/system/ota/native_ota_example", dut_class=ttfw_idf.ESP32DUT)
    server_port = 8002
    # Original binary file generated after compilation
    bin_name = "native_ota.bin"
    # Truncated binary file to be generated from original binary file
    truncated_bin_name = "truncated_header.bin"
    # Size of truncated file to be grnerated. This value should be less than 288 bytes (Image header size)
    truncated_bin_size = 180
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, bin_name)
    f = open(binary_file, "rb+")
    fo = open(os.path.join(dut1.app.binary_path, truncated_bin_name), "wb+")
    fo.write(f.read(truncated_bin_size))
    fo.close()
    f.close()
    binary_file = os.path.join(dut1.app.binary_path, truncated_bin_name)
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance("native_ota_bin_size", "{}KB".format(bin_size // 1024))
    ttfw_idf.check_performance("native_ota_bin_size", bin_size // 1024, dut1.TARGET)
    # start test
    host_ip = get_my_ip()
    if (get_server_status(host_ip, server_port) is False):
        thread1 = multiprocessing.Process(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    dut1.start_app()
    dut1.expect("Loaded app from partition at offset", timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r" sta ip: ([^,]+),"), timeout=60)
        print("Connected to AP with IP: {}".format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
        thread1.terminate()
    dut1.expect('Starting OTA example', timeout=30)

    print("writing to device: {}".format("https://" + host_ip + ":" + str(server_port) + "/" + truncated_bin_name))
    dut1.write("https://" + host_ip + ":" + str(server_port) + "/" + truncated_bin_name)
    dut1.expect("native_ota_example: received package is not fit len", timeout=20)
    os.remove(binary_file)
    thread1.terminate()


@ttfw_idf.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_native_ota_example_random(env, extra_data):
    """
    Working of OTA if random data is added in binary file are validated in this test case.
    Magic byte verification should fail in this case.
    steps: |
      1. join AP
      2. Generate random binary image
      3. Fetch OTA image over HTTPS
      4. Check working of code for random binary file
    """
    dut1 = env.get_dut("native_ota_example", "examples/system/ota/native_ota_example", dut_class=ttfw_idf.ESP32DUT)
    server_port = 8002
    # Random binary file to be generated
    random_bin_name = "random.bin"
    # Size of random binary file. 32000 is choosen, to reduce the time required to run the test-case
    random_bin_size = 32000
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, random_bin_name)
    fo = open(binary_file, "wb+")
    # First byte of binary file is always set to zero. If first byte is generated randomly,
    # in some cases it may generate 0xE9 which will result in failure of testcase.
    fo.write(struct.pack("B", 0))
    for i in range(random_bin_size - 1):
        fo.write(struct.pack("B", random.randrange(0,255,1)))
    fo.close()
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance("native_ota_bin_size", "{}KB".format(bin_size // 1024))
    ttfw_idf.check_performance("native_ota_bin_size", bin_size // 1024, dut1.TARGET)
    # start test
    host_ip = get_my_ip()
    if (get_server_status(host_ip, server_port) is False):
        thread1 = multiprocessing.Process(target=start_https_server, args=(dut1.app.binary_path, host_ip, server_port))
        thread1.daemon = True
        thread1.start()
    dut1.start_app()
    dut1.expect("Loaded app from partition at offset", timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r" sta ip: ([^,]+),"), timeout=60)
        print("Connected to AP with IP: {}".format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
        thread1.terminate()
    dut1.expect('Starting OTA example', timeout=30)

    print("writing to device: {}".format("https://" + host_ip + ":" + str(server_port) + "/" + random_bin_name))
    dut1.write("https://" + host_ip + ":" + str(server_port) + "/" + random_bin_name)
    dut1.expect("esp_ota_ops: OTA image has invalid magic byte", timeout=20)
    os.remove(binary_file)
    thread1.terminate()


@ttfw_idf.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_native_ota_example_chunked(env, extra_data):
    """
    This is a positive test case, which downloads complete binary file multiple number of times.
    Number of iterations can be specified in variable iterations.
    steps: |
      1. join AP
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    dut1 = env.get_dut("native_ota_example", "examples/system/ota/native_ota_example", dut_class=ttfw_idf.ESP32DUT)
    # File to be downloaded. This file is generated after compilation
    bin_name = "native_ota.bin"
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, bin_name)
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance("native_ota_bin_size", "{}KB".format(bin_size // 1024))
    ttfw_idf.check_performance("native_ota_bin_size", bin_size // 1024, dut1.TARGET)
    # start test
    host_ip = get_my_ip()
    chunked_server = start_chunked_server(dut1.app.binary_path, 8070)
    dut1.start_app()
    dut1.expect("Loaded app from partition at offset", timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r" sta ip: ([^,]+),"), timeout=30)
        print("Connected to AP with IP: {}".format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')

    dut1.expect("Starting OTA example", timeout=30)
    print("writing to device: {}".format("https://" + host_ip + ":8070/" + bin_name))
    dut1.write("https://" + host_ip + ":8070/" + bin_name)
    dut1.expect("Loaded app from partition at offset", timeout=60)
    dut1.expect("Starting OTA example", timeout=30)
    chunked_server.kill()
    os.remove(os.path.join(dut1.app.binary_path, "server_cert.pem"))
    os.remove(os.path.join(dut1.app.binary_path, "server_key.pem"))


if __name__ == '__main__':
    test_examples_protocol_native_ota_example()
    test_examples_protocol_native_ota_example_chunked()
    test_examples_protocol_native_ota_example_truncated_bin()
    test_examples_protocol_native_ota_example_truncated_header()
    test_examples_protocol_native_ota_example_random()
