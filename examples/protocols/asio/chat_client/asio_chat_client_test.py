import re
import os
import sys
import socket
from threading import Thread
import time

try:
    import IDF
except ImportError:
    # this is a test case write with tiny-test-fw.
    # to run test cases outside tiny-test-fw,
    # we need to set environment variable `TEST_FW_PATH`,
    # then get and insert `TEST_FW_PATH` to sys path before import FW module
    test_fw_path = os.getenv("TEST_FW_PATH")
    if test_fw_path and test_fw_path not in sys.path:
        sys.path.insert(0, test_fw_path)
    import IDF

global g_client_response
global g_msg_to_client

g_client_response = b""
g_msg_to_client = b"   3XYZ"


def get_my_ip():
    s1 = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s1.connect(("8.8.8.8", 80))
    my_ip = s1.getsockname()[0]
    s1.close()
    return my_ip


def chat_server_sketch(my_ip):
    global g_client_response
    print("Starting the server on {}".format(my_ip))
    port = 2222
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.settimeout(600)
    s.bind((my_ip, port))
    s.listen(1)
    q,addr = s.accept()
    print("connection accepted")
    q.settimeout(30)
    q.send(g_msg_to_client)
    data = q.recv(1024)
    # check if received initial empty message
    if (len(data) > 4):
        g_client_response = data
    else:
        g_client_response = q.recv(1024)
        print("received from client {}".format(g_client_response))
    s.close()
    print("server closed")


@IDF.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_asio_chat_client(env, extra_data):
    """
    steps: |
      1. Test to start simple tcp server
      2. `dut1` joins AP
      3. Test injects server IP to `dut1`via stdin
      4. Test evaluates `dut1` receives a message server placed
      5. Test injects a message to `dut1` to be sent as chat_client message
      6. Test evaluates received test message in host server
    """
    global g_client_response
    global g_msg_to_client
    test_msg = "ABC"
    dut1 = env.get_dut("chat_client", "examples/protocols/asio/chat_client")
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, "asio_chat_client.bin")
    bin_size = os.path.getsize(binary_file)
    IDF.log_performance("asio_chat_client_size", "{}KB".format(bin_size // 1024))
    IDF.check_performance("asio_chat_client_size", bin_size // 1024)
    # 1. start a tcp server on the host
    host_ip = get_my_ip()
    thread1 = Thread(target=chat_server_sketch, args=(host_ip,))
    thread1.start()
    # 2. start the dut test and wait till client gets IP address
    dut1.start_app()
    dut1.expect(re.compile(r" IPv4 address: ([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)"), timeout=30)
    # 3. send host's IP to the client i.e. the `dut1`
    dut1.write(host_ip)
    # 4. client `dut1` should receive a message
    dut1.expect(g_msg_to_client[4:].decode())  # Strip out the front 4 bytes of message len (see chat_message protocol)
    # 5. write test message from `dut1` chat_client to the server
    dut1.write(test_msg)
    while len(g_client_response) == 0:
        time.sleep(1)
    g_client_response = g_client_response.decode()
    print(g_client_response)
    # 6. evaluate host_server received this message
    if (g_client_response[4:7] == test_msg):
        print("PASS: Received correct message")
        pass
    else:
        print("Failure!")
        raise ValueError('Wrong data received from asi tcp server: {} (expected:{})'.format(g_client_response[4:7], test_msg))
    thread1.join()


if __name__ == '__main__':
    test_examples_protocol_asio_chat_client()
