# SPDX-FileCopyrightText: 2022-2024 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import http.server
import multiprocessing
import os
import ssl
import subprocess
import sys
from typing import Tuple

import pexpect
import pytest
from pytest_embedded import Dut

try:
    from common_test_methods import get_env_config_variable, get_host_ip4_by_dest_ip
except ModuleNotFoundError:
    idf_path = os.environ['IDF_PATH']
    sys.path.insert(0, idf_path + '/tools/ci/python_packages')
    from common_test_methods import get_env_config_variable, get_host_ip4_by_dest_ip

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


def start_https_server(ota_image_dir: str, server_ip: str, server_port: int, server_file: str = None, key_file: str = None) -> None:
    os.chdir(ota_image_dir)

    if server_file is None:
        server_file = os.path.join(ota_image_dir, 'server_cert.pem')
        cert_file_handle = open(server_file, 'w+')
        cert_file_handle.write(server_cert)
        cert_file_handle.close()

    if key_file is None:
        key_file = os.path.join(ota_image_dir, 'server_key.pem')
        key_file_handle = open('server_key.pem', 'w+')
        key_file_handle.write(server_key)
        key_file_handle.close()

    httpd = http.server.HTTPServer((server_ip, server_port), http.server.SimpleHTTPRequestHandler)

    httpd.socket = ssl.wrap_socket(httpd.socket,
                                   keyfile=key_file,
                                   certfile=server_file, server_side=True)
    httpd.serve_forever()


def start_tls1_3_server(ota_image_dir: str, server_port: int) -> subprocess.Popen:
    os.chdir(ota_image_dir)
    server_file = os.path.join(ota_image_dir, 'server_cert.pem')
    cert_file_handle = open(server_file, 'w+')
    cert_file_handle.write(server_cert)
    cert_file_handle.close()

    key_file = os.path.join(ota_image_dir, 'server_key.pem')
    key_file_handle = open('server_key.pem', 'w+')
    key_file_handle.write(server_key)
    key_file_handle.close()

    chunked_server = subprocess.Popen(['openssl', 's_server', '-tls1_3', '-WWW', '-key', key_file, '-cert', server_file, '-port', str(server_port)])
    return chunked_server


def check_sha256(sha256_expected: str, sha256_reported: str) -> None:
    print('sha256_expected: %s' % (sha256_expected))
    print('sha256_reported: %s' % (sha256_reported))
    if sha256_expected not in sha256_reported:
        raise ValueError('SHA256 mismatch')
    else:
        print('SHA256 expected and reported are the same')


def calc_all_sha256(dut: Dut) -> Tuple[str, str]:
    bootloader_path = os.path.join(dut.app.binary_path, 'bootloader', 'bootloader.bin')
    sha256_bootloader = dut.app.get_sha256(bootloader_path)

    app_path = os.path.join(dut.app.binary_path, 'simple_ota.bin')
    sha256_app = dut.app.get_sha256(app_path)

    return str(sha256_bootloader), str(sha256_app)


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.esp32s3
@pytest.mark.wifi_high_traffic
def test_examples_protocol_simple_ota_example(dut: Dut) -> None:
    """
    steps: |
      1. join AP/Ethernet
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    sha256_bootloader, sha256_app = calc_all_sha256(dut)
    # Start server
    thread1 = multiprocessing.Process(target=start_https_server, args=(dut.app.binary_path, '0.0.0.0', 8000))
    thread1.daemon = True
    thread1.start()
    try:
        # start test
        dut.expect('Loaded app from partition at offset 0x10000', timeout=30)
        check_sha256(sha256_bootloader, str(dut.expect(r'SHA-256 for bootloader:\s+([a-f0-9]){64}')[0]))
        check_sha256(sha256_app, str(dut.expect(r'SHA-256 for current firmware:\s+([a-f0-9]){64}')[0]))
        # Parse IP address of STA
        if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
            env_name = 'wifi_high_traffic'
            dut.expect('Please input ssid password:')
            ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
            ap_password = get_env_config_variable(env_name, 'ap_password')
            dut.write(f'{ap_ssid} {ap_password}')
        try:
            ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
            print('Connected to AP/Ethernet with IP: {}'.format(ip_address))
        except pexpect.exceptions.TIMEOUT:
            raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP/Ethernet')
        host_ip = get_host_ip4_by_dest_ip(ip_address)

        dut.expect('Starting OTA example task', timeout=30)
        print('writing to device: {}'.format('https://' + host_ip + ':8000/simple_ota.bin'))
        dut.write('https://' + host_ip + ':8000/simple_ota.bin')
        dut.expect('OTA Succeed, Rebooting...', timeout=60)
        # after reboot
        dut.expect('Loaded app from partition at offset 0x110000', timeout=30)
        dut.expect('OTA example app_main start', timeout=10)
    finally:
        thread1.terminate()


@pytest.mark.esp32
@pytest.mark.ethernet_ota
@pytest.mark.parametrize('config', ['spiram',], indirect=True)
def test_examples_protocol_simple_ota_example_ethernet_with_spiram_config(dut: Dut) -> None:
    """
    steps: |
      1. join AP/Ethernet
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    # Start server
    thread1 = multiprocessing.Process(target=start_https_server, args=(dut.app.binary_path, '0.0.0.0', 8000))
    thread1.daemon = True
    thread1.start()
    try:
        # start test
        dut.expect('Loaded app from partition at offset 0x10000', timeout=30)
        try:
            ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
            print('Connected to AP/Ethernet with IP: {}'.format(ip_address))
        except pexpect.exceptions.TIMEOUT:
            raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP/Ethernet')
        host_ip = get_host_ip4_by_dest_ip(ip_address)

        dut.expect('Starting OTA example task', timeout=30)
        print('writing to device: {}'.format('https://' + host_ip + ':8000/simple_ota.bin'))
        dut.write('https://' + host_ip + ':8000/simple_ota.bin')
        dut.expect('OTA Succeed, Rebooting...', timeout=60)
        # after reboot
        dut.expect('Loaded app from partition at offset 0x110000', timeout=30)
        dut.expect('OTA example app_main start', timeout=10)
    finally:
        thread1.terminate()


@pytest.mark.esp32
@pytest.mark.esp32c3
@pytest.mark.flash_encryption_wifi_high_traffic
@pytest.mark.nightly_run
@pytest.mark.parametrize('config', ['flash_enc_wifi',], indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
def test_examples_protocol_simple_ota_example_with_flash_encryption_wifi(dut: Dut) -> None:
    """
    steps: |
      1. join AP/Ethernet
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    # start test
    # Erase flash
    dut.serial.erase_flash()
    dut.serial.flash()
    # Start server
    thread1 = multiprocessing.Process(target=start_https_server, args=(dut.app.binary_path, '0.0.0.0', 8000))
    thread1.daemon = True
    thread1.start()
    try:
        dut.expect('Loaded app from partition at offset 0x20000', timeout=30)
        dut.expect('Flash encryption mode is DEVELOPMENT', timeout=10)
        # Parse IP address of STA
        if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
            env_name = 'flash_encryption_wifi_high_traffic'
            dut.expect('Please input ssid password:')
            ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
            ap_password = get_env_config_variable(env_name, 'ap_password')
            dut.write(f'{ap_ssid} {ap_password}')
        try:
            ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
            print('Connected to AP/Ethernet with IP: {}'.format(ip_address))
        except pexpect.exceptions.TIMEOUT:
            raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP/Ethernet')
        host_ip = get_host_ip4_by_dest_ip(ip_address)

        dut.expect('Starting OTA example task', timeout=30)
        print('writing to device: {}'.format('https://' + host_ip + ':8000/simple_ota.bin'))
        dut.write('https://' + host_ip + ':8000/simple_ota.bin')
        dut.expect('OTA Succeed, Rebooting...', timeout=60)
        # after reboot
        dut.expect('Loaded app from partition at offset 0x120000', timeout=30)
        dut.expect('Flash encryption mode is DEVELOPMENT', timeout=10)
        dut.expect('OTA example app_main start', timeout=10)
    finally:
        thread1.terminate()


@pytest.mark.esp32
@pytest.mark.ethernet_ota
@pytest.mark.parametrize('config', ['on_update_no_sb_ecdsa',], indirect=True)
def test_examples_protocol_simple_ota_example_with_verify_app_signature_on_update_no_secure_boot_ecdsa(dut: Dut) -> None:
    """
    steps: |
      1. join AP/Ethernet
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    sha256_bootloader, sha256_app = calc_all_sha256(dut)
    # Start server
    thread1 = multiprocessing.Process(target=start_https_server, args=(dut.app.binary_path, '0.0.0.0', 8000))
    thread1.daemon = True
    thread1.start()
    try:
        # start test
        dut.expect('Loaded app from partition at offset 0x20000', timeout=30)
        check_sha256(sha256_bootloader, str(dut.expect(r'SHA-256 for bootloader:\s+([a-f0-9]){64}')[0]))
        check_sha256(sha256_app, str(dut.expect(r'SHA-256 for current firmware:\s+([a-f0-9]){64}')[0]))
        try:
            ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
            print('Connected to AP/Ethernet with IP: {}'.format(ip_address))
        except pexpect.exceptions.TIMEOUT:
            raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP/Ethernet')
        host_ip = get_host_ip4_by_dest_ip(ip_address)

        dut.expect('Starting OTA example task', timeout=30)
        print('writing to device: {}'.format('https://' + host_ip + ':8000/simple_ota.bin'))
        dut.write('https://' + host_ip + ':8000/simple_ota.bin')
        dut.expect('Writing to partition subtype 16 at offset 0x120000', timeout=20)
        dut.expect('Verifying image signature...', timeout=60)
        dut.expect('OTA Succeed, Rebooting...', timeout=60)
        # after reboot
        dut.expect('Loaded app from partition at offset 0x120000', timeout=20)
        dut.expect('OTA example app_main start', timeout=10)
    finally:
        thread1.terminate()


@pytest.mark.esp32
@pytest.mark.ethernet_ota
@pytest.mark.parametrize('config', ['on_update_no_sb_rsa',], indirect=True)
def test_examples_protocol_simple_ota_example_with_verify_app_signature_on_update_no_secure_boot_rsa(dut: Dut) -> None:
    """
    steps: |
      1. join AP/Ethernet
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    sha256_bootloader, sha256_app = calc_all_sha256(dut)
    # Start server
    thread1 = multiprocessing.Process(target=start_https_server, args=(dut.app.binary_path, '0.0.0.0', 8000))
    thread1.daemon = True
    thread1.start()
    try:
        # start test
        dut.expect('Loaded app from partition at offset 0x20000', timeout=30)
        check_sha256(sha256_bootloader, str(dut.expect(r'SHA-256 for bootloader:\s+([a-f0-9]){64}')[0]))
        check_sha256(sha256_app, str(dut.expect(r'SHA-256 for current firmware:\s+([a-f0-9]){64}')[0]))
        try:
            ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
            print('Connected to AP/Ethernet with IP: {}'.format(ip_address))
        except pexpect.exceptions.TIMEOUT:
            raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP/Ethernet')
        host_ip = get_host_ip4_by_dest_ip(ip_address)

        dut.expect('Starting OTA example task', timeout=30)
        print('writing to device: {}'.format('https://' + host_ip + ':8000/simple_ota.bin'))
        dut.write('https://' + host_ip + ':8000/simple_ota.bin')
        dut.expect('Writing to partition subtype 16 at offset 0x120000', timeout=20)
        dut.expect('Verifying image signature...', timeout=60)
        dut.expect('#0 app key digest == #0 trusted key digest', timeout=10)
        dut.expect('Verifying with RSA-PSS...', timeout=10)
        dut.expect('Signature verified successfully!', timeout=10)
        dut.expect('OTA Succeed, Rebooting...', timeout=60)
        # after reboot
        dut.expect('Loaded app from partition at offset 0x120000', timeout=20)
        dut.expect('OTA example app_main start', timeout=10)
    finally:
        thread1.terminate()


@pytest.mark.esp32
@pytest.mark.ethernet_ota
@pytest.mark.parametrize('config', ['tls1_3',], indirect=True)
def test_examples_protocol_simple_ota_example_tls1_3(dut: Dut) -> None:
    """
    steps: |
      1. join AP/Ethernet
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    sha256_bootloader, sha256_app = calc_all_sha256(dut)
    # Start server
    tls1_3_server = start_tls1_3_server(dut.app.binary_path, 8000)
    try:
        # start test
        dut.expect('Loaded app from partition at offset 0x10000', timeout=30)
        check_sha256(sha256_bootloader, str(dut.expect(r'SHA-256 for bootloader:\s+([a-f0-9]){64}')[0]))
        check_sha256(sha256_app, str(dut.expect(r'SHA-256 for current firmware:\s+([a-f0-9]){64}')[0]))
        # Parse IP address of STA
        if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
            env_name = 'wifi_high_traffic'
            dut.expect('Please input ssid password:')
            ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
            ap_password = get_env_config_variable(env_name, 'ap_password')
            dut.write(f'{ap_ssid} {ap_password}')
        try:
            ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
            print('Connected to AP/Ethernet with IP: {}'.format(ip_address))
        except pexpect.exceptions.TIMEOUT:
            raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP/Ethernet')
        host_ip = get_host_ip4_by_dest_ip(ip_address)

        dut.expect('Starting OTA example task', timeout=30)
        print('writing to device: {}'.format('https://' + host_ip + ':8000/simple_ota.bin'))
        dut.write('https://' + host_ip + ':8000/simple_ota.bin')
        dut.expect('OTA Succeed, Rebooting...', timeout=120)
        # after reboot
        dut.expect('Loaded app from partition at offset 0x110000', timeout=30)
        dut.expect('OTA example app_main start', timeout=10)
    finally:
        tls1_3_server.kill()


if __name__ == '__main__':
    if sys.argv[2:]:    # if two or more arguments provided:
        # Usage: pytest_simple_ota.py <image_dir> <server_port> [cert_di>]
        this_dir = os.path.dirname(os.path.realpath(__file__))
        bin_dir = os.path.join(this_dir, sys.argv[1])
        port = int(sys.argv[2])
        cert_dir = bin_dir if not sys.argv[3:] else os.path.join(this_dir, sys.argv[3])  # optional argument
        print('Starting HTTPS server at "https://:{}"'.format(port))
        start_https_server(bin_dir, '', port,
                           server_file=os.path.join(cert_dir, 'ca_cert.pem'),
                           key_file=os.path.join(cert_dir, 'ca_key.pem'))
