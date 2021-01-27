from __future__ import unicode_literals

import glob
import json
import os
import re
import threading

import ttfw_idf
from SimpleWebSocketServer import SimpleWebSocketServer, WebSocket
from tiny_test_fw import Utility


class IDEWSProtocol(WebSocket):

    def handleMessage(self):
        try:
            j = json.loads(self.data)
        except Exception as e:
            Utility.console_log('Server ignores error: {}'.format(e), 'orange')
            return
        event = j.get('event')
        if event and 'prog' in j and ((event == 'gdb_stub' and 'port' in j) or
                                      (event == 'coredump' and 'file' in j)):
            payload = {'event': 'debug_finished'}
            self.sendMessage(json.dumps(payload))
            Utility.console_log('Server sent: {}'.format(payload))
        else:
            Utility.console_log('Server received: {}'.format(j), 'orange')

    def handleConnected(self):
        Utility.console_log('{} connected to server'.format(self.address))

    def handleClose(self):
        Utility.console_log('{} closed the connection'.format(self.address))


class WebSocketServer(object):
    HOST = '127.0.0.1'
    PORT = 1123

    def run(self):
        server = SimpleWebSocketServer(self.HOST, self.PORT, IDEWSProtocol)
        while not self.exit_event.is_set():
            server.serveonce()

    def __init__(self):
        self.exit_event = threading.Event()
        self.thread = threading.Thread(target=self.run)
        self.thread.start()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.exit_event.set()
        self.thread.join(10)
        if self.thread.is_alive():
            Utility.console_log('Thread cannot be joined', 'orange')


@ttfw_idf.idf_custom_test(env_tag='test_jtag_arm', group='test-apps')
def test_monitor_ide_integration(env, extra_data):
    config_files = glob.glob(os.path.join(os.path.dirname(__file__), 'sdkconfig.ci.*'))
    config_names = [os.path.basename(s).replace('sdkconfig.ci.', '') for s in config_files]
    rel_proj_path = 'tools/test_apps/system/monitor_ide_integration'
    for name in config_names:
        Utility.console_log('Checking config "{}"... '.format(name), 'green', end='')
        dut = env.get_dut('panic', rel_proj_path, app_config_name=name)
        monitor_path = os.path.join(dut.app.idf_path, 'tools/idf_monitor.py')
        elf_path = os.path.join(dut.app.binary_path, 'panic.elf')
        dut.start_app()
        # Closing the DUT because we will reconnect with IDF Monitor
        env.close_dut(dut.name)

        with WebSocketServer(), ttfw_idf.CustomProcess(' '.join([monitor_path,
                                                                 elf_path,
                                                                 '--port', str(dut.port),
                                                                 '--ws', 'ws://{}:{}'.format(WebSocketServer.HOST,
                                                                                             WebSocketServer.PORT)]),
                                                       logfile='monitor_{}.log'.format(name)) as p:
            p.pexpect_proc.expect(re.compile(r'Guru Meditation Error'), timeout=10)
            p.pexpect_proc.expect_exact('Communicating through WebSocket', timeout=5)
            # "u?" is for Python 2 only in the following regular expressions.
            # The elements of dictionary can be printed in different order depending on the Python version.
            p.pexpect_proc.expect(re.compile(r"WebSocket sent: \{u?.*'event': u?'" + name + "'"), timeout=5)
            p.pexpect_proc.expect_exact('Waiting for debug finished event', timeout=5)
            p.pexpect_proc.expect(re.compile(r"WebSocket received: \{u?'event': u?'debug_finished'\}"), timeout=5)
            p.pexpect_proc.expect_exact('Communications through WebSocket is finished', timeout=5)


if __name__ == '__main__':
    test_monitor_ide_integration()
