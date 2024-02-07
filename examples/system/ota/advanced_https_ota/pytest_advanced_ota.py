# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import http.server
import multiprocessing
import os
import random
import socket
import ssl
import struct
import subprocess
import time
from typing import Callable

import pexpect
import pytest
from common_test_methods import get_env_config_variable
from common_test_methods import get_host_ip4_by_dest_ip
from pytest_embedded import Dut
from RangeHTTPServer import RangeRequestHandler

server_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'test_certs/server_cert.pem')
key_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'test_certs/server_key.pem')


def https_request_handler() -> Callable[...,http.server.BaseHTTPRequestHandler]:
    """
    Returns a request handler class that handles broken pipe exception
    """
    class RequestHandler(RangeRequestHandler):
        def finish(self) -> None:
            try:
                if not self.wfile.closed:
                    self.wfile.flush()
                    self.wfile.close()
            except socket.error:
                pass
            self.rfile.close()

        def handle(self) -> None:
            try:
                RangeRequestHandler.handle(self)
            except socket.error:
                pass

    return RequestHandler


def start_https_server(ota_image_dir: str, server_ip: str, server_port: int) -> None:
    os.chdir(ota_image_dir)
    requestHandler = https_request_handler()
    httpd = http.server.HTTPServer((server_ip, server_port), requestHandler)

    httpd.socket = ssl.wrap_socket(httpd.socket,
                                   keyfile=key_file,
                                   certfile=server_file, server_side=True)
    httpd.serve_forever()


def start_chunked_server(ota_image_dir: str, server_port: int) -> subprocess.Popen:
    os.chdir(ota_image_dir)
    chunked_server = subprocess.Popen(['openssl', 's_server', '-WWW', '-key', key_file, '-cert', server_file, '-port', str(server_port)])
    return chunked_server


def redirect_handler_factory(url: str) -> Callable[...,http.server.BaseHTTPRequestHandler]:
    """
    Returns a request handler class that redirects to supplied `url`
    """
    class RedirectHandler(http.server.SimpleHTTPRequestHandler):
        def do_GET(self) -> None:
            print('Sending resp, URL: ' + url)
            self.send_response(301)
            self.send_header('Location', url)
            self.end_headers()

        def handle(self) -> None:
            try:
                http.server.BaseHTTPRequestHandler.handle(self)
            except socket.error:
                pass

    return RedirectHandler


def start_redirect_server(ota_image_dir: str, server_ip: str, server_port: int, redirection_port: int) -> None:
    os.chdir(ota_image_dir)
    redirectHandler = redirect_handler_factory('https://' + server_ip + ':' + str(redirection_port) + '/advanced_https_ota.bin')

    httpd = http.server.HTTPServer((server_ip, server_port), redirectHandler)

    httpd.socket = ssl.wrap_socket(httpd.socket,
                                   keyfile=key_file,
                                   certfile=server_file, server_side=True)
    httpd.serve_forever()


@pytest.mark.esp32
@pytest.mark.ethernet_ota
def test_examples_protocol_advanced_https_ota_example(dut: Dut) -> None:
    """
    This is a positive test case, which downloads complete binary file multiple number of times.
    Number of iterations can be specified in variable iterations.
    steps: |
      1. join AP/Ethernet
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    # Number of iterations to validate OTA
    iterations = 3
    server_port = 8001
    bin_name = 'advanced_https_ota.bin'
    # Start server
    thread1 = multiprocessing.Process(target=start_https_server, args=(dut.app.binary_path, '0.0.0.0', server_port))
    thread1.daemon = True
    thread1.start()
    try:
        # start test
        for _ in range(iterations):
            dut.expect('Loaded app from partition at offset', timeout=30)
            try:
                ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
                print('Connected to AP/Ethernet with IP: {}'.format(ip_address))
            except pexpect.exceptions.TIMEOUT:
                raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP/Ethernet')
            dut.expect('Starting Advanced OTA example', timeout=30)
            host_ip = get_host_ip4_by_dest_ip(ip_address)

            print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + bin_name))
            dut.write('https://' + host_ip + ':' + str(server_port) + '/' + bin_name)
            dut.expect('upgrade successful. Rebooting ...', timeout=150)
    finally:
        thread1.terminate()


@pytest.mark.esp32
@pytest.mark.ethernet_ota
def test_examples_protocol_advanced_https_ota_example_truncated_bin(dut: Dut) -> None:
    """
    Working of OTA if binary file is truncated is validated in this test case.
    Application should return with error message in this case.
    steps: |
      1. join AP/Ethernet
      2. Generate truncated binary file
      3. Fetch OTA image over HTTPS
      4. Check working of code if bin is truncated
    """
    server_port = 8001
    # Original binary file generated after compilation
    bin_name = 'advanced_https_ota.bin'
    # Truncated binary file to be generated from original binary file
    truncated_bin_name = 'truncated.bin'
    # Size of truncated file to be grnerated. This value can range from 288 bytes (Image header size) to size of original binary file
    # truncated_bin_size is set to 64000 to reduce consumed by the test case
    truncated_bin_size = 64000
    binary_file = os.path.join(dut.app.binary_path, bin_name)
    with open(binary_file, 'rb+') as f:
        with open(os.path.join(dut.app.binary_path, truncated_bin_name), 'wb+') as fo:
            fo.write(f.read(truncated_bin_size))
    binary_file = os.path.join(dut.app.binary_path, truncated_bin_name)
    # Start server
    thread1 = multiprocessing.Process(target=start_https_server, args=(dut.app.binary_path, '0.0.0.0', server_port))
    thread1.daemon = True
    thread1.start()
    try:
        # start test
        dut.expect('Loaded app from partition at offset', timeout=30)
        try:
            ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
            print('Connected to AP/Ethernet with IP: {}'.format(ip_address))
        except pexpect.exceptions.TIMEOUT:
            raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP/Ethernet')
        dut.expect('Starting Advanced OTA example', timeout=30)
        host_ip = get_host_ip4_by_dest_ip(ip_address)

        print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + truncated_bin_name))
        dut.write('https://' + host_ip + ':' + str(server_port) + '/' + truncated_bin_name)
        dut.expect('Image validation failed, image is corrupted', timeout=30)
        try:
            os.remove(binary_file)
        except OSError:
            pass
    finally:
        thread1.terminate()


@pytest.mark.esp32
@pytest.mark.ethernet_ota
def test_examples_protocol_advanced_https_ota_example_truncated_header(dut: Dut) -> None:
    """
    Working of OTA if headers of binary file are truncated is vaildated in this test case.
    Application should return with error message in this case.
    steps: |
      1. join AP/Ethernet
      2. Generate binary file with truncated headers
      3. Fetch OTA image over HTTPS
      4. Check working of code if headers are not sent completely
    """
    server_port = 8001
    # Original binary file generated after compilation
    bin_name = 'advanced_https_ota.bin'
    # Truncated binary file to be generated from original binary file
    truncated_bin_name = 'truncated_header.bin'
    # Size of truncated file to be generated. This value should be less than 288 bytes (Image header size)
    truncated_bin_size = 180
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, bin_name)
    with open(binary_file, 'rb+') as f:
        with open(os.path.join(dut.app.binary_path, truncated_bin_name), 'wb+') as fo:
            fo.write(f.read(truncated_bin_size))
    binary_file = os.path.join(dut.app.binary_path, truncated_bin_name)
    # Start server
    thread1 = multiprocessing.Process(target=start_https_server, args=(dut.app.binary_path, '0.0.0.0', server_port))
    thread1.daemon = True
    thread1.start()
    try:
        # start test
        dut.expect('Loaded app from partition at offset', timeout=30)
        try:
            ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
            print('Connected to AP/Ethernet with IP: {}'.format(ip_address))
        except pexpect.exceptions.TIMEOUT:
            raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP/Ethernet')
        host_ip = get_host_ip4_by_dest_ip(ip_address)

        dut.expect('Starting Advanced OTA example', timeout=30)
        print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + truncated_bin_name))
        dut.write('https://' + host_ip + ':' + str(server_port) + '/' + truncated_bin_name)
        dut.expect('advanced_https_ota_example: esp_https_ota_get_img_desc failed', timeout=30)
        try:
            os.remove(binary_file)
        except OSError:
            pass
    finally:
        thread1.terminate()


@pytest.mark.esp32
@pytest.mark.ethernet_ota
def test_examples_protocol_advanced_https_ota_example_random(dut: Dut) -> None:
    """
    Working of OTA if random data is added in binary file are validated in this test case.
    Magic byte verification should fail in this case.
    steps: |
      1. join AP/Ethernet
      2. Generate random binary image
      3. Fetch OTA image over HTTPS
      4. Check working of code for random binary file
    """
    server_port = 8001
    # Random binary file to be generated
    random_bin_name = 'random.bin'
    # Size of random binary file. 32000 is choosen, to reduce the time required to run the test-case
    random_bin_size = 32000
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, random_bin_name)
    with open(binary_file, 'wb+') as fo:
        # First byte of binary file is always set to zero. If first byte is generated randomly,
        # in some cases it may generate 0xE9 which will result in failure of testcase.
        fo.write(struct.pack('B', 0))
        for i in range(random_bin_size - 1):
            fo.write(struct.pack('B', random.randrange(0,255,1)))
    # Start server
    thread1 = multiprocessing.Process(target=start_https_server, args=(dut.app.binary_path, '0.0.0.0', server_port))
    thread1.daemon = True
    thread1.start()
    try:
        # start test
        dut.expect('Loaded app from partition at offset', timeout=30)
        try:
            ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
            print('Connected to AP/Ethernet with IP: {}'.format(ip_address))
        except pexpect.exceptions.TIMEOUT:
            raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP/Ethernet')
        host_ip = get_host_ip4_by_dest_ip(ip_address)

        dut.expect('Starting Advanced OTA example', timeout=30)
        print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + random_bin_name))
        dut.write('https://' + host_ip + ':' + str(server_port) + '/' + random_bin_name)
        dut.expect(r'esp_https_ota: Incorrect app descriptor magic', timeout=10)
        try:
            os.remove(binary_file)
        except OSError:
            pass
    finally:
        thread1.terminate()


@pytest.mark.esp32
@pytest.mark.ethernet_ota
def test_examples_protocol_advanced_https_ota_example_invalid_chip_id(dut: Dut) -> None:
    """
    Working of OTA if binary file have invalid chip id is validated in this test case.
    Chip id verification should fail in this case.
    steps: |
      1. join AP/Ethernet
      2. Generate binary image with invalid chip id
      3. Fetch OTA image over HTTPS
      4. Check working of code for random binary file
    """
    server_port = 8001
    bin_name = 'advanced_https_ota.bin'
    # Random binary file to be generated
    random_bin_name = 'random.bin'
    random_binary_file = os.path.join(dut.app.binary_path, random_bin_name)
    # Size of random binary file. 2000 is choosen, to reduce the time required to run the test-case
    random_bin_size = 2000

    binary_file = os.path.join(dut.app.binary_path, bin_name)
    with open(binary_file, 'rb+') as f:
        data = list(f.read(random_bin_size))
    # Changing Chip id
    data[13] = 0xfe
    with open(random_binary_file, 'wb+') as fo:
        fo.write(bytearray(data))
    # Start server
    thread1 = multiprocessing.Process(target=start_https_server, args=(dut.app.binary_path, '0.0.0.0', server_port))
    thread1.daemon = True
    thread1.start()
    try:
        # start test
        dut.expect('Loaded app from partition at offset', timeout=30)
        try:
            ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
            print('Connected to AP/Ethernet with IP: {}'.format(ip_address))
        except pexpect.exceptions.TIMEOUT:
            raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP/Ethernet')
        host_ip = get_host_ip4_by_dest_ip(ip_address)

        dut.expect('Starting Advanced OTA example', timeout=30)
        print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + random_bin_name))
        dut.write('https://' + host_ip + ':' + str(server_port) + '/' + random_bin_name)
        dut.expect(r'esp_https_ota: Mismatch chip id, expected 0, found \d', timeout=10)
        try:
            os.remove(random_binary_file)
        except OSError:
            pass
    finally:
        thread1.terminate()


@pytest.mark.esp32
@pytest.mark.ethernet_ota
def test_examples_protocol_advanced_https_ota_example_chunked(dut: Dut) -> None:
    """
    This is a positive test case, which downloads complete binary file multiple number of times.
    Number of iterations can be specified in variable iterations.
    steps: |
      1. join AP/Ethernet
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    # File to be downloaded. This file is generated after compilation
    bin_name = 'advanced_https_ota.bin'
    # Start server
    chunked_server = start_chunked_server(dut.app.binary_path, 8070)
    try:
        # start test
        dut.expect('Loaded app from partition at offset', timeout=30)
        try:
            ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
            print('Connected to AP/Ethernet with IP: {}'.format(ip_address))
        except pexpect.exceptions.TIMEOUT:
            raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP/Ethernet')
        host_ip = get_host_ip4_by_dest_ip(ip_address)

        dut.expect('Starting Advanced OTA example', timeout=30)
        print('writing to device: {}'.format('https://' + host_ip + ':8070/' + bin_name))
        dut.write('https://' + host_ip + ':8070/' + bin_name)
        dut.expect('upgrade successful. Rebooting ...', timeout=150)
        # after reboot
        dut.expect('Loaded app from partition at offset', timeout=30)
        dut.expect('OTA example app_main start', timeout=10)
    finally:
        chunked_server.kill()


@pytest.mark.esp32
@pytest.mark.ethernet_ota
def test_examples_protocol_advanced_https_ota_example_redirect_url(dut: Dut) -> None:
    """
    This is a positive test case, which starts a server and a redirection server.
    Redirection server redirects http_request to different port
    Number of iterations can be specified in variable iterations.
    steps: |
      1. join AP/Ethernet
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    server_port = 8001
    # Port to which the request should be redirected
    redirection_server_port = 8081
    redirection_server_port1 = 8082
    # File to be downloaded. This file is generated after compilation
    bin_name = 'advanced_https_ota.bin'
    # start test
    dut.expect('Loaded app from partition at offset', timeout=30)
    try:
        ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
        print('Connected to AP/Ethernet with IP: {}'.format(ip_address))
    except pexpect.exceptions.TIMEOUT:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP/Ethernet')
    dut.expect('Starting Advanced OTA example', timeout=30)

    # Start server
    host_ip = get_host_ip4_by_dest_ip(ip_address)
    thread1 = multiprocessing.Process(target=start_https_server, args=(dut.app.binary_path, host_ip, server_port))
    thread1.daemon = True
    thread2 = multiprocessing.Process(target=start_redirect_server, args=(dut.app.binary_path, host_ip, redirection_server_port, redirection_server_port1))
    thread2.daemon = True
    thread3 = multiprocessing.Process(target=start_redirect_server, args=(dut.app.binary_path, host_ip, redirection_server_port1, server_port))
    thread3.daemon = True
    thread1.start()
    thread2.start()
    thread3.start()
    time.sleep(1)

    try:
        print('writing to device: {}'.format('https://' + host_ip + ':' + str(redirection_server_port) + '/' + bin_name))
        dut.write('https://' + host_ip + ':' + str(redirection_server_port) + '/' + bin_name)
        dut.expect('upgrade successful. Rebooting ...', timeout=150)
        # after reboot
        dut.expect('Loaded app from partition at offset', timeout=30)
        dut.expect('OTA example app_main start', timeout=10)
    finally:
        thread1.terminate()
        thread2.terminate()
        thread3.terminate()


@pytest.mark.esp32
@pytest.mark.ethernet_flash_8m
@pytest.mark.parametrize('config', ['anti_rollback',], indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
def test_examples_protocol_advanced_https_ota_example_anti_rollback(dut: Dut) -> None:
    """
    Working of OTA when anti_rollback is enabled and security version of new image is less than current one.
    Application should return with error message in this case.
    steps: |
      1. join AP/Ethernet
      2. Generate binary file with lower security version
      3. Fetch OTA image over HTTPS
      4. Check working of anti_rollback feature
    """
    dut.serial.erase_flash()
    dut.serial.flash()
    server_port = 8001
    # Original binary file generated after compilation
    bin_name = 'advanced_https_ota.bin'
    # Modified firmware image to lower security version in its header. This is to enable negative test case
    anti_rollback_bin_name = 'advanced_https_ota_lower_sec_version.bin'
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, bin_name)
    file_size = os.path.getsize(binary_file)
    with open(binary_file, 'rb+') as f:
        with open(os.path.join(dut.app.binary_path, anti_rollback_bin_name), 'wb+') as fo:
            fo.write(f.read(file_size))
            # Change security_version to 0 for negative test case
            fo.seek(36)
            fo.write(b'\x00')
    binary_file = os.path.join(dut.app.binary_path, anti_rollback_bin_name)
    # Start server
    thread1 = multiprocessing.Process(target=start_https_server, args=(dut.app.binary_path, '0.0.0.0', server_port))
    thread1.daemon = True
    thread1.start()
    try:
        # start test
        # Positive Case
        dut.expect('Loaded app from partition at offset', timeout=30)
        try:
            ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
            print('Connected to AP/Ethernet with IP: {}'.format(ip_address))
        except pexpect.exceptions.TIMEOUT:
            raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP/Ethernet')
        host_ip = get_host_ip4_by_dest_ip(ip_address)

        dut.expect('Starting Advanced OTA example', timeout=30)
        # Use originally generated image with secure_version=1
        print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + bin_name))
        dut.write('https://' + host_ip + ':' + str(server_port) + '/' + bin_name)
        dut.expect('Loaded app from partition at offset', timeout=60)
        dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
        dut.expect(r'App is valid, rollback cancelled successfully', timeout=30)

        # Negative Case
        dut.expect('Starting Advanced OTA example', timeout=30)
        # Use modified image with secure_version=0
        print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + anti_rollback_bin_name))
        dut.write('https://' + host_ip + ':' + str(server_port) + '/' + anti_rollback_bin_name)
        dut.expect('New firmware security version is less than eFuse programmed, 0 < 1', timeout=30)
        try:
            os.remove(binary_file)
        except OSError:
            pass
    finally:
        thread1.terminate()


@pytest.mark.esp32
@pytest.mark.ethernet_ota
@pytest.mark.parametrize('config', ['partial_download',], indirect=True)
def test_examples_protocol_advanced_https_ota_example_partial_request(dut: Dut) -> None:
    """
    This is a positive test case, to test OTA workflow with Range HTTP header.
    steps: |
      1. join AP/Ethernet
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    server_port = 8001
    # Size of partial HTTP request
    request_size = int(dut.app.sdkconfig.get('EXAMPLE_HTTP_REQUEST_SIZE'))
    # File to be downloaded. This file is generated after compilation
    bin_name = 'advanced_https_ota.bin'
    binary_file = os.path.join(dut.app.binary_path, bin_name)
    bin_size = os.path.getsize(binary_file)
    http_requests = int((bin_size / request_size) - 1)
    assert http_requests > 1
    # Start server
    thread1 = multiprocessing.Process(target=start_https_server, args=(dut.app.binary_path, '0.0.0.0', server_port))
    thread1.daemon = True
    thread1.start()
    try:
        # start test
        dut.expect('Loaded app from partition at offset', timeout=30)
        try:
            ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
            print('Connected to AP/Ethernet with IP: {}'.format(ip_address))
        except pexpect.exceptions.TIMEOUT:
            raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
        host_ip = get_host_ip4_by_dest_ip(ip_address)

        dut.expect('Starting Advanced OTA example', timeout=30)
        print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + bin_name))
        dut.write('https://' + host_ip + ':' + str(server_port) + '/' + bin_name)
        for _ in range(http_requests):
            dut.expect('Connection closed', timeout=60)
        dut.expect('upgrade successful. Rebooting ...', timeout=60)
        # after reboot
        dut.expect('Loaded app from partition at offset', timeout=30)
        dut.expect('OTA example app_main start', timeout=20)
    finally:
        thread1.terminate()


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.wifi_high_traffic
@pytest.mark.parametrize('config', ['nimble',], indirect=True)
def test_examples_protocol_advanced_https_ota_example_nimble_gatts(dut: Dut) -> None:
    """
    Run an OTA image update while a BLE GATT Server is running in background. This GATT server will be using NimBLE Host stack.
    steps: |
      1. join AP/Ethernet
      2. Run BLE advertise and then GATT server.
      3. Fetch OTA image over HTTPS
      4. Reboot with the new OTA image
    """
    server_port = 8001
    # File to be downloaded. This file is generated after compilation
    bin_name = 'advanced_https_ota.bin'
    # Start server
    thread1 = multiprocessing.Process(target=start_https_server, args=(dut.app.binary_path, '0.0.0.0', server_port))
    thread1.daemon = True
    thread1.start()
    try:
        # start test
        dut.expect('Loaded app from partition at offset', timeout=30)
        # Parse IP address of STA
        if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
            env_name = 'wifi_high_traffic'
            dut.expect('Please input ssid password:')
            ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
            ap_password = get_env_config_variable(env_name, 'ap_password')
            dut.write(f'{ap_ssid} {ap_password}')
        try:
            ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
            print('Connected to AP with IP: {}'.format(ip_address))
        except pexpect.exceptions.TIMEOUT:
            raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
        host_ip = get_host_ip4_by_dest_ip(ip_address)

        dut.expect('Starting Advanced OTA example', timeout=30)
        print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + bin_name))
        print('Started GAP advertising.')

        dut.write('https://' + host_ip + ':' + str(server_port) + '/' + bin_name)
        dut.expect('upgrade successful. Rebooting ...', timeout=150)
        # after reboot
        dut.expect('Loaded app from partition at offset', timeout=30)
        dut.expect('OTA example app_main start', timeout=10)
    finally:
        thread1.terminate()


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.wifi_high_traffic
@pytest.mark.parametrize('config', ['bluedroid',], indirect=True)
def test_examples_protocol_advanced_https_ota_example_bluedroid_gatts(dut: Dut) -> None:
    """
    Run an OTA image update while a BLE GATT Server is running in background. This GATT server will be using Bluedroid Host stack.
    steps: |
      1. join AP/Ethernet
      2. Run BLE advertise and then GATT server.
      3. Fetch OTA image over HTTPS
      4. Reboot with the new OTA image
    """
    server_port = 8001
    # File to be downloaded. This file is generated after compilation
    bin_name = 'advanced_https_ota.bin'
    # Start server
    thread1 = multiprocessing.Process(target=start_https_server, args=(dut.app.binary_path, '0.0.0.0', server_port))
    thread1.daemon = True
    thread1.start()
    try:
        # start test
        dut.expect('Loaded app from partition at offset', timeout=30)
        # Parse IP address of STA
        if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
            env_name = 'wifi_high_traffic'
            dut.expect('Please input ssid password:')
            ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
            ap_password = get_env_config_variable(env_name, 'ap_password')
            dut.write(f'{ap_ssid} {ap_password}')
        try:
            ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
            print('Connected to AP with IP: {}'.format(ip_address))
        except pexpect.exceptions.TIMEOUT:
            raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
        host_ip = get_host_ip4_by_dest_ip(ip_address)

        dut.expect('Started advertising.', timeout=30)
        print('Started GAP advertising.')

        time.sleep(1)
        print('writing to device: {}'.format('https://' + host_ip + ':' + str(server_port) + '/' + bin_name))
        dut.write('https://' + host_ip + ':' + str(server_port) + '/' + bin_name)
        dut.expect('upgrade successful. Rebooting ...', timeout=150)
        # after reboot
        dut.expect('Loaded app from partition at offset', timeout=30)
        dut.expect('OTA example app_main start', timeout=10)
    finally:
        thread1.terminate()


@pytest.mark.esp32
@pytest.mark.ethernet_ota
def test_examples_protocol_advanced_https_ota_example_openssl_aligned_bin(dut: Dut) -> None:
    """
    This is a test case for esp_http_client_read with binary size multiple of 289 bytes
    steps: |
      1. join AP/Ethernet
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    # Original binary file generated after compilation
    bin_name = 'advanced_https_ota.bin'
    # Binary file aligned to DEFAULT_OTA_BUF_SIZE(289 bytes) boundary
    aligned_bin_name = 'aligned.bin'
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, bin_name)
    # Original binary size
    bin_size = os.path.getsize(binary_file)
    # Dummy data required to align binary size to 289 bytes boundary
    dummy_data_size = 289 - (bin_size % 289)
    with open(binary_file, 'rb+') as f:
        with open(os.path.join(dut.app.binary_path, aligned_bin_name), 'wb+') as fo:
            fo.write(f.read(bin_size))
            for _ in range(dummy_data_size):
                fo.write(struct.pack('B', random.randrange(0,255,1)))
    # Start server
    chunked_server = start_chunked_server(dut.app.binary_path, 8070)
    try:
        # start test
        dut.expect('Loaded app from partition at offset', timeout=30)
        try:
            ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
            print('Connected to AP/Ethernet with IP: {}'.format(ip_address))
        except pexpect.exceptions.TIMEOUT:
            raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP/Ethernet')
        host_ip = get_host_ip4_by_dest_ip(ip_address)

        dut.expect('Starting Advanced OTA example', timeout=30)
        print('writing to device: {}'.format('https://' + host_ip + ':8070/' + aligned_bin_name))
        dut.write('https://' + host_ip + ':8070/' + aligned_bin_name)
        dut.expect('upgrade successful. Rebooting ...', timeout=150)
        # after reboot
        dut.expect('Loaded app from partition at offset', timeout=30)
        dut.expect('OTA example app_main start', timeout=10)
        try:
            os.remove(aligned_bin_name)
        except OSError:
            pass
    finally:
        chunked_server.kill()
