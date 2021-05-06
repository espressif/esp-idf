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

# APIs for interpreting and creating protobuf packets for `custom-config` protocomm endpoint

from __future__ import print_function
from future.utils import tobytes

import utils
import proto


def print_verbose(security_ctx, data):
    if (security_ctx.verbose):
        print("++++ " + data + " ++++")


def custom_config_request(security_ctx, info, version):
    # Form protobuf request packet from custom-config data
    cmd = proto.custom_config_pb2.CustomConfigRequest()
    cmd.info = tobytes(info)
    cmd.version = version
    enc_cmd = security_ctx.encrypt_data(cmd.SerializeToString()).decode('latin-1')
    print_verbose(security_ctx, "Client -> Device (CustomConfig cmd) " + utils.str_to_hexstr(enc_cmd))
    return enc_cmd


def custom_config_response(security_ctx, response_data):
    # Interpret protobuf response packet
    decrypt = security_ctx.decrypt_data(tobytes(response_data))
    cmd_resp = proto.custom_config_pb2.CustomConfigResponse()
    cmd_resp.ParseFromString(decrypt)
    print_verbose(security_ctx, "CustomConfig status " + str(cmd_resp.status))
    return cmd_resp.status


def custom_data_request(security_ctx, data):
    # Encrypt the custom data
    enc_cmd = security_ctx.encrypt_data(tobytes(data))
    print_verbose(security_ctx, "Client -> Device (CustomData cmd) " + utils.str_to_hexstr(enc_cmd))
    return enc_cmd


def custom_data_response(security_ctx, response_data):
    # Decrypt response packet
    decrypt = security_ctx.decrypt_data(tobytes(response_data))
    print("CustomData response: " + str(decrypt))
    return 0
