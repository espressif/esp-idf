# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

# APIs for interpreting and creating protobuf packets for `custom-config` protocomm endpoint

from utils import str_to_bytes


def print_verbose(security_ctx, data):
    if (security_ctx.verbose):
        print(f'\x1b[32;20m++++ {data} ++++\x1b[0m')


def custom_data_request(security_ctx, data):
    # Encrypt the custom data
    enc_cmd = security_ctx.encrypt_data(str_to_bytes(data))
    print_verbose(security_ctx, f'Client -> Device (CustomData cmd): 0x{enc_cmd.hex()}')
    return enc_cmd.decode('latin-1')


def custom_data_response(security_ctx, response_data):
    # Decrypt response packet
    decrypt = security_ctx.decrypt_data(str_to_bytes(response_data))
    print(f'++++ CustomData response: {str(decrypt)}++++')
    return 0
