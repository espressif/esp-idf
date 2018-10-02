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

import http.client

from .transport import *

class Transport_Softap(Transport):
    def __init__(self, url):
        self.conn = http.client.HTTPConnection(url, timeout=30)
        self.headers = {"Content-type": "application/x-www-form-urlencoded","Accept": "text/plain"}

    def _send_post_request(self, path, data):
        try:
            self.conn.request("POST", path, tobytes(data), self.headers)
            response = self.conn.getresponse()
            if response.status == 200:
                return response.read().decode('latin-1')
        except Exception as err:
            raise RuntimeError("Connection Failure : " + str(err))
        raise RuntimeError("Server responded with error code " + str(response.status))

    def send_data(self, ep_name, data):
        return self._send_post_request('/'+ ep_name, data)
