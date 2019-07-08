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

import socket
import http.client
import ssl

from .transport import Transport


class Transport_HTTP(Transport):
    def __init__(self, hostname, certfile=None):
        try:
            socket.gethostbyname(hostname.split(':')[0])
        except socket.gaierror:
            raise RuntimeError("Unable to resolve hostname :" + hostname)

        if certfile is None:
            self.conn = http.client.HTTPConnection(hostname, timeout=30)
        else:
            ssl_ctx = ssl.create_default_context(cafile=certfile)
            self.conn = http.client.HTTPSConnection(hostname, context=ssl_ctx, timeout=30)
        try:
            print("Connecting to " + hostname)
            self.conn.connect()
        except Exception as err:
            raise RuntimeError("Connection Failure : " + str(err))
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
        return self._send_post_request('/' + ep_name, data)
