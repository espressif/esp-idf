# SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
# SPDX-License-Identifier: Unlicense OR CC0-1.0
import json
import logging
import multiprocessing
import os
import re
import sys
from types import TracebackType
from typing import Optional, Type, TypeVar

import pexpect
import pytest
from pytest_embedded import Dut
from SimpleWebSocketServer import SimpleWebSocketServer, WebSocket

WebSocketServerType = TypeVar('WebSocketServerType')


class IDEWSProtocol(WebSocket):

    def handleMessage(self) -> None:
        try:
            j = json.loads(self.data)
        except Exception as e:
            logging.info(f'Server ignores error: {e}')
            return
        event = j.get('event')
        if event and 'prog' in j and ((event == 'gdb_stub' and 'port' in j) or
                                      (event == 'coredump' and 'file' in j)):
            payload = {'event': 'debug_finished'}
            self.sendMessage(json.dumps(payload))
            logging.info(f'Server sent: {payload}')
        else:
            logging.info(f'Server received: {j}')

    def handleConnected(self) -> None:
        logging.info(f'{self.address} connected to server')

    def handleClose(self) -> None:
        logging.info(f'{self.address} closed the connection')


class WebSocketServer(object):
    HOST = '127.0.0.1'
    PORT = 1123

    def run(self) -> None:
        server = SimpleWebSocketServer(self.HOST, self.PORT, IDEWSProtocol)
        while not self.exit_event.is_set():
            server.serveonce()

    def __init__(self) -> None:
        self.exit_event = multiprocessing.Event()
        self.proc = multiprocessing.Process(target=self.run)
        self.proc.start()

    def __enter__(self: WebSocketServerType) -> WebSocketServerType:
        return self

    def __exit__(self, exc_type: Optional[Type[BaseException]], exc_value: Optional[BaseException], traceback:
                 Optional[TracebackType]) -> None:
        self.exit_event.set()
        self.proc.join(10)
        if self.proc.is_alive():
            logging.info('Process cannot be joined')


@pytest.mark.esp32
@pytest.mark.test_jtag_arm
@pytest.mark.parametrize('config', ['gdb_stub', 'coredump'], indirect=True)
def test_monitor_ide_integration(config: str, dut: Dut) -> None:
    # The port needs to be closed because idf_monitor.py will connect to it
    dut.serial.stop_redirect_thread()

    monitor_py = os.path.join(os.environ['IDF_PATH'], 'tools', 'idf_monitor.py')
    with open(f'monitor_{config}.log', 'w') as log:
        monitor_cmd = ' '.join([sys.executable, monitor_py, os.path.join(dut.app.binary_path, 'panic.elf'),
                                '--port', str(dut.serial.port),
                                '--ws', f'ws://{WebSocketServer.HOST}:{WebSocketServer.PORT}'])
        with WebSocketServer(), pexpect.spawn(monitor_cmd,
                                              logfile=log,
                                              timeout=60,
                                              encoding='utf-8',
                                              codec_errors='ignore') as p:
            p.expect(re.compile(r'Guru Meditation Error'), timeout=10)
            p.expect_exact('Communicating through WebSocket', timeout=5)
            # The elements of dictionary can be printed in different order depending on the Python version.
            p.expect(re.compile(r"WebSocket sent: \{.*'event': '" + config + "'"), timeout=5)
            p.expect_exact('Waiting for debug finished event', timeout=5)
            p.expect(re.compile(r"WebSocket received: \{'event': 'debug_finished'\}"), timeout=5)
            p.expect_exact('Communications through WebSocket is finished', timeout=5)
