# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

import os
from types import ModuleType


def _load_source(name, path):  # type: (str, str) -> ModuleType
    from importlib.machinery import SourceFileLoader
    return SourceFileLoader(name, path).load_module(name)


idf_path = os.environ['IDF_PATH']

# protocomm component related python files generated from .proto files
constants_pb2 = _load_source('constants_pb2', idf_path + '/components/protocomm/python/constants_pb2.py')
sec0_pb2      = _load_source('sec0_pb2',      idf_path + '/components/protocomm/python/sec0_pb2.py')
sec1_pb2      = _load_source('sec1_pb2',      idf_path + '/components/protocomm/python/sec1_pb2.py')
session_pb2   = _load_source('session_pb2',   idf_path + '/components/protocomm/python/session_pb2.py')

# wifi_provisioning component related python files generated from .proto files
wifi_constants_pb2 = _load_source('wifi_constants_pb2', idf_path + '/components/wifi_provisioning/python/wifi_constants_pb2.py')
wifi_config_pb2    = _load_source('wifi_config_pb2',    idf_path + '/components/wifi_provisioning/python/wifi_config_pb2.py')
wifi_scan_pb2      = _load_source('wifi_scan_pb2',      idf_path + '/components/wifi_provisioning/python/wifi_scan_pb2.py')
