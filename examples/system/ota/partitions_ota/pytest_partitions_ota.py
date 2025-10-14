# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import http.server
import multiprocessing
import os
import ssl
import sys
from typing import Any
from typing import Optional

import pexpect
import pytest
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

try:
    from common_test_methods import get_env_config_variable
    from common_test_methods import get_host_ip4_by_dest_ip
except ModuleNotFoundError:
    idf_path = os.environ['IDF_PATH']
    sys.path.append(idf_path + '/tools/ci')
    sys.path.insert(0, idf_path + '/tools/ci/python_packages')
    from common_test_methods import get_env_config_variable
    from common_test_methods import get_host_ip4_by_dest_ip

server_cert = (
    '-----BEGIN CERTIFICATE-----\n'
    'MIIDWDCCAkACCQCbF4+gVh/MLjANBgkqhkiG9w0BAQsFADBuMQswCQYDVQQGEwJJ\n'
    'TjELMAkGA1UECAwCTUgxDDAKBgNVBAcMA1BVTjEMMAoGA1UECgwDRVNQMQwwCgYD\n'
    'VQQLDANFU1AxDDAKBgNVBAMMA0VTUDEaMBgGCSqGSIb3DQEJARYLZXNwQGVzcC5j\n'
    'b20wHhcNMjEwNzEyMTIzNjI3WhcNNDEwNzA3MTIzNjI3WjBuMQswCQYDVQQGEwJJ\n'
    'TjELMAkGA1UECAwCTUgxDDAKBgNVBAcMA1BVTjEMMAoGA1UECgwDRVNQMQwwCgYD\n'
    'VQQLDANFU1AxDDAKBgNVBAMMA0VTUDEaMBgGCSqGSIb3DQEJARYLZXNwQGVzcC5j\n'
    'b20wggEiMA0GCSqGSIb3DQEBAQUAA4IBDwAwggEKAoIBAQDhxF/y7bygndxPwiWL\n'
    'SwS9LY3uBMaJgup0ufNKVhx+FhGQOu44SghuJAaH3KkPUnt6SOM8jC97/yQuc32W\n'
    'ukI7eBZoA12kargSnzdv5m5rZZpd+NznSSpoDArOAONKVlzr25A1+aZbix2mKRbQ\n'
    'S5w9o1N2BriQuSzd8gL0Y0zEk3VkOWXEL+0yFUT144HnErnD+xnJtHe11yPO2fEz\n'
    'YaGiilh0ddL26PXTugXMZN/8fRVHP50P2OG0SvFpC7vghlLp4VFM1/r3UJnvL6Oz\n'
    '3ALc6dhxZEKQucqlpj8l1UegszQToopemtIj0qXTHw2+uUnkUyWIPjPC+wdOAoap\n'
    'rFTRAgMBAAEwDQYJKoZIhvcNAQELBQADggEBAItw24y565k3C/zENZlxyzto44ud\n'
    'IYPQXN8Fa2pBlLe1zlSIyuaA/rWQ+i1daS8nPotkCbWZyf5N8DYaTE4B0OfvoUPk\n'
    'B5uGDmbuk6akvlB5BGiYLfQjWHRsK9/4xjtIqN1H58yf3QNROuKsPAeywWS3Fn32\n'
    '3//OpbWaClQePx6udRYMqAitKR+QxL7/BKZQsX+UyShuq8hjphvXvk0BW8ONzuw9\n'
    'RcoORxM0FzySYjeQvm4LhzC/P3ZBhEq0xs55aL2a76SJhq5hJy7T/Xz6NFByvlrN\n'
    'lFJJey33KFrAf5vnV9qcyWFIo7PYy2VsaaEjFeefr7q3sTFSMlJeadexW2Y=\n'
    '-----END CERTIFICATE-----\n'
)

server_key = (
    '-----BEGIN PRIVATE KEY-----\n'
    'MIIEvQIBADANBgkqhkiG9w0BAQEFAASCBKcwggSjAgEAAoIBAQDhxF/y7bygndxP\n'
    'wiWLSwS9LY3uBMaJgup0ufNKVhx+FhGQOu44SghuJAaH3KkPUnt6SOM8jC97/yQu\n'
    'c32WukI7eBZoA12kargSnzdv5m5rZZpd+NznSSpoDArOAONKVlzr25A1+aZbix2m\n'
    'KRbQS5w9o1N2BriQuSzd8gL0Y0zEk3VkOWXEL+0yFUT144HnErnD+xnJtHe11yPO\n'
    '2fEzYaGiilh0ddL26PXTugXMZN/8fRVHP50P2OG0SvFpC7vghlLp4VFM1/r3UJnv\n'
    'L6Oz3ALc6dhxZEKQucqlpj8l1UegszQToopemtIj0qXTHw2+uUnkUyWIPjPC+wdO\n'
    'AoaprFTRAgMBAAECggEAE0HCxV/N1Q1h+1OeDDGL5+74yjKSFKyb/vTVcaPCrmaH\n'
    'fPvp0ddOvMZJ4FDMAsiQS6/n4gQ7EKKEnYmwTqj4eUYW8yxGUn3f0YbPHbZT+Mkj\n'
    'z5woi3nMKi/MxCGDQZX4Ow3xUQlITUqibsfWcFHis8c4mTqdh4qj7xJzehD2PVYF\n'
    'gNHZsvVj6MltjBDAVwV1IlGoHjuElm6vuzkfX7phxcA1B4ZqdYY17yCXUnvui46z\n'
    'Xn2kUTOOUCEgfgvGa9E+l4OtdXi5IxjaSraU+dlg2KsE4TpCuN2MEVkeR5Ms3Y7Q\n'
    'jgJl8vlNFJDQpbFukLcYwG7rO5N5dQ6WWfVia/5XgQKBgQD74at/bXAPrh9NxPmz\n'
    'i1oqCHMDoM9sz8xIMZLF9YVu3Jf8ux4xVpRSnNy5RU1gl7ZXbpdgeIQ4v04zy5aw\n'
    '8T4tu9K3XnR3UXOy25AK0q+cnnxZg3kFQm+PhtOCKEFjPHrgo2MUfnj+EDddod7N\n'
    'JQr9q5rEFbqHupFPpWlqCa3QmQKBgQDldWUGokNaEpmgHDMnHxiibXV5LQhzf8Rq\n'
    'gJIQXb7R9EsTSXEvsDyqTBb7PHp2Ko7rZ5YQfyf8OogGGjGElnPoU/a+Jij1gVFv\n'
    'kZ064uXAAISBkwHdcuobqc5EbG3ceyH46F+FBFhqM8KcbxJxx08objmh58+83InN\n'
    'P9Qr25Xw+QKBgEGXMHuMWgQbSZeM1aFFhoMvlBO7yogBTKb4Ecpu9wI5e3Kan3Al\n'
    'pZYltuyf+VhP6XG3IMBEYdoNJyYhu+nzyEdMg8CwXg+8LC7FMis/Ve+o7aS5scgG\n'
    '1to/N9DK/swCsdTRdzmc/ZDbVC+TuVsebFBGYZTyO5KgqLpezqaIQrTxAoGALFCU\n'
    '10glO9MVyl9H3clap5v+MQ3qcOv/EhaMnw6L2N6WVT481tnxjW4ujgzrFcE4YuxZ\n'
    'hgwYu9TOCmeqopGwBvGYWLbj+C4mfSahOAs0FfXDoYazuIIGBpuv03UhbpB1Si4O\n'
    'rJDfRnuCnVWyOTkl54gKJ2OusinhjztBjcrV1XkCgYEA3qNi4uBsPdyz9BZGb/3G\n'
    'rOMSw0CaT4pEMTLZqURmDP/0hxvTk1polP7O/FYwxVuJnBb6mzDa0xpLFPTpIAnJ\n'
    'YXB8xpXU69QVh+EBbemdJWOd+zp5UCfXvb2shAeG3Tn/Dz4cBBMEUutbzP+or0nG\n'
    'vSXnRLaxQhooWm+IuX9SuBQ=\n'
    '-----END PRIVATE KEY-----\n'
)


@pytest.mark.wifi_high_traffic
@pytest.mark.parametrize(
    'config',
    ['on_update_no_sb_ecdsa', 'on_update_no_sb_rsa', 'virt_sb_v2_and_fe', 'virt_sb_v2_and_fe_2'],
    indirect=True,
)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
@pytest.mark.timeout(2400)
@idf_parametrize('target', ['esp32', 'esp32c3', 'esp32s3'], indirect=['target'])
def test_examples_partitions_ota(dut: Dut) -> None:
    print(' - Erase flash')
    dut.serial.erase_flash()
    print(' - Flash bootloader')
    dut.serial.bootloader_flash()
    print(' - Start app (flash partition_table and app)')
    dut.serial.write_flash_no_enc()
    update_partitions(dut, 'wifi_high_traffic')


@pytest.mark.flash_encryption_wifi_high_traffic
@pytest.mark.nightly_run
@pytest.mark.timeout(2400)
@pytest.mark.parametrize('config', ['flash_enc_wifi', 'flash_enc_wifi_2'], indirect=True)
@pytest.mark.parametrize('skip_autoflash', ['y'], indirect=True)
@idf_parametrize('target', ['esp32', 'esp32c3'], indirect=['target'])
def test_examples_partitions_ota_with_flash_encryption_wifi(dut: Dut) -> None:
    dut.serial.erase_flash()
    dut.serial.flash()
    update_partitions(dut, 'flash_encryption_wifi_high_traffic')


def update_partitions(dut: Dut, env_name: Optional[str]) -> None:
    port = 8000
    thread1 = multiprocessing.Process(target=start_https_server, args=(dut.app.binary_path, '0.0.0.0', port))
    thread1.daemon = True
    thread1.start()
    try:
        update(dut, port, 'partitions_ota.bin', env_name)
        update(dut, port, 'bootloader/bootloader.bin', env_name)
        update(dut, port, 'partition_table/partition-table.bin', env_name)
        update(dut, port, 'storage.bin', env_name)
    finally:
        thread1.terminate()


def update(dut: Dut, port: int, path_to_image: str, env_name: Optional[str]) -> None:
    dut.expect('OTA example app_main start', timeout=90)
    host_ip = setting_connection(dut, env_name)
    dut.expect('Starting OTA example task', timeout=30)
    url = f'https://{host_ip}:{port}/{path_to_image}'
    print(f'Writing to device: {url}')
    dut.write(url)
    dut.expect('OTA Succeed, Rebooting...', timeout=90)


def setting_connection(dut: Dut, env_name: Optional[str]) -> Any:
    if env_name is not None and dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    try:
        ip_address = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()
        print(f'Connected to AP/Ethernet with IP: {ip_address}')
    except pexpect.exceptions.TIMEOUT:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP/Ethernet')
    return get_host_ip4_by_dest_ip(ip_address)


def start_https_server(
    ota_image_dir: str,
    server_ip: str,
    server_port: int,
    server_file: Optional[str] = None,
    key_file: Optional[str] = None,
) -> None:
    os.chdir(ota_image_dir)

    if server_file is None:
        server_file = os.path.join(ota_image_dir, 'server_cert.pem')
        cert_file_handle = open(server_file, 'w+', encoding='utf-8')
        cert_file_handle.write(server_cert)
        cert_file_handle.close()

    if key_file is None:
        key_file = os.path.join(ota_image_dir, 'server_key.pem')
        key_file_handle = open('server_key.pem', 'w+', encoding='utf-8')
        key_file_handle.write(server_key)
        key_file_handle.close()

    httpd = http.server.HTTPServer((server_ip, server_port), http.server.SimpleHTTPRequestHandler)

    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    ssl_context.load_cert_chain(certfile=server_file, keyfile=key_file)

    httpd.socket = ssl_context.wrap_socket(httpd.socket, server_side=True)
    httpd.serve_forever()


if __name__ == '__main__':
    if sys.argv[2:]:  # if two or more arguments provided:
        # Usage: pytest_partition_ota.py <image_dir> <server_port> [cert_dir]
        image_dir = os.path.dirname(os.path.realpath(__file__))
        bin_dir = os.path.join(image_dir, sys.argv[1])
        server_port = int(sys.argv[2])
        server_file = None
        key_file = None
        if sys.argv[3:]:  # [cert_dir] - optional argument
            cert_dir = os.path.join(image_dir, sys.argv[3])
            server_file = os.path.join(cert_dir, 'ca_cert.pem')
            key_file = os.path.join(cert_dir, 'ca_key.pem')
        print(f'Starting HTTPS server at "https://0.0.0.0:{server_port}"')
        start_https_server(bin_dir, '0.0.0.0', server_port, server_file=server_file, key_file=key_file)
