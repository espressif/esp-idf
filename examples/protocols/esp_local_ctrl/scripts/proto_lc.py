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


from __future__ import print_function
from future.utils import tobytes
import os


def _load_source(name, path):
    try:
        from importlib.machinery import SourceFileLoader
        return SourceFileLoader(name, path).load_module()
    except ImportError:
        # importlib.machinery doesn't exists in Python 2 so we will use imp (deprecated in Python 3)
        import imp
        return imp.load_source(name, path)


idf_path = os.environ['IDF_PATH']
constants_pb2 = _load_source("constants_pb2", idf_path + "/components/protocomm/python/constants_pb2.py")
local_ctrl_pb2 = _load_source("esp_local_ctrl_pb2", idf_path + "/components/esp_local_ctrl/python/esp_local_ctrl_pb2.py")


def get_prop_count_request(security_ctx):
    req = local_ctrl_pb2.LocalCtrlMessage()
    req.msg = local_ctrl_pb2.TypeCmdGetPropertyCount
    payload = local_ctrl_pb2.CmdGetPropertyCount()
    req.cmd_get_prop_count.MergeFrom(payload)
    enc_cmd = security_ctx.encrypt_data(req.SerializeToString())
    return enc_cmd


def get_prop_count_response(security_ctx, response_data):
    decrypt = security_ctx.decrypt_data(tobytes(response_data))
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
    return enc_cmd


def get_prop_vals_response(security_ctx, response_data):
    decrypt = security_ctx.decrypt_data(tobytes(response_data))
    resp = local_ctrl_pb2.LocalCtrlMessage()
    resp.ParseFromString(decrypt)
    results = []
    if (resp.resp_get_prop_vals.status == 0):
        for prop in resp.resp_get_prop_vals.props:
            results += [{
                "name": prop.name,
                "type": prop.type,
                "flags": prop.flags,
                "value": tobytes(prop.value)
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
    return enc_cmd


def set_prop_vals_response(security_ctx, response_data):
    decrypt = security_ctx.decrypt_data(tobytes(response_data))
    resp = local_ctrl_pb2.LocalCtrlMessage()
    resp.ParseFromString(decrypt)
    return (resp.resp_set_prop_vals.status == 0)
