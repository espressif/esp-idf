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
from builtins import input

import utils

from .transport import Transport


class Transport_Console(Transport):

    def send_data(self, path, data, session_id=0):
        print("Client->Device msg :", path, session_id, utils.str_to_hexstr(data))
        try:
            resp = input("Enter device->client msg : ")
        except Exception as err:
            print("error:", err)
            return None
        return utils.hexstr_to_str(resp)
