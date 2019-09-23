# Copyright 2018 Espressif Systems (Shanghai) PTE LTD
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# APIs for interpreting and creating protobuf packets for
# protocomm endpoint with security type protocomm_security1

from __future__ import print_function
from future.utils import tobytes

import utils
import proto
from .security import Security

from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.asymmetric.x25519 import X25519PrivateKey, X25519PublicKey
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes

import session_pb2


# Enum for state of protocomm_security1 FSM
class security_state:
    REQUEST1 = 0
    RESPONSE1_REQUEST2 = 1
    RESPONSE2 = 2
    FINISHED = 3


def xor(a, b):
    # XOR two inputs of type `bytes`
    ret = bytearray()
    # Decode the input bytes to strings
    a = a.decode('latin-1')
    b = b.decode('latin-1')
    for i in range(max(len(a), len(b))):
        # Convert the characters to corresponding 8-bit ASCII codes
        # then XOR them and store in bytearray
        ret.append(([0, ord(a[i])][i < len(a)]) ^ ([0, ord(b[i])][i < len(b)]))
    # Convert bytearray to bytes
    return bytes(ret)


class Security1(Security):
    def __init__(self, pop, verbose):
        # Initialize state of the security1 FSM
        self.session_state = security_state.REQUEST1
        self.pop = tobytes(pop)
        self.verbose = verbose
        Security.__init__(self, self.security1_session)

    def security1_session(self, response_data):
        # protocomm security1 FSM which interprets/forms
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
        else:
            print("Unexpected state")
            return None

    def __generate_key(self):
        # Generate private and public key pair for client
        self.client_private_key = X25519PrivateKey.generate()
        self.client_public_key  = self.client_private_key.public_key().public_bytes()

    def _print_verbose(self, data):
        if (self.verbose):
            print("++++ " + data + " ++++")

    def setup0_request(self):
        # Form SessionCmd0 request packet using client public key
        setup_req = session_pb2.SessionData()
        setup_req.sec_ver = session_pb2.SecScheme1
        self.__generate_key()
        setup_req.sec1.sc0.client_pubkey = self.client_public_key
        self._print_verbose("Client Public Key:\t" + utils.str_to_hexstr(self.client_public_key.decode('latin-1')))
        return setup_req.SerializeToString().decode('latin-1')

    def setup0_response(self, response_data):
        # Interpret SessionResp0 response packet
        setup_resp = proto.session_pb2.SessionData()
        setup_resp.ParseFromString(tobytes(response_data))
        self._print_verbose("Security version:\t" + str(setup_resp.sec_ver))
        if setup_resp.sec_ver != session_pb2.SecScheme1:
            print("Incorrect sec scheme")
            exit(1)
        self.device_public_key = setup_resp.sec1.sr0.device_pubkey
        # Device random is the initialization vector
        device_random = setup_resp.sec1.sr0.device_random
        self._print_verbose("Device Public Key:\t" + utils.str_to_hexstr(self.device_public_key.decode('latin-1')))
        self._print_verbose("Device Random:\t" + utils.str_to_hexstr(device_random.decode('latin-1')))

        # Calculate Curve25519 shared key using Client private key and Device public key
        sharedK = self.client_private_key.exchange(X25519PublicKey.from_public_bytes(self.device_public_key))
        self._print_verbose("Shared Key:\t" + utils.str_to_hexstr(sharedK.decode('latin-1')))

        # If PoP is provided, XOR SHA256 of PoP with the previously
        # calculated Shared Key to form the actual Shared Key
        if len(self.pop) > 0:
            # Calculate SHA256 of PoP
            h = hashes.Hash(hashes.SHA256(), backend=default_backend())
            h.update(self.pop)
            digest = h.finalize()
            # XOR with and update Shared Key
            sharedK = xor(sharedK, digest)
            self._print_verbose("New Shared Key XORed with PoP:\t" + utils.str_to_hexstr(sharedK.decode('latin-1')))
        # Initialize the encryption engine with Shared Key and initialization vector
        cipher = Cipher(algorithms.AES(sharedK), modes.CTR(device_random), backend=default_backend())
        self.cipher = cipher.encryptor()

    def setup1_request(self):
        # Form SessionCmd1 request packet using encrypted device public key
        setup_req = proto.session_pb2.SessionData()
        setup_req.sec_ver = session_pb2.SecScheme1
        setup_req.sec1.msg = proto.sec1_pb2.Session_Command1
        # Encrypt device public key and attach to the request packet
        client_verify = self.cipher.update(self.device_public_key)
        self._print_verbose("Client Verify:\t" + utils.str_to_hexstr(client_verify.decode('latin-1')))
        setup_req.sec1.sc1.client_verify_data = client_verify
        return setup_req.SerializeToString().decode('latin-1')

    def setup1_response(self, response_data):
        # Interpret SessionResp1 response packet
        setup_resp = proto.session_pb2.SessionData()
        setup_resp.ParseFromString(tobytes(response_data))
        # Ensure security scheme matches
        if setup_resp.sec_ver == session_pb2.SecScheme1:
            # Read encrypyed device verify string
            device_verify = setup_resp.sec1.sr1.device_verify_data
            self._print_verbose("Device verify:\t" + utils.str_to_hexstr(device_verify.decode('latin-1')))
            # Decrypt the device verify string
            enc_client_pubkey = self.cipher.update(setup_resp.sec1.sr1.device_verify_data)
            self._print_verbose("Enc client pubkey:\t " + utils.str_to_hexstr(enc_client_pubkey.decode('latin-1')))
            # Match decryped string with client public key
            if enc_client_pubkey != self.client_public_key:
                print("Mismatch in device verify")
                return -2
        else:
            print("Unsupported security protocol")
            return -1

    def encrypt_data(self, data):
        return self.cipher.update(data)

    def decrypt_data(self, data):
        return self.cipher.update(data)
