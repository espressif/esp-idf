# SPDX-FileCopyrightText: 2018-2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Apache-2.0
#

from __future__ import print_function

import socket

from future.utils import tobytes

try:
    from http.client import HTTPConnection, HTTPSConnection
except ImportError:
    # Python 2 fallback
    from httplib import HTTPConnection, HTTPSConnection  # type: ignore

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
        try:
            self.conn.request('POST', path, tobytes(data), self.headers)
            response = self.conn.getresponse()
            if response.status == 200:
                return response.read().decode('latin-1')
        except Exception as err:
            raise RuntimeError('Connection Failure : ' + str(err))
        raise RuntimeError('Server responded with error code ' + str(response.status))

    async def send_data(self, ep_name, data):
        return self._send_post_request('/' + ep_name, data)
