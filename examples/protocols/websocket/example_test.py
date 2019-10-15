from __future__ import print_function
from __future__ import unicode_literals
import re
import os
import socket
import hashlib
import base64
from threading import Thread

import ttfw_idf


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


# Simple Websocket server for testing purposes
class Websocket:
    HEADER_LEN = 6

    def __init__(self, port):
        self.port = port
        self.socket = socket.socket()
        self.socket.settimeout(10.0)

    def __enter__(self):
        try:
            self.socket.bind(('', self.port))
        except socket.error as e:
            print("Bind failed:{}".format(e))
            raise

        self.socket.listen(1)
        self.server_thread = Thread(target=self.run_server)
        self.server_thread.start()

    def __exit__(self, exc_type, exc_value, traceback):
        self.server_thread.join()
        self.socket.close()
        self.conn.close()

    def run_server(self):
        self.conn, address = self.socket.accept()  # accept new connection
        self.conn.settimeout(10.0)
        print("Connection from: {}".format(address))

        self.establish_connection()

        # Echo data until client closes connection
        self.echo_data()

    def establish_connection(self):
        while True:
            try:
                # receive data stream. it won't accept data packet greater than 1024 bytes
                data = self.conn.recv(1024).decode()
                if not data:
                    # exit if data is not received
                    raise

                if "Upgrade: websocket" in data and "Connection: Upgrade" in data:
                    self.handshake(data)
                    return
            except socket.error as err:
                print("Unable to establish a websocket connection: {}, {}".format(err))
                raise

    def handshake(self, data):
        # Magic string from RFC
        MAGIC_STRING = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11"
        headers = data.split("\r\n")

        for header in headers:
            if "Sec-WebSocket-Key" in header:
                client_key = header.split()[1]

        if client_key:
            resp_key = client_key + MAGIC_STRING
            resp_key = base64.standard_b64encode(hashlib.sha1(resp_key.encode()).digest())

            resp = "HTTP/1.1 101 Switching Protocols\r\n" + \
                "Upgrade: websocket\r\n" + \
                "Connection: Upgrade\r\n" + \
                "Sec-WebSocket-Accept: {}\r\n\r\n".format(resp_key.decode())

            self.conn.send(resp.encode())

    def echo_data(self):
        while(True):
            try:
                header = bytearray(self.conn.recv(self.HEADER_LEN, socket.MSG_WAITALL))
                if not header:
                    # exit if data is not received
                    return

                # Remove mask bit
                payload_len = ~(1 << 7) & header[1]

                payload = bytearray(self.conn.recv(payload_len, socket.MSG_WAITALL))
                frame = header + payload

                decoded_payload = self.decode_frame(frame)

                echo_frame = self.encode_frame(decoded_payload)
                self.conn.send(echo_frame)
            except socket.error as err:
                print("Stopped echoing data: {}".format(err))

    def decode_frame(self, frame):
        # Mask out MASK bit from payload length, this len is only valid for short messages (<126)
        payload_len = ~(1 << 7) & frame[1]

        mask = frame[2:self.HEADER_LEN]

        encrypted_payload = frame[self.HEADER_LEN:self.HEADER_LEN + payload_len]
        payload = bytearray()

        for i in range(payload_len):
            payload.append(encrypted_payload[i] ^ mask[i % 4])

        return payload

    def encode_frame(self, payload):
        # Set FIN = 1 and OP_CODE = 1 (text)
        header = (1 << 7) | (1 << 0)

        frame = bytearray(header)
        frame.append(len(payload))
        frame += payload

        return frame


def test_echo(dut):
    dut.expect("WEBSOCKET_EVENT_CONNECTED")
    for i in range(0, 10):
        dut.expect(re.compile(r"Received=hello (\d)"))
    dut.expect("Websocket Stopped")


@ttfw_idf.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_websocket(env, extra_data):
    """
    steps:
      1. join AP
      2. connect to uri specified in the config
      3. send and receive data
    """
    dut1 = env.get_dut("websocket", "examples/protocols/websocket")
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, "websocket-example.bin")
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance("websocket_bin_size", "{}KB".format(bin_size // 1024))
    ttfw_idf.check_performance("websocket_bin_size", bin_size // 1024)

    try:
        if "CONFIG_WEBSOCKET_URI_FROM_STDIN" in dut1.app.get_sdkconfig():
            uri_from_stdin = True
        else:
            uri = dut1.app.get_sdkconfig()["CONFIG_WEBSOCKET_URI"].strip('"')
            uri_from_stdin = False

    except Exception:
        print('ENV_TEST_FAILURE: Cannot find uri settings in sdkconfig')
        raise

    # start test
    dut1.start_app()

    if uri_from_stdin:
        server_port = 4455
        with Websocket(server_port):
            uri = "ws://{}:{}".format(get_my_ip(), server_port)
            print("DUT connecting to {}".format(uri))
            dut1.expect("Please enter uri of websocket endpoint", timeout=30)
            dut1.write(uri)
            test_echo(dut1)

    else:
        print("DUT connecting to {}".format(uri))
        test_echo(dut1)


if __name__ == '__main__':
    test_examples_protocol_websocket()
