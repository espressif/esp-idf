from __future__ import print_function, unicode_literals

import os
import random
import re
import socket
import string
from threading import Event, Thread

import ttfw_idf
from SimpleWebSocketServer import SimpleWebSocketServer, WebSocket
from tiny_test_fw import Utility


def get_my_ip():
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    try:
        # doesn't even have to be reachable
        s.connect(('10.255.255.255', 1))
        IP = s.getsockname()[0]
    except Exception:
        IP = '127.0.0.1'
    finally:
        s.close()
    return IP


class TestEcho(WebSocket):

    def handleMessage(self):
        self.sendMessage(self.data)
        print('Server sent: {}'.format(self.data))

    def handleConnected(self):
        print('Connection from: {}'.format(self.address))

    def handleClose(self):
        print('{} closed the connection'.format(self.address))


# Simple Websocket server for testing purposes
class Websocket(object):

    def send_data(self, data):
        for nr, conn in self.server.connections.items():
            conn.sendMessage(data)

    def run(self):
        self.server = SimpleWebSocketServer('', self.port, TestEcho)
        while not self.exit_event.is_set():
            self.server.serveonce()

    def __init__(self, port):
        self.port = port
        self.exit_event = Event()
        self.thread = Thread(target=self.run)
        self.thread.start()

    def __enter__(self):
        return self

    def __exit__(self, exc_type, exc_value, traceback):
        self.exit_event.set()
        self.thread.join(10)
        if self.thread.is_alive():
            Utility.console_log('Thread cannot be joined', 'orange')


def test_echo(dut):
    dut.expect('WEBSOCKET_EVENT_CONNECTED')
    for i in range(0, 5):
        dut.expect(re.compile(r'Received=hello (\d)'), timeout=30)
    print('All echos received')


def test_close(dut):
    code = dut.expect(re.compile(r'WEBSOCKET: Received closed message with code=(\d*)'), timeout=60)[0]
    print('Received close frame with code {}'.format(code))


def test_recv_long_msg(dut, websocket, msg_len, repeats):
    send_msg = ''.join(random.choice(string.ascii_uppercase + string.ascii_lowercase + string.digits) for _ in range(msg_len))

    for _ in range(repeats):
        websocket.send_data(send_msg)

        recv_msg = ''
        while len(recv_msg) < msg_len:
            # Filter out color encoding
            match = dut.expect(re.compile(r'Received=([a-zA-Z0-9]*).*\n'), timeout=30)[0]
            recv_msg += match

        if recv_msg == send_msg:
            print('Sent message and received message are equal')
        else:
            raise ValueError('DUT received string do not match sent string, \nexpected: {}\nwith length {}\
                            \nreceived: {}\nwith length {}'.format(send_msg, len(send_msg), recv_msg, len(recv_msg)))


@ttfw_idf.idf_example_test(env_tag='Example_EthKitV1')
def test_examples_protocol_websocket(env, extra_data):
    """
    steps:
      1. obtain IP address
      2. connect to uri specified in the config
      3. send and receive data
    """
    dut1 = env.get_dut('websocket', 'examples/protocols/websocket', dut_class=ttfw_idf.ESP32DUT)
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, 'websocket_example.bin')
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance('websocket_bin_size', '{}KB'.format(bin_size // 1024))

    try:
        if 'CONFIG_WEBSOCKET_URI_FROM_STDIN' in dut1.app.get_sdkconfig():
            uri_from_stdin = True
        else:
            uri = dut1.app.get_sdkconfig()['CONFIG_WEBSOCKET_URI'].strip('"')
            uri_from_stdin = False

    except Exception:
        print('ENV_TEST_FAILURE: Cannot find uri settings in sdkconfig')
        raise

    # start test
    dut1.start_app()

    if uri_from_stdin:
        server_port = 4455
        with Websocket(server_port) as ws:
            uri = 'ws://{}:{}'.format(get_my_ip(), server_port)
            print('DUT connecting to {}'.format(uri))
            dut1.expect('Please enter uri of websocket endpoint', timeout=30)
            dut1.write(uri)
            test_echo(dut1)
            # Message length should exceed DUT's buffer size to test fragmentation, default is 1024 byte
            test_recv_long_msg(dut1, ws, 2000, 3)
            test_close(dut1)

    else:
        print('DUT connecting to {}'.format(uri))
        test_echo(dut1)


if __name__ == '__main__':
    test_examples_protocol_websocket()
