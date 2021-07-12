import http.server
import os
import re
import socket
import ssl
import sys
from threading import Thread

import ttfw_idf
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


def start_https_server(ota_image_dir, server_ip, server_port, server_file=None, key_file=None):
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


def check_sha256(sha256_expected, sha256_reported):
    Utility.console_log('sha256_expected: %s' % (sha256_expected))
    Utility.console_log('sha256_reported: %s' % (sha256_reported))
    if sha256_reported not in sha256_expected:
        raise ValueError('SHA256 mismatch')
    else:
        Utility.console_log('SHA256 expected and reported are the same')


def calc_all_sha256(dut):
    bootloader_path = os.path.join(dut.app.binary_path, 'bootloader', 'bootloader.bin')
    output = dut.image_info(bootloader_path)
    sha256_bootloader = re.search(r'Validation Hash:\s+([a-f0-9]+)', output).group(1)
    Utility.console_log('bootloader SHA256: %s' % sha256_bootloader)

    app_path = os.path.join(dut.app.binary_path, 'simple_ota.bin')
    output = dut.image_info(app_path)
    sha256_app = re.search(r'Validation Hash:\s+([a-f0-9]+)', output).group(1)
    Utility.console_log('app SHA256: %s' % sha256_app)

    return sha256_bootloader, sha256_app


@ttfw_idf.idf_example_test(env_tag='Example_WIFI_OTA')
def test_examples_protocol_simple_ota_example(env, extra_data):
    """
    steps: |
      1. join AP
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    dut1 = env.get_dut('simple_ota_example', 'examples/system/ota/simple_ota_example', dut_class=ttfw_idf.ESP32DUT)
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, 'simple_ota.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('simple_ota_bin_size', '{}KB'.format(bin_size // 1024))
    sha256_bootloader, sha256_app = calc_all_sha256(dut1)
    # start test
    host_ip = get_my_ip()
    thread1 = Thread(target=start_https_server, args=(dut1.app.binary_path, host_ip, 8000))
    thread1.daemon = True
    thread1.start()
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset 0x10000', timeout=30)
    check_sha256(sha256_bootloader, dut1.expect(re.compile(r'SHA-256 for bootloader:\s+([a-f0-9]+)'))[0])
    check_sha256(sha256_app, dut1.expect(re.compile(r'SHA-256 for current firmware:\s+([a-f0-9]+)'))[0])
    try:
        ip_address = dut1.expect(re.compile(r' sta ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
    dut1.expect('Starting OTA example', timeout=30)

    print('writing to device: {}'.format('https://' + host_ip + ':8000/simple_ota.bin'))
    dut1.write('https://' + host_ip + ':8000/simple_ota.bin')
    dut1.expect('Loaded app from partition at offset 0x110000', timeout=60)
    dut1.expect('Starting OTA example', timeout=30)


@ttfw_idf.idf_example_test(env_tag='Example_EthKitV1')
def test_examples_protocol_simple_ota_example_ethernet_with_spiram_config(env, extra_data):
    """
    steps: |
      1. join AP
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    dut1 = env.get_dut('simple_ota_example', 'examples/system/ota/simple_ota_example', dut_class=ttfw_idf.ESP32DUT, app_config_name='spiram')
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, 'simple_ota.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('simple_ota_bin_size', '{}KB'.format(bin_size // 1024))
    # start test
    host_ip = get_my_ip()
    thread1 = Thread(target=start_https_server, args=(dut1.app.binary_path, host_ip, 8000))
    thread1.daemon = True
    thread1.start()
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset 0x10000', timeout=30)
    try:
        ip_address = dut1.expect(re.compile(r' eth ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
    dut1.expect('Starting OTA example', timeout=30)

    print('writing to device: {}'.format('https://' + host_ip + ':8000/simple_ota.bin'))
    dut1.write('https://' + host_ip + ':8000/simple_ota.bin')
    dut1.expect('Loaded app from partition at offset 0x110000', timeout=60)
    dut1.expect('Starting OTA example', timeout=30)


@ttfw_idf.idf_example_test(env_tag='Example_Flash_Encryption_OTA')
def test_examples_protocol_simple_ota_example_with_flash_encryption(env, extra_data):
    """
    steps: |
      1. join AP
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    dut1 = env.get_dut('simple_ota_example', 'examples/system/ota/simple_ota_example', dut_class=ttfw_idf.ESP32DUT, app_config_name='flash_enc')
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, 'simple_ota.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('simple_ota_bin_size', '{}KB'.format(bin_size // 1024))
    # erase flash on the device
    print('Erasing the flash in order to have an empty NVS key partiton')
    dut1.erase_flash()
    # start test
    host_ip = get_my_ip()
    thread1 = Thread(target=start_https_server, args=(dut1.app.binary_path, host_ip, 8000))
    thread1.daemon = True
    thread1.start()
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset 0x20000', timeout=30)
    dut1.expect('Flash encryption mode is DEVELOPMENT (not secure)', timeout=10)
    try:
        ip_address = dut1.expect(re.compile(r' eth ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
    dut1.expect('Starting OTA example', timeout=30)

    print('writing to device: {}'.format('https://' + host_ip + ':8000/simple_ota.bin'))
    dut1.write('https://' + host_ip + ':8000/simple_ota.bin')
    dut1.expect('Loaded app from partition at offset 0x120000', timeout=60)
    dut1.expect('Flash encryption mode is DEVELOPMENT (not secure)', timeout=10)
    dut1.expect('Starting OTA example', timeout=30)


@ttfw_idf.idf_example_test(env_tag='Example_Flash_Encryption_OTA_WiFi', target=['esp32c3'])
def test_examples_protocol_simple_ota_example_with_flash_encryption_wifi(env, extra_data):
    """
    steps: |
      1. join AP
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    dut1 = env.get_dut('simple_ota_example', 'examples/system/ota/simple_ota_example', app_config_name='flash_enc_wifi')
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, 'simple_ota.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('simple_ota_bin_size', '{}KB'.format(bin_size // 1024))
    # erase flash on the device
    print('Erasing the flash in order to have an empty NVS key partiton')
    dut1.erase_flash()
    # start test
    host_ip = get_my_ip()
    thread1 = Thread(target=start_https_server, args=(dut1.app.binary_path, host_ip, 8000))
    thread1.daemon = True
    thread1.start()
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset 0x20000', timeout=30)
    dut1.expect('Flash encryption mode is DEVELOPMENT (not secure)', timeout=10)
    try:
        ip_address = dut1.expect(re.compile(r' sta ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
    dut1.expect('Starting OTA example', timeout=30)

    print('writing to device: {}'.format('https://' + host_ip + ':8000/simple_ota.bin'))
    dut1.write('https://' + host_ip + ':8000/simple_ota.bin')
    dut1.expect('Loaded app from partition at offset 0x120000', timeout=60)
    dut1.expect('Flash encryption mode is DEVELOPMENT (not secure)', timeout=10)
    dut1.expect('Starting OTA example', timeout=30)


@ttfw_idf.idf_example_test(env_tag='Example_EthKitV1')
def test_examples_protocol_simple_ota_example_with_verify_app_signature_on_update_no_secure_boot_ecdsa(env, extra_data):
    """
    steps: |
      1. join AP
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    dut1 = env.get_dut('simple_ota_example', 'examples/system/ota/simple_ota_example', dut_class=ttfw_idf.ESP32DUT,
                       app_config_name='on_update_no_sb_ecdsa')
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, 'simple_ota.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('simple_ota_bin_size', '{}KB'.format(bin_size // 1024))
    sha256_bootloader, sha256_app = calc_all_sha256(dut1)
    # start test
    host_ip = get_my_ip()
    thread1 = Thread(target=start_https_server, args=(dut1.app.binary_path, host_ip, 8000))
    thread1.daemon = True
    thread1.start()
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset 0x20000', timeout=30)
    check_sha256(sha256_bootloader, dut1.expect(re.compile(r'SHA-256 for bootloader:\s+([a-f0-9]+)'))[0])
    check_sha256(sha256_app, dut1.expect(re.compile(r'SHA-256 for current firmware:\s+([a-f0-9]+)'))[0])
    try:
        ip_address = dut1.expect(re.compile(r' eth ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
    dut1.expect('Starting OTA example', timeout=30)

    print('writing to device: {}'.format('https://' + host_ip + ':8000/simple_ota.bin'))
    dut1.write('https://' + host_ip + ':8000/simple_ota.bin')
    dut1.expect('Writing to partition subtype 16 at offset 0x120000', timeout=20)

    dut1.expect('Verifying image signature...', timeout=60)

    dut1.expect('Loaded app from partition at offset 0x120000', timeout=20)
    dut1.expect('Starting OTA example', timeout=30)


@ttfw_idf.idf_example_test(env_tag='Example_EthKitV12')
def test_examples_protocol_simple_ota_example_with_verify_app_signature_on_update_no_secure_boot_rsa(env, extra_data):
    """
    steps: |
      1. join AP
      2. Fetch OTA image over HTTPS
      3. Reboot with the new OTA image
    """
    dut1 = env.get_dut('simple_ota_example', 'examples/system/ota/simple_ota_example', dut_class=ttfw_idf.ESP32DUT,
                       app_config_name='on_update_no_sb_rsa')
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, 'simple_ota.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('simple_ota_bin_size', '{}KB'.format(bin_size // 1024))
    sha256_bootloader, sha256_app = calc_all_sha256(dut1)
    # start test
    host_ip = get_my_ip()
    thread1 = Thread(target=start_https_server, args=(dut1.app.binary_path, host_ip, 8000))
    thread1.daemon = True
    thread1.start()
    dut1.start_app()
    dut1.expect('Loaded app from partition at offset 0x20000', timeout=30)
    check_sha256(sha256_bootloader, dut1.expect(re.compile(r'SHA-256 for bootloader:\s+([a-f0-9]+)'))[0])
    check_sha256(sha256_app, dut1.expect(re.compile(r'SHA-256 for current firmware:\s+([a-f0-9]+)'))[0])
    try:
        ip_address = dut1.expect(re.compile(r' eth ip: ([^,]+),'), timeout=30)
        print('Connected to AP with IP: {}'.format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
    dut1.expect('Starting OTA example', timeout=30)

    print('writing to device: {}'.format('https://' + host_ip + ':8000/simple_ota.bin'))
    dut1.write('https://' + host_ip + ':8000/simple_ota.bin')
    dut1.expect('Writing to partition subtype 16 at offset 0x120000', timeout=20)

    dut1.expect('Verifying image signature...', timeout=60)
    dut1.expect('#0 app key digest == #0 trusted key digest', timeout=10)
    dut1.expect('Verifying with RSA-PSS...', timeout=10)
    dut1.expect('Signature verified successfully!', timeout=10)

    dut1.expect('Loaded app from partition at offset 0x120000', timeout=20)
    dut1.expect('Starting OTA example', timeout=30)


if __name__ == '__main__':
    if sys.argv[2:]:    # if two or more arguments provided:
        # Usage: example_test.py <image_dir> <server_port> [cert_di>]
        this_dir = os.path.dirname(os.path.realpath(__file__))
        bin_dir = os.path.join(this_dir, sys.argv[1])
        port = int(sys.argv[2])
        cert_dir = bin_dir if not sys.argv[3:] else os.path.join(this_dir, sys.argv[3])  # optional argument
        print('Starting HTTPS server at "https://:{}"'.format(port))
        start_https_server(bin_dir, '', port,
                           server_file=os.path.join(cert_dir, 'ca_cert.pem'),
                           key_file=os.path.join(cert_dir, 'ca_key.pem'))
    else:
        test_examples_protocol_simple_ota_example()
        test_examples_protocol_simple_ota_example_ethernet_with_spiram_config()
        test_examples_protocol_simple_ota_example_with_flash_encryption()
        test_examples_protocol_simple_ota_example_with_flash_encryption_wifi()
        test_examples_protocol_simple_ota_example_with_verify_app_signature_on_update_no_secure_boot_ecdsa()
        test_examples_protocol_simple_ota_example_with_verify_app_signature_on_update_no_secure_boot_rsa()
