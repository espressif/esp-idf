# SPDX-FileCopyrightText: 2018-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

import importlib.util
import os
import sys
from importlib.abc import Loader
from typing import Any


def _load_source(name: str, path: str) -> Any:
    spec = importlib.util.spec_from_file_location(name, path)
    if not spec:
        return None

    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    assert isinstance(spec.loader, Loader)
    spec.loader.exec_module(module)
    return module


idf_path = os.environ['IDF_PATH']
constants_pb2 = _load_source('constants_pb2', idf_path + '/components/protocomm/python/constants_pb2.py')
local_ctrl_pb2 = _load_source('esp_local_ctrl_pb2', idf_path + '/components/esp_local_ctrl/python/esp_local_ctrl_pb2.py')


def to_bytes(s: str) -> bytes:
    return bytes(s, encoding='latin-1')


def get_prop_count_request(security_ctx):
    req = local_ctrl_pb2.LocalCtrlMessage()
    req.msg = local_ctrl_pb2.TypeCmdGetPropertyCount
    payload = local_ctrl_pb2.CmdGetPropertyCount()
    req.cmd_get_prop_count.MergeFrom(payload)
    enc_cmd = security_ctx.encrypt_data(req.SerializeToString())
    return enc_cmd.decode('latin-1')


def get_prop_count_response(security_ctx, response_data):
    decrypt = security_ctx.decrypt_data(to_bytes(response_data))
    resp = local_ctrl_pb2.LocalCtrlMessage()
    resp.ParseFromString(decrypt)
    if (resp.resp_get_prop_count.status == 0):
        return resp.resp_get_prop_count.count
    else:
        return 0


def get_prop_vals_request(security_ctx, indices):
    req = local_ctrl_pb2.LocalCtrlMessage()
    req.msg = local_ctrl_pb2.TypeCmdGetPropertyValues
    payload = local_ctrl_pb2.CmdGetPropertyValues()
    payload.indices.extend(indices)
    req.cmd_get_prop_vals.MergeFrom(payload)
    enc_cmd = security_ctx.encrypt_data(req.SerializeToString())
    return enc_cmd.decode('latin-1')


def get_prop_vals_response(security_ctx, response_data):
    decrypt = security_ctx.decrypt_data(to_bytes(response_data))
    resp = local_ctrl_pb2.LocalCtrlMessage()
    resp.ParseFromString(decrypt)
    results = []
    if (resp.resp_get_prop_vals.status == 0):
        for prop in resp.resp_get_prop_vals.props:
            results += [{
                'name': prop.name,
                'type': prop.type,
                'flags': prop.flags,
                'value': prop.value
            }]
    return results


def set_prop_vals_request(security_ctx, indices, values):
    req = local_ctrl_pb2.LocalCtrlMessage()
    req.msg = local_ctrl_pb2.TypeCmdSetPropertyValues
    payload = local_ctrl_pb2.CmdSetPropertyValues()
    for i, v in zip(indices, values):
        prop = payload.props.add()
        prop.index = i
        prop.value = v
    req.cmd_set_prop_vals.MergeFrom(payload)
    enc_cmd = security_ctx.encrypt_data(req.SerializeToString())
    return enc_cmd.decode('latin-1')


def set_prop_vals_response(security_ctx, response_data):
    decrypt = security_ctx.decrypt_data(to_bytes(response_data))
    resp = local_ctrl_pb2.LocalCtrlMessage()
    resp.ParseFromString(decrypt)
    return (resp.resp_set_prop_vals.status == 0)
