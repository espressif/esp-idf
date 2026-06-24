# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
import hashlib
import http.server
import json
import logging
import multiprocessing
import os
import ssl
import time
from typing import Any

import pexpect
import pytest
from common_test_methods import get_env_config_variable
from common_test_methods import get_host_ip4_by_dest_ip
from cryptography.hazmat.primitives.asymmetric import utils
from ecdsa.curves import NIST256p
from ecdsa.keys import VerifyingKey
from ecdsa.util import sigdecode_der
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize

TEST_TARGETS = ['esp32c6', 'esp32c5', 'esp32c61', 'esp32h2']

TEST_TARGETS_OTA = ['esp32c6', 'esp32c5', 'esp32c61']

TEST_MSG = 'hello world'

server_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'test_certs/server_cert.pem')
key_file = os.path.join(os.path.dirname(os.path.abspath(__file__)), 'test_certs/server_key.pem')


###########################
# ESP-TEE: Secure Storage #
###########################


@pytest.mark.generic
@idf_parametrize('target', TEST_TARGETS, indirect=['target'])
def test_tee_cli_secure_storage(dut: Dut) -> None:
    # Dumping the REE binary size
    binary_file = os.path.join(dut.app.binary_path, 'tee_cli.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info(f'tee_cli_bin_size : {bin_size // 1024}KB')

    # Starting the test
    dut.expect('ESP-TEE: Secure services demonstration', timeout=30)
    time.sleep(1)

    # Get the SHA256 digest of the test message
    dut.write(f'get_msg_sha256 "{TEST_MSG}"')
    test_msg_hash = dut.expect(r'Message digest \(SHA256\) -\s*([0-9a-fA-F]{64})', timeout=30)[1].decode()
    time.sleep(1)

    # Test out the TEE secure storage workflow - Message signing and verification
    sec_stg_key_ids = {0: 'key0', 1: 'key1', 2: 'key2'}
    iterations = len(sec_stg_key_ids)

    for i in range(iterations):
        dut.write(f'tee_sec_stg_gen_key {sec_stg_key_ids.get(i)} 1')
        dut.expect(r'Generated ECDSA_SECP256R1 key with ID (\S+)', timeout=30)

        dut.write(f'tee_sec_stg_sign {sec_stg_key_ids.get(i)} {test_msg_hash}')
        test_msg_sign = dut.expect(r'Generated signature -\s*([0-9a-fA-F]{128})', timeout=30)[1].decode()
        test_msg_pubkey = dut.expect(r'Public key \(Uncompressed\) -\s*([0-9a-fA-F]{130})', timeout=30)[1].decode()
        dut.expect('Signature verified successfully', timeout=30)

        vk = VerifyingKey.from_string(bytes.fromhex(test_msg_pubkey), curve=NIST256p, hashfunc=hashlib.sha256)
        assert vk.verify_digest(bytes.fromhex(test_msg_sign), bytes.fromhex(test_msg_hash))
        time.sleep(1)

    # Test out the TEE secure storage workflow - Encryption and decryption
    for i in range(iterations):
        dut.write(f'tee_sec_stg_gen_key {sec_stg_key_ids.get(i)} 0')
        dut.expect(r'Generated AES256 key with ID (\S+)', timeout=30)

        dut.write(f'tee_sec_stg_encrypt {sec_stg_key_ids.get(i)} {test_msg_hash}')
        test_msg_cipher = dut.expect(r'Ciphertext -\s*([0-9a-fA-F]{64})', timeout=30)[1].decode()
        test_msg_iv = dut.expect(r'IV -\s*([0-9a-fA-F]{24})', timeout=30)[1].decode()
        test_msg_tag = dut.expect(r'Tag -\s*([0-9a-fA-F]{32})', timeout=30)[1].decode()

        dut.write(f'tee_sec_stg_decrypt {sec_stg_key_ids.get(i)} {test_msg_cipher} {test_msg_iv} {test_msg_tag}')
        test_msg_decipher = dut.expect(r'Decrypted plaintext -\s*([0-9a-fA-F]{64})', timeout=30)[1].decode()

        assert test_msg_decipher == test_msg_hash
        time.sleep(1)


########################
# ESP-TEE: Attestation #
########################


def verify_att_token_signature(att_tk: str) -> Any:
    # Parsing the token
    tk_info = json.loads(att_tk)

    # Fetching the data to be verified
    tk_hdr_val = json.dumps(tk_info['header'], separators=(',', ':')).encode('latin-1')
    tk_eat_val = json.dumps(tk_info['eat'], separators=(',', ':')).encode('latin-1')
    tk_pubkey_val = json.dumps(tk_info['public_key'], separators=(',', ':')).encode('latin-1')

    # Pre-hashing the data
    ctx = hashlib.new('sha256')
    ctx.update(tk_hdr_val)
    ctx.update(tk_eat_val)
    ctx.update(tk_pubkey_val)
    digest = ctx.digest()

    # Fetching the public key
    tk_pubkey_c = bytes.fromhex(tk_info['public_key']['compressed'])

    # Fetching the appended signature
    tk_sign_r = bytes.fromhex(tk_info['sign']['r'])
    tk_sign_s = bytes.fromhex(tk_info['sign']['s'])

    # Construct the signature using the R and S components
    signature = utils.encode_dss_signature(int.from_bytes(tk_sign_r, 'big'), int.from_bytes(tk_sign_s, 'big'))

    # Uncompress the public key and verify the signature
    vk = VerifyingKey.from_string(tk_pubkey_c, NIST256p, hashfunc=hashlib.sha256)
    return vk.verify_digest(signature, digest, sigdecode=sigdecode_der)


@pytest.mark.generic
@idf_parametrize('target', TEST_TARGETS, indirect=['target'])
def test_tee_cli_attestation(dut: Dut) -> None:
    # Dumping the REE binary size
    binary_file = os.path.join(dut.app.binary_path, 'tee_cli.bin')
    bin_size = os.path.getsize(binary_file)
    logging.info(f'tee_cli_bin_size : {bin_size // 1024}KB')

    # Starting the test
    dut.expect('ESP-TEE: Secure services demonstration', timeout=30)
    time.sleep(1)

    att_key_id = dut.app.sdkconfig.get('SECURE_TEE_ATT_KEY_STR_ID')
    dut.write(f'tee_sec_stg_gen_key {att_key_id} 1')
    dut.expect(r'Generated ECDSA_SECP256R1 key with ID (\S+)', timeout=30)

    # Get the Entity Attestation token from TEE and verify its signature
    dut.write('tee_att_info')
    dut.expect(r'Attestation token - Length: (\d+)', timeout=30)
    att_tk = dut.expect(r"'(.*?)'", timeout=30)[1].decode()
    assert verify_att_token_signature(att_tk)


#######################################
# ESP-TEE: Over-the-Air (OTA) updates #
#######################################


def start_https_server(ota_image_dir: str, server_ip: str, server_port: int) -> None:
    os.chdir(ota_image_dir)
    server_address = (server_ip, server_port)

    Handler = http.server.SimpleHTTPRequestHandler
    httpd = http.server.HTTPServer(server_address, Handler)

    context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
    context.load_cert_chain(certfile=server_file, keyfile=key_file)

    httpd.socket = context.wrap_socket(httpd.socket, server_side=True)
    httpd.serve_forever()


@pytest.mark.wifi_high_traffic
@idf_parametrize('target', TEST_TARGETS_OTA, indirect=['target'])
def test_tee_cli_secure_ota_wifi(dut: Dut) -> None:
    """
    This is a positive test case, which downloads complete binary file multiple number of times.
    Number of iterations can be specified in variable iterations.
    steps:
      1. join AP
      2. Fetch TEE/REE OTA image over HTTPS
      3. Reboot with the new TEE OTA image
    """
    # Number of iterations to validate OTA
    iterations = 4
    server_port = 8001
    tee_bin = 'esp_tee/esp_tee.bin'
    user_bin = 'tee_cli.bin'
    prev_tee_offs = None
    prev_app_offs = None

    # Fetch Wi-Fi credentials
    env_name = 'wifi_high_traffic'
    ap_ssid = get_env_config_variable(env_name, 'ap_ssid')
    ap_password = get_env_config_variable(env_name, 'ap_password')

    # Start server
    thread1 = multiprocessing.Process(target=start_https_server, args=(dut.app.binary_path, '0.0.0.0', server_port))
    thread1.daemon = True
    thread1.start()
    time.sleep(1)

    try:
        # start test
        for i in range(iterations):
            # Boot up sequence checks
            curr_tee_offs = (
                dut.expect(r'Loaded TEE app from partition at offset (0x[0-9a-fA-F]+)', timeout=30).group(1).decode()
            )
            curr_app_offs = (
                dut.expect(r'Loaded app from partition at offset (0x[0-9a-fA-F]+)', timeout=30).group(1).decode()
            )

            # Check for offset change across iterations
            if prev_tee_offs is not None and curr_tee_offs == prev_tee_offs:
                raise ValueError('Updated TEE app is not running')

            prev_tee_offs = curr_tee_offs
            if prev_app_offs is None:
                prev_app_offs = curr_app_offs

            # Starting the test
            dut.expect('ESP-TEE: Secure services demonstration', timeout=30)
            time.sleep(2)

            # Connecting to Wi-Fi
            dut.write(f'wifi_connect {ap_ssid} {ap_password}')

            # Fetch the DUT IP address
            try:
                ip_address = dut.expect(r'got ip:(\d+\.\d+\.\d+\.\d+)[^\d]', timeout=60)[1].decode()
                print(f'Connected to AP/Ethernet with IP: {ip_address}')
            except pexpect.exceptions.TIMEOUT:
                raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')

            host_ip = get_host_ip4_by_dest_ip(ip_address)

            # User OTA for last iteration
            if i == (iterations - 1):
                dut.write(f'user_ota https://{host_ip}:{str(server_port)}/{user_bin}')
                dut.expect('OTA Succeed, Rebooting', timeout=150)
                curr_app_offs = (
                    dut.expect(r'Loaded app from partition at offset (0x[0-9a-fA-F]+)', timeout=30).group(1).decode()
                )
                if curr_app_offs == prev_app_offs:
                    raise ValueError('Updated user app is not running')
            else:
                dut.write(f'tee_ota https://{host_ip}:{str(server_port)}/{tee_bin}')
                dut.expect('esp_tee_ota_end succeeded', timeout=150)
                dut.expect('Prepare to restart system!', timeout=150)

    finally:
        thread1.terminate()
