# SPDX-FileCopyrightText: 2018-2023 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#
import socket
from http.client import HTTPConnection, HTTPSConnection

from utils import str_to_bytes

from .transport import Transport


class Transport_HTTP(Transport):
    def __init__(self, hostname, ssl_context=None):
        try:
            socket.gethostbyname(hostname.split(':')[0])
        except socket.gaierror:
            raise RuntimeError(f'Unable to resolve hostname: {hostname}')

        if ssl_context is None:
            self.conn = HTTPConnection(hostname, timeout=60)
        else:
            self.conn = HTTPSConnection(hostname, context=ssl_context, timeout=60)
        try:
            print(f'++++ Connecting to {hostname}++++')
            self.conn.connect()
        except Exception as err:
            raise RuntimeError('Connection Failure : ' + str(err))
        self.headers = {'Content-type': 'application/x-www-form-urlencoded','Accept': 'text/plain'}

    def _send_post_request(self, path, data):
        data = str_to_bytes(data) if isinstance(data, str) else data
        try:
            self.conn.request('POST', path, data, self.headers)
            response = self.conn.getresponse()
            # While establishing a session, the device sends the Set-Cookie header
            # with value 'session=cookie_session_id' in its first response of the session to the tool.
            # To maintain the same session, successive requests from the tool should include
            # an additional 'Cookie' header with the above recieved value.
            for hdr_key, hdr_val in response.getheaders():
                if hdr_key == 'Set-Cookie':
                    self.headers['Cookie'] = hdr_val
            if response.status == 200:
                return response.read().decode('latin-1')
        except Exception as err:
            raise RuntimeError('Connection Failure : ' + str(err))
        raise RuntimeError('Server responded with error code ' + str(response.status))

    async def send_data(self, ep_name, data):
        return self._send_post_request('/' + ep_name, data)
