import re
import os
import socket

import ttfw_idf


@ttfw_idf.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_asio_tcp_server(env, extra_data):
    """
    steps: |
      1. join AP
      2. Start server
      3. Test connects to server and sends a test message
      4. Test evaluates received test message from server
      5. Test evaluates received test message on server stdout
    """
    test_msg = b"echo message from client to server"
    dut1 = env.get_dut("tcp_echo_server", "examples/protocols/asio/tcp_echo_server", dut_class=ttfw_idf.ESP32DUT)
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, "asio_tcp_echo_server.bin")
    bin_size = os.path.getsize(binary_file)
    ttfw_idf.log_performance("asio_tcp_echo_server_bin_size", "{}KB".format(bin_size // 1024))
    ttfw_idf.check_performance("asio_tcp_echo_server_size", bin_size // 1024, dut1.TARGET)
    # 1. start test
    dut1.start_app()
    # 2. get the server IP address
    data = dut1.expect(re.compile(r" IPv4 address: ([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)"), timeout=30)
    # 3. create tcp client and connect to server
    dut1.expect('ASIO engine is up and running', timeout=1)
    cli = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    cli.settimeout(30)
    cli.connect((data[0], 2222))
    cli.send(test_msg)
    data = cli.recv(1024)
    # 4. check the message received back from the server
    if (data == test_msg):
        print("PASS: Received correct message")
        pass
    else:
        print("Failure!")
        raise ValueError('Wrong data received from asi tcp server: {} (expected:{})'.format(data, test_msg))
    # 5. check the client message appears also on server terminal
    dut1.expect(test_msg.decode())


if __name__ == '__main__':
    test_examples_protocol_asio_tcp_server()
