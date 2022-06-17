# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0

# APIs for interpreting and creating protobuf packets for
# protocomm endpoint with security type protocomm_security2


from typing import Any, Type

import proto
from cryptography.hazmat.primitives.ciphers.aead import AESGCM
from future.utils import tobytes

from .security import Security
from .srp6a import Srp6a, bytes_to_long, generate_salt_and_verifier, long_to_bytes

AES_KEY_LEN = 256 // 8


# Enum for state of protocomm_security1 FSM
class security_state:
    REQUEST1 = 0
    RESPONSE1_REQUEST2 = 1
    RESPONSE2 = 2
    FINISHED = 3


def sec2_gen_salt_verifier(username: str, password: str, salt_len: int) -> Any:
    salt, verifier = generate_salt_and_verifier(username, password, len_s=salt_len)

    salt_str = ', '.join([format(b, '#04x') for b in salt])
    salt_c_arr = '\n    '.join(salt_str[i: i + 96] for i in range(0, len(salt_str), 96))
    print(f'static const char sec2_salt[] = {{\n    {salt_c_arr}\n}};\n')

    verifier_str = ', '.join([format(b, '#04x') for b in verifier])
    verifier_c_arr = '\n    '.join(verifier_str[i: i + 96] for i in range(0, len(verifier_str), 96))
    print(f'static const char sec2_verifier[] = {{\n    {verifier_c_arr}\n}};\n')


class Security2(Security):
    def __init__(self, username: str, password: str, verbose: bool) -> None:
        # Initialize state of the security2 FSM
        self.session_state = security_state.REQUEST1
        self.username = username
        self.password = password
        self.verbose = verbose

        self.srp6a_ctx: Type[Srp6a]
        self.cipher: Type[AESGCM]

        self.client_pop_key = None
        self.nonce = None

        Security.__init__(self, self.security2_session)

    def security2_session(self, response_data: bytes) -> Any:
        # protocomm security2 FSM which interprets/forms
        # protobuf packets according to present state of session
        if (self.session_state == security_state.REQUEST1):
            self.session_state = security_state.RESPONSE1_REQUEST2
            return self.setup0_request()

        if (self.session_state == security_state.RESPONSE1_REQUEST2):
            self.session_state = security_state.RESPONSE2
            self.setup0_response(response_data)
            return self.setup1_request()

        if (self.session_state == security_state.RESPONSE2):
            self.session_state = security_state.FINISHED
            self.setup1_response(response_data)
            return None

        print('Unexpected state')
        return None

    def _print_verbose(self, data: str) -> None:
        if (self.verbose):
            print(f'\x1b[32;20m++++ {data} ++++\x1b[0m')

    def setup0_request(self) -> Any:
        # Form SessionCmd0 request packet using client public key
        setup_req = proto.session_pb2.SessionData()
        setup_req.sec_ver = proto.session_pb2.SecScheme2
        setup_req.sec2.msg = proto.sec2_pb2.S2Session_Command0

        setup_req.sec2.sc0.client_username = tobytes(self.username)
        self.srp6a_ctx = Srp6a(self.username, self.password)
        if self.srp6a_ctx is None:
            print('Failed to initialize SRP6a instance!')
            exit(1)

        client_pubkey = long_to_bytes(self.srp6a_ctx.A)
        setup_req.sec2.sc0.client_pubkey = client_pubkey

        self._print_verbose('Client Public Key:\t' + hex(bytes_to_long(client_pubkey)))
        return setup_req.SerializeToString().decode('latin-1')

    def setup0_response(self, response_data: bytes) -> None:
        # Interpret SessionResp0 response packet
        setup_resp = proto.session_pb2.SessionData()
        setup_resp.ParseFromString(tobytes(response_data))
        self._print_verbose('Security version:\t' + str(setup_resp.sec_ver))
        if setup_resp.sec_ver != proto.session_pb2.SecScheme2:
            print('Incorrect sec scheme')
            exit(1)

        # Device public key, random salt and password verifier
        device_pubkey = setup_resp.sec2.sr0.device_pubkey
        device_salt = setup_resp.sec2.sr0.device_salt

        self._print_verbose('Device Public Key:\t' + hex(bytes_to_long(device_pubkey)))
        self._print_verbose('Device Salt:\t' + hex(bytes_to_long(device_salt)))

        self.client_pop_key = self.srp6a_ctx.process_challenge(device_salt, device_pubkey)

    def setup1_request(self) -> Any:
        # Form SessionCmd1 request packet using encrypted device public key
        setup_req = proto.session_pb2.SessionData()
        setup_req.sec_ver = proto.session_pb2.SecScheme2
        setup_req.sec2.msg = proto.sec2_pb2.S2Session_Command1

        # Encrypt device public key and attach to the request packet
        self._print_verbose('Client Proof:\t' + hex(bytes_to_long(self.client_pop_key)))
        setup_req.sec2.sc1.client_proof = self.client_pop_key

        return setup_req.SerializeToString().decode('latin-1')

    def setup1_response(self, response_data: bytes) -> Any:
        # Interpret SessionResp1 response packet
        setup_resp = proto.session_pb2.SessionData()
        setup_resp.ParseFromString(tobytes(response_data))
        # Ensure security scheme matches
        if setup_resp.sec_ver == proto.session_pb2.SecScheme2:
            # Read encrypyed device proof string
            device_proof = setup_resp.sec2.sr1.device_proof
            self._print_verbose('Device Proof:\t' + hex(bytes_to_long(device_proof)))
            self.srp6a_ctx.verify_session(device_proof)
            if not self.srp6a_ctx.authenticated():
                print('Failed to verify device proof')
                exit(1)
        else:
            print('Unsupported security protocol')
            exit(1)

        # Getting the shared secret
        shared_secret = self.srp6a_ctx.get_session_key()
        self._print_verbose('Shared Secret:\t' + hex(bytes_to_long(shared_secret)))

        # Using the first 256 bits of a 512 bit key
        session_key = shared_secret[:AES_KEY_LEN]
        self._print_verbose('Session Key:\t' + hex(bytes_to_long(session_key)))

        # 96-bit nonce
        self.nonce = setup_resp.sec2.sr1.device_nonce
        self._print_verbose('Nonce:\t' + hex(bytes_to_long(self.nonce)))

        # Initialize the encryption engine with Shared Key and initialization vector
        self.cipher = AESGCM(session_key)
        if self.cipher is None:
            print('Failed to initialize AES-GCM cryptographic engine!')
            exit(1)

    def encrypt_data(self, data: bytes) -> Any:
        return self.cipher.encrypt(self.nonce, data, None)

    def decrypt_data(self, data: bytes) -> Any:
        return self.cipher.decrypt(self.nonce, data, None)
