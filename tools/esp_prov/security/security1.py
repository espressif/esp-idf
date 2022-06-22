# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

# APIs for interpreting and creating protobuf packets for
# protocomm endpoint with security type protocomm_security1

import proto
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes, serialization
from cryptography.hazmat.primitives.asymmetric.x25519 import X25519PrivateKey, X25519PublicKey
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from utils import long_to_bytes, str_to_bytes

from .security import Security


def a_xor_b(a: bytes, b: bytes) -> bytes:
    return b''.join(long_to_bytes(a[i] ^ b[i]) for i in range(0, len(b)))


# Enum for state of protocomm_security1 FSM
class security_state:
    REQUEST1 = 0
    RESPONSE1_REQUEST2 = 1
    RESPONSE2 = 2
    FINISHED = 3


class Security1(Security):
    def __init__(self, pop, verbose):
        # Initialize state of the security1 FSM
        self.session_state = security_state.REQUEST1
        self.pop = str_to_bytes(pop)
        self.verbose = verbose
        Security.__init__(self, self.security1_session)

    def security1_session(self, response_data):
        # protocomm security1 FSM which interprets/forms
        # protobuf packets according to present state of session
        if (self.session_state == security_state.REQUEST1):
            self.session_state = security_state.RESPONSE1_REQUEST2
            return self.setup0_request()
        elif (self.session_state == security_state.RESPONSE1_REQUEST2):
            self.session_state = security_state.RESPONSE2
            self.setup0_response(response_data)
            return self.setup1_request()
        elif (self.session_state == security_state.RESPONSE2):
            self.session_state = security_state.FINISHED
            self.setup1_response(response_data)
            return None

        print('Unexpected state')
        return None

    def __generate_key(self):
        # Generate private and public key pair for client
        self.client_private_key = X25519PrivateKey.generate()
        self.client_public_key = self.client_private_key.public_key().public_bytes(
            encoding=serialization.Encoding.Raw,
            format=serialization.PublicFormat.Raw)

    def _print_verbose(self, data):
        if (self.verbose):
            print(f'\x1b[32;20m++++ {data} ++++\x1b[0m')

    def setup0_request(self):
        # Form SessionCmd0 request packet using client public key
        setup_req = proto.session_pb2.SessionData()
        setup_req.sec_ver = proto.session_pb2.SecScheme1
        self.__generate_key()
        setup_req.sec1.sc0.client_pubkey = self.client_public_key
        self._print_verbose(f'Client Public Key:\t0x{self.client_public_key.hex()}')
        return setup_req.SerializeToString().decode('latin-1')

    def setup0_response(self, response_data):
        # Interpret SessionResp0 response packet
        setup_resp = proto.session_pb2.SessionData()
        setup_resp.ParseFromString(str_to_bytes(response_data))
        self._print_verbose('Security version:\t' + str(setup_resp.sec_ver))
        if setup_resp.sec_ver != proto.session_pb2.SecScheme1:
            raise RuntimeError('Incorrect security scheme')

        self.device_public_key = setup_resp.sec1.sr0.device_pubkey
        # Device random is the initialization vector
        device_random = setup_resp.sec1.sr0.device_random
        self._print_verbose(f'Device Public Key:\t0x{self.device_public_key.hex()}')
        self._print_verbose(f'Device Random:\t0x{device_random.hex()}')

        # Calculate Curve25519 shared key using Client private key and Device public key
        sharedK = self.client_private_key.exchange(X25519PublicKey.from_public_bytes(self.device_public_key))
        self._print_verbose(f'Shared Key:\t0x{sharedK.hex()}')

        # If PoP is provided, XOR SHA256 of PoP with the previously
        # calculated Shared Key to form the actual Shared Key
        if len(self.pop) > 0:
            # Calculate SHA256 of PoP
            h = hashes.Hash(hashes.SHA256(), backend=default_backend())
            h.update(self.pop)
            digest = h.finalize()
            # XOR with and update Shared Key
            sharedK = a_xor_b(sharedK, digest)
            self._print_verbose(f'Updated Shared Key (Shared key XORed with PoP):\t0x{sharedK.hex()}')
        # Initialize the encryption engine with Shared Key and initialization vector
        cipher = Cipher(algorithms.AES(sharedK), modes.CTR(device_random), backend=default_backend())
        self.cipher = cipher.encryptor()

    def setup1_request(self):
        # Form SessionCmd1 request packet using encrypted device public key
        setup_req = proto.session_pb2.SessionData()
        setup_req.sec_ver = proto.session_pb2.SecScheme1
        setup_req.sec1.msg = proto.sec1_pb2.Session_Command1
        # Encrypt device public key and attach to the request packet
        client_verify = self.cipher.update(self.device_public_key)
        self._print_verbose(f'Client Proof:\t0x{client_verify.hex()}')
        setup_req.sec1.sc1.client_verify_data = client_verify
        return setup_req.SerializeToString().decode('latin-1')

    def setup1_response(self, response_data):
        # Interpret SessionResp1 response packet
        setup_resp = proto.session_pb2.SessionData()
        setup_resp.ParseFromString(str_to_bytes(response_data))
        # Ensure security scheme matches
        if setup_resp.sec_ver == proto.session_pb2.SecScheme1:
            # Read encrypyed device verify string
            device_verify = setup_resp.sec1.sr1.device_verify_data
            self._print_verbose(f'Device Proof:\t0x{device_verify.hex()}')
            # Decrypt the device verify string
            enc_client_pubkey = self.cipher.update(setup_resp.sec1.sr1.device_verify_data)
            # Match decryped string with client public key
            if enc_client_pubkey != self.client_public_key:
                raise RuntimeError('Failed to verify device!')
        else:
            raise RuntimeError('Unsupported security protocol')

    def encrypt_data(self, data):
        return self.cipher.update(data)

    def decrypt_data(self, data):
        return self.cipher.update(data)
