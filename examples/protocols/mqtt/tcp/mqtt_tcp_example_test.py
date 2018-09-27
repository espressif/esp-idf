import re
import os
import sys
from socket import *
from threading import Thread
import struct
import time

msgid=-1

def get_my_ip():
    s1 = socket(AF_INET, SOCK_DGRAM)
    s1.connect(("8.8.8.8", 80))
    my_ip = s1.getsockname()[0]
    s1.close()
    return my_ip

def mqqt_server_sketch(my_ip, port):
    global msgid
    print("Starting the server on {}".format(my_ip))
    s = None
    try:
        s=socket(AF_INET, SOCK_STREAM)
        s.settimeout(60)
        s.bind((my_ip, port))
        s.listen(1)
        q,addr=s.accept()
        q.settimeout(30)
        print("connection accepted")
    except:
        print("Local server on {}:{} listening/accepting failure: {}"
                "Possibly check permissions or firewall settings"
                "to accept connections on this address".format(my_ip, port, sys.exc_info()[0]))
        raise
    data = q.recv(1024)
    # check if received initial empty message
    print("received from client {}".format(data))
    data = bytearray([0x20, 0x02, 0x00, 0x00])
    q.send(data)
    # try to receive qos1
    data = q.recv(1024)
    msgid = struct.unpack(">H", data[15:17])[0]
    print("received from client {}, msgid: {}".format(data, msgid))
    data = bytearray([0x40, 0x02, data[15], data[16]])
    q.send(data)
    time.sleep(5)
    s.close()
    print("server closed")

# this is a test case write with tiny-test-fw.
# to run test cases outside tiny-test-fw,
# we need to set environment variable `TEST_FW_PATH`,
# then get and insert `TEST_FW_PATH` to sys path before import FW module
test_fw_path = os.getenv("TEST_FW_PATH")
if test_fw_path and test_fw_path not in sys.path:
    sys.path.insert(0, test_fw_path)

import TinyFW
import IDF
import DUT




@IDF.idf_example_test(env_tag="Example_WIFI")
def test_examples_protocol_mqtt_qos1(env, extra_data):
    global msgid
    """
    steps: (QoS1: Happy flow)
      1. start the broker broker (with correctly sending ACK)
      2. DUT client connects to a broker and publishes qos1 message
      3. Test evaluates that qos1 message is queued and removed from queued after ACK received
      4. Test the broker received the same message id evaluated in step 3
    """
    dut1 = env.get_dut("mqtt_tcp", "examples/protocols/mqtt/tcp")
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, "mqtt_tcp.bin")
    bin_size = os.path.getsize(binary_file)
    IDF.log_performance("mqtt_tcp_bin_size", "{}KB".format(bin_size//1024))
    IDF.check_performance("mqtt_tcp_size", bin_size//1024)
    # 1. start mqtt broker sketch
    host_ip = get_my_ip()
    thread1 = Thread(target = mqqt_server_sketch, args = (host_ip,1883))
    thread1.start()
    # 2. start the dut test and wait till client gets IP address
    dut1.start_app()    
    # waiting for getting the IP address
    try:
        ip_address = dut1.expect(re.compile(r" sta ip: ([^,]+),"), timeout=30)
        print("Connected to AP with IP: {}".format(ip_address))
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')

    print ("writing to device: {}".format("mqtt://" + host_ip + "\n"))
    dut1.write("mqtt://" + host_ip + "\n")
    thread1.join()
    print ("Message id received from server: {}".format(msgid))
    # 3. check the message id was enqueued and then deleted
    msgid_enqueued = dut1.expect(re.compile(r"OUTBOX: ENQUEUE msgid=([0-9]+)"), timeout=30)
    msgid_deleted = dut1.expect(re.compile(r"OUTBOX: DELETED msgid=([0-9]+)"), timeout=30)
    # 4. check the msgid of received data are the same as that of enqueued and deleted from outbox
    if (msgid_enqueued[0] == str(msgid) and msgid_deleted[0] == str(msgid)):
        print("PASS: Received correct msg id")
    else:
        print("Failure!")
        raise ValueError('Mismatch of msgid: received: {}, enqueued {}, deleted {}'.format(msgid, msgid_enqueued, msgid_deleted))

if __name__ == '__main__':
    test_examples_protocol_mqtt_qos1()
