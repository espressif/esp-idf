import re
import os
import sys
from socket import *


# this is a test case write with tiny-test-fw.
# to run test cases outside tiny-test-fw,
# we need to set environment variable `TEST_FW_PATH`,
# then get and insert `TEST_FW_PATH` to sys path before import FW module
test_fw_path = os.getenv("TEST_FW_PATH")
if test_fw_path and test_fw_path not in sys.path:
    sys.path.insert(0, test_fw_path)

import TinyFW
import IDF




@IDF.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_asio_chat_server(env, extra_data):
    """
    steps: |
      1. join AP
      2. Start server
      3. Test connects to server and sends a test message
      4. Test evaluates received test message from server
    """
    test_msg="   4ABC\n"
    dut1 = env.get_dut("chat_server", "examples/protocols/asio/chat_server")
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, "asio_chatserver.bin")
    bin_size = os.path.getsize(binary_file)
    IDF.log_performance("asio_chatserver_bin_size", "{}KB".format(bin_size//1024))
    IDF.check_performance("asio_chatserver_size", bin_size//1024)
    # 1. start test
    dut1.start_app()
    # 2. get the server IP address
    data = dut1.expect(re.compile(r" sta ip: ([^,]+),"))
    # 3. create tcp client and connect to server
    cli = socket(AF_INET,SOCK_STREAM)
    cli.connect((data[0],80))
    cli.send(test_msg)
    data = cli.recv(1024)
    # 4. check the message received back from the server
    if (data == test_msg):
        print("PASS: Received correct message {}".format(data))
        pass
    else:
        print("Failure!")
        raise ValueError('Wrong data received from asi tcp server: {} (expoected:{})'.format(data, test_msg))


if __name__ == '__main__':
    test_examples_protocol_asio_chat_server()
