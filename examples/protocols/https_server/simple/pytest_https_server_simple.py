#!/usr/bin/env python
#
# SPDX-FileCopyrightText: 2021-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import http.client
import logging
import os
import ssl

import pytest
from common_test_methods import get_env_config_variable
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

server_cert_pem = (
    '-----BEGIN CERTIFICATE-----\n'
    'MIIDOzCCAiOgAwIBAgIUG/S51QF4EeUkdaqg54oogqIKBZkwDQYJKoZIhvcNAQEL\n'
    'BQAwJTEjMCEGA1UEAwwaRVNQMzIgSFRUUFMgc2VydmVyIGV4YW1wbGUwHhcNMjUw\n'
    'NDAyMDcwMzI2WhcNMzUwMzMxMDcwMzI2WjAlMSMwIQYDVQQDDBpFU1AzMiBIVFRQ\n'
    'UyBzZXJ2ZXIgZXhhbXBsZTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n'
    'ALBint6nP77RCQcmKgwPtTsGK0uClxg+LwKJ3WXuye3oqnnjqJCwMEneXzGdG09T\n'
    'sA0SyNPwrEgebLCH80an3gWU4pHDdqGHfJQa2jBL290e/5L5MB+6PTs2NKcojK/k\n'
    'qcZkn58MWXhDW1NpAnJtjVniK2Ksvr/YIYSbyD+JiEs0MGxEx+kOl9d7hRHJaIzd\n'
    'GF/vO2pl295v1qXekAlkgNMtYIVAjUy9CMpqaQBCQRL+BmPSJRkXBsYk8GPnieS4\n'
    'sUsp53DsNvCCtWDT6fd9D1v+BB6nDk/FCPKhtjYOwOAZlX4wWNSZpRNr5dfrxKsb\n'
    'jAn4PCuR2akdF4G8WLUeDWECAwEAAaNjMGEwHQYDVR0OBBYEFMnmdJKOEepXrHI/\n'
    'ivM6mVqJgAX8MB8GA1UdIwQYMBaAFMnmdJKOEepXrHI/ivM6mVqJgAX8MA8GA1Ud\n'
    'EwEB/wQFMAMBAf8wDgYDVR0PAQH/BAQDAgKEMA0GCSqGSIb3DQEBCwUAA4IBAQBP\n'
    'AgAagM33DqsDi+UArUxEoqmov1rH0PHXnd/a6Ct/IvNzr0qUH8hW4Lv0tWHfOJY8\n'
    'pCf7bkejxXlhP/QHb6M+sobN9tN/WupEaeqNg4pCWi+6Caj2uFW9vkQQf2j50lMg\n'
    'R0oxnd6SMEQArzy3f3yYRp8rliPERY6F2Rtb9HJNh53K51FE60xONPLZ/1dtSgDB\n'
    'KcJseZfhg6oAUSLjFCYJEn5xa7CsIuQ8Jx2xMo4IkU44BJ8TJS4zw/hP1/vVjjvS\n'
    'uU2Z0ZOUCQ78/3eMnsFfLMtDUYqXPyhNogm51GeHOR6dk+ICQ+c5gCDkJUnOTqzg\n'
    'G2JUmXAXxJoUZDfalijl\n'
    '-----END CERTIFICATE-----\n'
)

client_cert_pem = (
    '-----BEGIN CERTIFICATE-----\n'
    'MIID7TCCAtWgAwIBAgIUBdm7RStsshnl3CCpknSJhXQK4GcwDQYJKoZIhvcNAQEL\n'
    'BQAwgYUxCzAJBgNVBAYTAkNOMRAwDgYDVQQIDAdKaWFuZ3N1MQ8wDQYDVQQHDAZT\n'
    'dXpob3UxEjAQBgNVBAoMCUVzcHJlc3NpZjEMMAoGA1UECwwDY29tMRIwEAYDVQQD\n'
    'DAkxMjcuMC4wLjExHTAbBgkqhkiG9w0BCQEWDmVzcDMyeEBlc3AuY29tMB4XDTIx\n'
    'MTAwNTExMTMxMFoXDTMxMTAwMzExMTMxMFowgYUxCzAJBgNVBAYTAkNOMRAwDgYD\n'
    'VQQIDAdKaWFuZ3N1MQ8wDQYDVQQHDAZTdXpob3UxEjAQBgNVBAoMCUVzcHJlc3Np\n'
    'ZjEMMAoGA1UECwwDY29tMRIwEAYDVQQDDAkxMjcuMC4wLjExHTAbBgkqhkiG9w0B\n'
    'CQEWDmVzcDMyeEBlc3AuY29tMIIBIjANBgkqhkiG9w0BAQEFAAOCAQ8AMIIBCgKC\n'
    'AQEAu2nP0HPtgKvRUwFuOs72caf4oyeK33OVfa6fGGttr/QYyw9PrwtdFDyEWEiI\n'
    '4P4hnxNC+bvNSYtJUzF9EmkqrUtKxhBsRVTKWOqumcgtiMWOxpdVKl0936ne2Pqh\n'
    'SweddrQwvPDFuB3hRikRX11+d5vkjFBV9FoZobKHWemDkXSc2R99xRie5PJoEfoz\n'
    'rmu5zjCaPHxzkyZsmH4MILfTuhUGc/Eye9Nl+lpY5KLjM14ZMQLK1CHRuI/oqCN6\n'
    '1WQrgUY5EyXGe0jXHTVhlL2RN8njxJ/4r3JnK/BQkcXTIMPOP8jIv9Sy1HhxfXKy\n'
    'HzLqOBn0Ft+mOADrpAWX8WnwUQIDAQABo1MwUTAdBgNVHQ4EFgQUpu4d8d+IywjB\n'
    'HMiKX84L+1ri8BIwHwYDVR0jBBgwFoAUpu4d8d+IywjBHMiKX84L+1ri8BIwDwYD\n'
    'VR0TAQH/BAUwAwEB/zANBgkqhkiG9w0BAQsFAAOCAQEAXm5Hn/aKKO3RnHqqfxok\n'
    'Hbw5yA2L2T6VPj2puI0Sh5GW62INjM0Kszy3L5mQqLUSsjcEcFAZmpeo14ytPRLG\n'
    'o6+WG/4er3hBA7D8oDni7hp8Qs+/EtNuEuoU+qQiKsT2DvA5rafT7laNfvjgqaoJ\n'
    'YMTCvzKLnMBaglB+qC9grgvJwMN0RTzHyY6UySdNZmcf5QXWLWjsX8E8/u4iSq8l\n'
    'eZlddTjh7HGGEOim7AkvKR9VYAvKGOV+FvUzCxPpoTr6kS2NGwnR7QnvKADECtLj\n'
    'gf+hW1FalMn0yTVspg4+BNbIThh0thbsvPDUTekMNfaRKKHZpJP2Ty3LkCbANLBR\n'
    'tQ==\n'
    '-----END CERTIFICATE-----\n'
)


client_key_pem = (
    '-----BEGIN PRIVATE KEY-----\n'
    'MIIEvgIBADANBgkqhkiG9w0BAQEFAASCBKgwggSkAgEAAoIBAQC7ac/Qc+2Aq9FT\n'
    'AW46zvZxp/ijJ4rfc5V9rp8Ya22v9BjLD0+vC10UPIRYSIjg/iGfE0L5u81Ji0lT\n'
    'MX0SaSqtS0rGEGxFVMpY6q6ZyC2IxY7Gl1UqXT3fqd7Y+qFLB512tDC88MW4HeFG\n'
    'KRFfXX53m+SMUFX0WhmhsodZ6YORdJzZH33FGJ7k8mgR+jOua7nOMJo8fHOTJmyY\n'
    'fgwgt9O6FQZz8TJ702X6WljkouMzXhkxAsrUIdG4j+ioI3rVZCuBRjkTJcZ7SNcd\n'
    'NWGUvZE3yePEn/ivcmcr8FCRxdMgw84/yMi/1LLUeHF9crIfMuo4GfQW36Y4AOuk\n'
    'BZfxafBRAgMBAAECggEBAJuJZ1UCwRtGfUS8LTVVSiZtVuZhDNoB3REfeR4VGkUq\n'
    '+eCcZm9JqQgAaX2zRRYlEtYocC8+c1MT69jFe51p9mc302ipfJHVmtFMg3dRMKkP\n'
    '/DxIn/+2voD/Q9kjt/TC7yXyyXglApKZCbrmnmpc93ZgxL7GdW+Dzz3pIne2WuC9\n'
    'T6ie71R8X60sau6ApMgkUq6On0f21v/VLkNU67tQJGBF6Q1HE8PK7Ptun3WSBVNm\n'
    'FNNJKRBwiqfWXe9hPlqqCWayYBrojSqJJXn5Xd6n5XzLDPzAXuPlkPF3VwWeXGam\n'
    '3RBZA26gwv50E1PeiUQOipkR57J+O9j/oA07AnhsxPkCgYEA8RMvE3ImZTkPVqdX\n'
    '72E2A5ScJswVvZelnRS/mG8U+8UlvevAu5MYr717DHKHy3yOw/u7wbkqk6KEIcyz\n'
    'ctNPBPqTweaZ28eEY/+lXSdQaWLD2UgZC8JIcMOSeFugghEHeBaxLzUYBNDToE3q\n'
    '1El2HJ7W14QuTA+CEtCEb+tc7ssCgYEAxwQkBTT8A7mOEE0phfUACqaBuAXld+zu\n'
    'I3PNJDIhg1ZABEJ9vo9+3duFDoEHVsJOetijrBBxf/XAvi3bTJ+gAjcA54cGpkxz\n'
    '6ssbFWZeC9exyo0ILKn33o716GrCvQn1kmuF2gasmAcrOVsMygawR7P02oasDP/X\n'
    'UckbZdqofdMCgYEAom0GfteePv0e9Idzm/mnZuot+4Xt7/vIvflIze+p96hxMXEy\n'
    'Pi9xppbH3S8dh2C44Bsv+epEYYxR8mP1VBxDVVtvSmmQqJ/Y93c7d3QRna/JvQ/y\n'
    'sBWKsU9T1HwHvRq0KZlAcEoZkMUSkSNuYPHN/qKWpkaM2vpn7T1Ivg+aYdkCgYA/\n'
    'CGO0NnzfXSTOqvHM2LVDqksJkuyD2Enwdpvxq+MLawTplHmpIl/HOuDgoCNH6lDa\n'
    '/cSRGcApDBgY5ANCOIiASxWBPzXu8+X+5odUdtCwpYdNJPAC3W6BUfw2uaGmKAJc\n'
    'dqu1S0nc+OBK0Tiyv/2TKD8T+3WAxINZBv4je2bEOwKBgEavm5zTN9NILJsJCf9k\n'
    'te7+uDFuyoNWkL1vmMPuJYVC1QMVq1yr3DSaxA19BG9P4ZyOMOwVlPVWA+LofD4D\n'
    'S+w4Jjl2KDI4tSLUr6bsAJWdDfmrmGmRN3Kpds4RXaymV3rjj7qRk1J+ivtwo89s\n'
    'Vj+VslYzxw7FKKmnBgh/qGbJ\n'
    '-----END PRIVATE KEY-----\n'
)

success_response = '<h1>Hello Secure World!</h1>'


@pytest.mark.wifi_router
@idf_parametrize('target', ['esp32', 'esp32c3', 'esp32s3'], indirect=['target'])
def test_examples_protocol_https_server_simple(dut: Dut) -> None:
    """
    steps: |
      1. join AP
      2. connect to www.howsmyssl.com:443
      3. send http request
    """
    # check and log bin size
    binary_file = os.path.join(dut.app.binary_path, 'https_server.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info('https_server_simple_bin_size : {}KB'.format(bin_size // 1024))
    # start test
    logging.info('Waiting to connect with AP')
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    # Parse IP address and port of the server
    dut.expect(r'Starting server')
    got_port = int(dut.expect(r'Server listening on port (\d+)', timeout=30)[1].decode())
    got_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()

    # Expected logs

    logging.info('Got IP   : {}'.format(got_ip))
    logging.info('Got Port : {}'.format(got_port))

    logging.info('Performing GET request over an SSL connection with the server')

    CLIENT_CERT_FILE = 'client_cert.pem'
    CLIENT_KEY_FILE = 'client_key.pem'

    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
    ssl_context.verify_mode = ssl.CERT_REQUIRED
    ssl_context.check_hostname = False
    ssl_context.load_verify_locations(cadata=server_cert_pem)

    with open(CLIENT_CERT_FILE, 'w', encoding='utf-8') as cert, open(CLIENT_KEY_FILE, 'w', encoding='utf-8') as key:
        cert.write(client_cert_pem)
        key.write(client_key_pem)

    ssl_context.load_cert_chain(certfile=CLIENT_CERT_FILE, keyfile=CLIENT_KEY_FILE)

    conn = http.client.HTTPSConnection(got_ip, got_port, context=ssl_context)
    logging.info('Performing SSL handshake with the server')
    conn.request('GET', '/')
    resp = conn.getresponse()
    dut.expect('performing session handshake')
    got_resp = resp.read().decode('utf-8')
    if got_resp != success_response:
        logging.info('Response obtained does not match with correct response')
        raise RuntimeError('Failed to test SSL connection')

    if dut.app.sdkconfig.get('CONFIG_EXAMPLE_ENABLE_HTTPS_USER_CALLBACK') is True:
        current_cipher = dut.expect(r'Current Ciphersuite(.*)', timeout=5)[0]
        logging.info('Current Ciphersuite {}'.format(current_cipher))

        logging.info('Checking user callback: Obtaining client certificate...')

        serial_number = dut.expect(r'serial number\s*:([^\n]*)', timeout=5)[0]
        issuer_name = dut.expect(r'issuer name\s*:([^\n]*)', timeout=5)[0]
        expiry = dut.expect(r'expires on ((.*)\d{4}\-(0?[1-9]|1[012])\-(0?[1-9]|[12][0-9]|3[01])*)', timeout=5)[
            1
        ].decode()

        logging.info('Serial No. {}'.format(serial_number))
        logging.info('Issuer Name {}'.format(issuer_name))
        logging.info('Expires on {}'.format(expiry))

    # Close the connection
    conn.close()
    logging.info('Correct response obtained')
    logging.info('SSL connection test successful\nClosing the connection')


@pytest.mark.wifi_router
@pytest.mark.parametrize(
    'config',
    [
        'dynamic_buffer',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32', 'esp32c3', 'esp32s3'], indirect=['target'])
def test_examples_protocol_https_server_simple_dynamic_buffers(dut: Dut) -> None:
    # Test with mbedTLS dynamic buffer feature

    # start test
    logging.info('Waiting to connect with AP')
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    # Parse IP address and port of the server
    dut.expect(r'Starting server')
    got_port = int(dut.expect(r'Server listening on port (\d+)', timeout=30)[1].decode())
    got_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()

    # Expected logs

    logging.info('Got IP   : {}'.format(got_ip))
    logging.info('Got Port : {}'.format(got_port))

    logging.info('Performing GET request over an SSL connection with the server')

    CLIENT_CERT_FILE = 'client_cert.pem'
    CLIENT_KEY_FILE = 'client_key.pem'

    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLSv1_2)
    ssl_context.verify_mode = ssl.CERT_REQUIRED
    ssl_context.check_hostname = False
    ssl_context.load_verify_locations(cadata=server_cert_pem)

    ssl_context.load_cert_chain(certfile=CLIENT_CERT_FILE, keyfile=CLIENT_KEY_FILE)

    os.remove(CLIENT_CERT_FILE)
    os.remove(CLIENT_KEY_FILE)

    conn = http.client.HTTPSConnection(got_ip, got_port, context=ssl_context)
    logging.info('Performing SSL handshake with the server')
    conn.request('GET', '/')
    resp = conn.getresponse()
    dut.expect('performing session handshake')
    got_resp = resp.read().decode('utf-8')
    if got_resp != success_response:
        logging.info('Response obtained does not match with correct response')
        raise RuntimeError('Failed to test SSL connection')

    if dut.app.sdkconfig.get('CONFIG_EXAMPLE_ENABLE_HTTPS_USER_CALLBACK') is True:
        current_cipher = dut.expect(r'Current Ciphersuite(.*)', timeout=5)[0]
        logging.info('Current Ciphersuite {}'.format(current_cipher))

        logging.info('Checking user callback: Obtaining client certificate...')

        serial_number = dut.expect(r'serial number\s*:([^\n]*)', timeout=5)[0]
        issuer_name = dut.expect(r'issuer name\s*:([^\n]*)', timeout=5)[0]
        expiry = dut.expect(r'expires on\s*:((.*)\d{4}\-(0?[1-9]|1[012])\-(0?[1-9]|[12][0-9]|3[01])*)', timeout=5)[
            1
        ].decode()

        logging.info('Serial No. : {}'.format(serial_number))
        logging.info('Issuer Name : {}'.format(issuer_name))
        logging.info('Expires on : {}'.format(expiry))

    # Close the connection
    conn.close()
    logging.info('Correct response obtained')
    logging.info('SSL connection test successful\nClosing the connection')


@pytest.mark.wifi_router
@pytest.mark.parametrize(
    'config',
    [
        'tls1_3',
    ],
    indirect=True,
)
@idf_parametrize('target', ['esp32', 'esp32c3', 'esp32s3'], indirect=['target'])
def test_examples_protocol_https_server_tls1_3(dut: Dut) -> None:
    logging.info('Waiting to connect with AP')
    if dut.app.sdkconfig.get('EXAMPLE_WIFI_SSID_PWD_FROM_STDIN') is True:
        dut.expect('Please input ssid password:')
        env_name = 'wifi_router'
        ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
        ap_password = get_env_config_variable(env_name, 'ap_password')
        dut.write(f'{ap_ssid} {ap_password}')
    # Parse IP address and port of the server
    dut.expect(r'Starting server')
    got_port = int(dut.expect(r'Server listening on port (\d+)', timeout=30)[1].decode())
    got_ip = dut.expect(r'IPv4 address: (\d+\.\d+\.\d+\.\d+)[^\d]', timeout=30)[1].decode()

    # Expected logs

    logging.info('Got IP   : {}'.format(got_ip))
    logging.info('Got Port : {}'.format(got_port))
    logging.info('Performing GET request over an SSL connection with the server using TLSv1.3')

    CLIENT_CERT_FILE = 'client_cert.pem'
    CLIENT_KEY_FILE = 'client_key.pem'

    with open(CLIENT_CERT_FILE, 'w', encoding='utf-8') as cert, open(CLIENT_KEY_FILE, 'w', encoding='utf-8') as key:
        cert.write(client_cert_pem)
        key.write(client_key_pem)

    ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
    ssl_context.minimum_version = ssl.TLSVersion.TLSv1_3
    ssl_context.maximum_version = ssl.TLSVersion.TLSv1_3
    ssl_context.verify_mode = ssl.CERT_REQUIRED
    ssl_context.check_hostname = False
    ssl_context.load_verify_locations(cadata=server_cert_pem)

    ssl_context.load_cert_chain(certfile=CLIENT_CERT_FILE, keyfile=CLIENT_KEY_FILE)

    os.remove(CLIENT_CERT_FILE)
    os.remove(CLIENT_KEY_FILE)

    conn = http.client.HTTPSConnection(got_ip, got_port, context=ssl_context)
    logging.info('Performing SSL handshake with the server')
    conn.request('GET', '/')
    resp = conn.getresponse()
    dut.expect('performing session handshake')
    got_resp = resp.read().decode('utf-8')
    if got_resp != success_response:
        logging.info('Response obtained does not match with correct response')
        raise RuntimeError('Failed to test SSL connection')

    if dut.app.sdkconfig.get('CONFIG_EXAMPLE_ENABLE_HTTPS_USER_CALLBACK') is True:
        current_cipher = dut.expect(r'Current Ciphersuite(.*)', timeout=5)[0]
        logging.info('Current Ciphersuite {}'.format(current_cipher))

        logging.info('Checking user callback: Obtaining client certificate...')

        serial_number = dut.expect(r'serial number\s*:([^\n]*)', timeout=5)[0]
        issuer_name = dut.expect(r'issuer name\s*:([^\n]*)', timeout=5)[0]
        expiry = dut.expect(
            r'expires on\s*:((.*)\d{4}\-(0?[1-9]|1[012])\-(0?[1-9]|[12][0-9]|3[01])*)',
            timeout=5,
        )[1].decode()

        logging.info('Serial No. : {}'.format(serial_number))
        logging.info('Issuer Name : {}'.format(issuer_name))
        logging.info('Expires on : {}'.format(expiry))

    # Close the connection
    conn.close()
    logging.info('Correct response obtained')
    logging.info('SSL connection test successful\nClosing the connection')
