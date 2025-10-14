# SPDX-FileCopyrightText: 2024-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import hashlib
import json
import logging
from typing import Any

import pytest
from cryptography.hazmat.primitives.asymmetric import utils
from ecdsa.curves import NIST256p
from ecdsa.keys import VerifyingKey
from ecdsa.util import sigdecode_der
from pytest_embedded import Dut
from pytest_embedded_idf.utils import idf_parametrize


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
@idf_parametrize('target', ['esp32c6'], indirect=['target'])
def test_example_tee_attestation(dut: Dut) -> None:
    # Erase the TEE secure_storage partition
    dut.serial.erase_partition('secure_storage')

    # Start test
    dut.expect('TEE Attestation Service', timeout=30)
    dut.expect(r'Attestation token - Length: (\d+)', timeout=30)
    att_tk = dut.expect(r"'(.*?)'", timeout=30)[1].decode()
    assert verify_att_token_signature(att_tk)

    logging.info('Attestation token - Signature verified!')
