# SPDX-FileCopyrightText: 2018-2025 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
# APIs for interpreting and creating protobuf packets for
# protocomm endpoint with security type protocomm_security2
import struct
from typing import Any
from typing import Type

import proto
from cryptography.hazmat.primitives.ciphers.aead import AESGCM
from utils import long_to_bytes
from utils import str_to_bytes

from .security import Security
from .srp6a import generate_salt_and_verifier
from .srp6a import Srp6a

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
    print(f'static const char sec2_salt[] = {{\n    {salt_c_arr}\n}};\n')  # noqa E702

    verifier_str = ', '.join([format(b, '#04x') for b in verifier])
    verifier_c_arr = '\n    '.join(verifier_str[i: i + 96] for i in range(0, len(verifier_str), 96))
    print(f'static const char sec2_verifier[] = {{\n    {verifier_c_arr}\n}};\n')  # noqa E702


class Security2(Security):
    def __init__(self, sec_patch_ver:int, username: str, password: str, verbose: bool) -> None:
        # Initialize state of the security2 FSM
        self.session_state = security_state.REQUEST1
        self.sec_patch_ver = sec_patch_ver
        self.username = username
        self.password = password
        self.verbose = verbose

        self.srp6a_ctx: Type[Srp6a]
        self.cipher: Type[AESGCM]

        self.client_pop_key = None
        self.nonce = bytearray()

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

        print('---- Unexpected state! ----')
        return None

    def _print_verbose(self, data: str) -> None:
        if (self.verbose):
            print(f'\x1b[32;20m++++ {data} ++++\x1b[0m')  # noqa E702

    def setup0_request(self) -> Any:
        # Form SessionCmd0 request packet using client public key
        setup_req = proto.session_pb2.SessionData()
        setup_req.sec_ver = proto.session_pb2.SecScheme2
        setup_req.sec2.msg = proto.sec2_pb2.S2Session_Command0

        setup_req.sec2.sc0.client_username = str_to_bytes(self.username)
        self.srp6a_ctx = Srp6a(self.username, self.password)
        if self.srp6a_ctx is None:
            raise RuntimeError('Failed to initialize SRP6a instance!')

        client_pubkey = long_to_bytes(self.srp6a_ctx.A)
        setup_req.sec2.sc0.client_pubkey = client_pubkey

        self._print_verbose(f'Client Public Key:\t0x{client_pubkey.hex()}')
        return setup_req.SerializeToString().decode('latin-1')

    def setup0_response(self, response_data: bytes) -> None:
        # Interpret SessionResp0 response packet
        setup_resp = proto.session_pb2.SessionData()
        setup_resp.ParseFromString(str_to_bytes(response_data))
        self._print_verbose(f'Security version:\t{str(setup_resp.sec_ver)}')
        if setup_resp.sec_ver != proto.session_pb2.SecScheme2:
            raise RuntimeError('Incorrect security scheme')

        # Device public key, random salt and password verifier
        device_pubkey = setup_resp.sec2.sr0.device_pubkey
        device_salt = setup_resp.sec2.sr0.device_salt

        self._print_verbose(f'Device Public Key:\t0x{device_pubkey.hex()}')
        self.client_pop_key = self.srp6a_ctx.process_challenge(device_salt, device_pubkey)

    def setup1_request(self) -> Any:
        # Form SessionCmd1 request packet using encrypted device public key
        setup_req = proto.session_pb2.SessionData()
        setup_req.sec_ver = proto.session_pb2.SecScheme2
        setup_req.sec2.msg = proto.sec2_pb2.S2Session_Command1

        # Encrypt device public key and attach to the request packet
        if self.client_pop_key is None:
            raise RuntimeError('Failed to generate client proof!')

        self._print_verbose(f'Client Proof:\t0x{self.client_pop_key.hex()}')
        setup_req.sec2.sc1.client_proof = self.client_pop_key

        return setup_req.SerializeToString().decode('latin-1')

    def setup1_response(self, response_data: bytes) -> Any:
        # Interpret SessionResp1 response packet
        setup_resp = proto.session_pb2.SessionData()
        setup_resp.ParseFromString(str_to_bytes(response_data))
        # Ensure security scheme matches
        if setup_resp.sec_ver == proto.session_pb2.SecScheme2:
            # Read encrypyed device proof string
            device_proof = setup_resp.sec2.sr1.device_proof
            self._print_verbose(f'Device Proof:\t0x{device_proof.hex()}')
            self.srp6a_ctx.verify_session(device_proof)
            if not self.srp6a_ctx.authenticated():
                raise RuntimeError('Failed to verify device proof')
        else:
            raise RuntimeError('Unsupported security protocol')

        # Getting the shared secret
        shared_secret = self.srp6a_ctx.get_session_key()
        self._print_verbose(f'Shared Secret:\t0x{shared_secret.hex()}')

        # Using the first 256 bits of a 512 bit key
        session_key = shared_secret[:AES_KEY_LEN]
        self._print_verbose(f'Session Key:\t0x{session_key.hex()}')

        # 96-bit nonce
        self.nonce = bytearray(setup_resp.sec2.sr1.device_nonce)
        if self.nonce is None:
            raise RuntimeError('Received invalid nonce from device!')
        self._print_verbose(f'Nonce:\t0x{self.nonce.hex()}')

        # Initialize the encryption engine with Shared Key and initialization vector
        self.cipher = AESGCM(session_key)
        if self.cipher is None:
            raise RuntimeError('Failed to initialize AES-GCM cryptographic engine!')

    def _increment_nonce(self) -> None:
        """Increment the last 4 bytes of nonce (big-endian counter)."""
        if self.sec_patch_ver == 1:
            counter = struct.unpack('>I', self.nonce[8:])[0]  # Read last 4 bytes as big-endian integer
            counter += 1  # Increment counter
            if counter > 0xFFFFFFFF:  # Check for overflow
                raise RuntimeError('Nonce counter overflow')
            self.nonce[8:] = struct.pack('>I', counter)  # Store back as big-endian

    def encrypt_data(self, data: bytes) -> Any:
        self._print_verbose(f'Nonce:\t0x{self.nonce.hex()}')
        ciphertext = self.cipher.encrypt(self.nonce, data, None)
        self._increment_nonce()
        return ciphertext

    def decrypt_data(self, data: bytes) -> Any:
        self._print_verbose(f'Nonce:\t0x{self.nonce.hex()}')
        plaintext = self.cipher.decrypt(self.nonce, data, None)
        self._increment_nonce()
        return plaintext
