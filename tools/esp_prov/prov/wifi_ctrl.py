# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

# APIs for interpreting and creating protobuf packets for Wi-Fi State Controlling

import proto
from utils import str_to_bytes


def print_verbose(security_ctx, data):
    if (security_ctx.verbose):
        print(f'\x1b[32;20m++++ {data} ++++\x1b[0m')


def ctrl_reset_request(security_ctx):
    # Form protobuf request packet for CtrlReset command
    cmd = proto.wifi_ctrl_pb2.WiFiCtrlPayload()
    cmd.msg = proto.wifi_ctrl_pb2.TypeCmdCtrlReset
    enc_cmd = security_ctx.encrypt_data(cmd.SerializeToString())
    print_verbose(security_ctx, f'Client -> Device (Encrypted CmdCtrlReset): 0x{enc_cmd.hex()}')
    return enc_cmd.decode('latin-1')


def ctrl_reset_response(security_ctx, response_data):
    # Interpret protobuf response packet from CtrlReset command
    dec_resp = security_ctx.decrypt_data(str_to_bytes(response_data))
    resp = proto.wifi_ctrl_pb2.WiFiCtrlPayload()
    resp.ParseFromString(dec_resp)
    print_verbose(security_ctx, f'CtrlReset status: 0x{str(resp.status)}')
    if resp.status != 0:
        raise RuntimeError


def ctrl_reprov_request(security_ctx):
    # Form protobuf request packet for CtrlReprov command
    cmd = proto.wifi_ctrl_pb2.WiFiCtrlPayload()
    cmd.msg = proto.wifi_ctrl_pb2.TypeCmdCtrlReprov
    enc_cmd = security_ctx.encrypt_data(cmd.SerializeToString())
    print_verbose(security_ctx, f'Client -> Device (Encrypted CmdCtrlReset): 0x{enc_cmd.hex()}')
    return enc_cmd.decode('latin-1')


def ctrl_reprov_response(security_ctx, response_data):
    # Interpret protobuf response packet from CtrlReprov command
    dec_resp = security_ctx.decrypt_data(str_to_bytes(response_data))
    resp = proto.wifi_ctrl_pb2.WiFiCtrlPayload()
    resp.ParseFromString(dec_resp)
    print_verbose(security_ctx, f'CtrlReset status: 0x{str(resp.status)}')
    if resp.status != 0:
        raise RuntimeError
