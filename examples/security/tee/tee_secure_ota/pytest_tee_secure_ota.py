# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import http.server
import multiprocessing
import os
import socket
import ssl
import time
from typing import Callable

import pexpect
import pytest
from common_test_methods import get_env_config_variable
from common_test_methods import get_host_ip4_by_dest_ip
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize
from RangeHTTPServer import RangeRequestHandler

server_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'test_certs/server_cert.pem')
key_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'test_certs/server_key.pem')


def https_request_handler() -> Callable[..., http.server.BaseHTTPRequestHandler]:
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

    httpd.socket = ssl.wrap_socket(httpd.socket, keyfile=key_file, certfile=server_file, server_side=True)
    httpd.serve_forever()


@pytest.mark.wifi_high_traffic
@idf_parametrize('target', ['esp32c6'], indirect=['target'])
def test_examples_tee_secure_ota_example(dut: Dut) -> None:
    """
    This is a positive test case, which downloads complete binary file multiple number of times.
    Number of iterations can be specified in variable iterations.
    steps:
      1. join AP
      2. Fetch TEE OTA image over HTTPS
      3. Reboot with the new TEE OTA image
    """
    # Number of iterations to validate OTA
    iterations = 4
    server_port = 8001
    tee_bin = 'esp_tee/esp_tee.bin'
    user_bin = 'tee_secure_ota.bin'

    # Start server
    thread1 = multiprocessing.Process(target=start_https_server, args=(dut.app.binary_path, '0.0.0.0', server_port))
    thread1.daemon = True
    thread1.start()
    time.sleep(1)

    try:
        # start test
        for i in range(iterations):
            # Boot up sequence checks
            dut.expect('Loaded TEE app from partition at offset', timeout=30)
            dut.expect('Loaded app from partition at offset', timeout=30)

            # Starting the test
            dut.expect('OTA with TEE enabled', timeout=30)
            time.sleep(1)

            # Connecting to Wi-Fi
            if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
                dut.expect('Please input ssid password:')
                env_name = 'wifi_high_traffic'
                ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
                ap_password = get_env_config_variable(env_name, 'ap_password')
                dut.write(f'{ap_ssid} {ap_password}')
            try:
                ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
                print('Connected to AP/Ethernet with IP: {}'.format(ip_address))
            except pexpect.exceptions.TIMEOUT:
                raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')

            host_ip = get_host_ip4_by_dest_ip(ip_address)
            dut.expect('Returned from app_main', timeout=30)

            # User OTA for last iteration
            if i == (iterations - 1):
                dut.write(f'user_ota https://{host_ip}:{str(server_port)}/{user_bin}')
                dut.expect('OTA Succeed, Rebooting', timeout=150)
            else:
                dut.write(f'tee_ota https://{host_ip}:{str(server_port)}/{tee_bin}')
                dut.expect('esp_tee_ota_end succeeded', timeout=60)
                dut.expect('Prepare to restart system!', timeout=60)
    finally:
        thread1.terminate()
