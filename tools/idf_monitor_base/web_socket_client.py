# Copyright 2015-2021 Espressif Systems (Shanghai) CO LTD
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

import json
import time

from .output_helpers import red_print, yellow_print

try:
    import websocket
except ImportError:
    # This is needed for IDE integration only.
    pass


class WebSocketClient(object):
    """
    WebSocket client used to advertise debug events to WebSocket server by sending and receiving JSON-serialized
    dictionaries.

    Advertisement of debug event:
    {'event': 'gdb_stub', 'port': '/dev/ttyUSB1', 'prog': 'build/elf_file'} for GDB Stub, or
    {'event': 'coredump', 'file': '/tmp/xy', 'prog': 'build/elf_file'} for coredump,
    where 'port' is the port for the connected device, 'prog' is the full path to the ELF file and 'file' is the
    generated coredump file.

    Expected end of external debugging:
    {'event': 'debug_finished'}
    """

    RETRIES = 3
    CONNECTION_RETRY_DELAY = 1

    def __init__(self, url):  # type: (str) -> None
        self.url = url
        self._connect()

    def _connect(self):  # type: () -> None
        """
        Connect to WebSocket server at url
        """

        self.close()
        for _ in range(self.RETRIES):
            try:
                self.ws = websocket.create_connection(self.url)
                break  # success
            except NameError:
                raise RuntimeError('Please install the websocket_client package for IDE integration!')
            except Exception as e:  # noqa
                red_print('WebSocket connection error: {}'.format(e))
            time.sleep(self.CONNECTION_RETRY_DELAY)
        else:
            raise RuntimeError('Cannot connect to WebSocket server')

    def close(self):  # type: () -> None
        try:
            self.ws.close()
        except AttributeError:
            # Not yet connected
            pass
        except Exception as e:  # noqa
            red_print('WebSocket close error: {}'.format(e))

    def send(self, payload_dict):  # type: (dict) -> None
        """
        Serialize payload_dict in JSON format and send it to the server
        """
        for _ in range(self.RETRIES):
            try:
                self.ws.send(json.dumps(payload_dict))
                yellow_print('WebSocket sent: {}'.format(payload_dict))
                break
            except Exception as e:  # noqa
                red_print('WebSocket send error: {}'.format(e))
                self._connect()
        else:
            raise RuntimeError('Cannot send to WebSocket server')

    def wait(self, expect_iterable):  # type: (list) -> None
        """
        Wait until a dictionary in JSON format is received from the server with all (key, value) tuples from
        expect_iterable.
        """
        for _ in range(self.RETRIES):
            try:
                r = self.ws.recv()
            except Exception as e:
                red_print('WebSocket receive error: {}'.format(e))
                self._connect()
                continue
            obj = json.loads(r)
            if all([k in obj and obj[k] == v for k, v in expect_iterable]):
                yellow_print('WebSocket received: {}'.format(obj))
                break
            red_print('WebSocket expected: {}, received: {}'.format(dict(expect_iterable), obj))
        else:
            raise RuntimeError('Cannot receive from WebSocket server')
