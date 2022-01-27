# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

# APIs for interpreting and creating protobuf packets for `custom-config` protocomm endpoint

from __future__ import print_function

import utils
from future.utils import tobytes


def print_verbose(security_ctx, data):
    if (security_ctx.verbose):
        print('++++ ' + data + ' ++++')


def custom_data_request(security_ctx, data):
    # Encrypt the custom data
    enc_cmd = security_ctx.encrypt_data(tobytes(data))
    print_verbose(security_ctx, 'Client -> Device (CustomData cmd) ' + utils.str_to_hexstr(enc_cmd))
    return enc_cmd


def custom_data_response(security_ctx, response_data):
    # Decrypt response packet
    decrypt = security_ctx.decrypt_data(tobytes(response_data))
    print('CustomData response: ' + str(decrypt))
    return 0
