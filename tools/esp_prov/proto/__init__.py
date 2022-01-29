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

import importlib.util
import os
import sys
from importlib.abc import Loader
from typing import Any


def _load_source(name, path):  # type: (str, str) -> Any
    spec = importlib.util.spec_from_file_location(name, path)
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    assert isinstance(spec.loader, Loader)
    spec.loader.exec_module(module)
    return module


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

# custom_provisioning component related python files generated from .proto files
custom_config_pb2  = _load_source('custom_config_pb2',  idf_path +
                                  '/examples/provisioning/legacy/custom_config/components/custom_provisioning/python/custom_config_pb2.py')
