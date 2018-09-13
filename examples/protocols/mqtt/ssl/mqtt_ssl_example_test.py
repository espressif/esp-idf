import re
import os
import sys
import time
import socket
import imp
import ssl

use_mqtt_client_sketch = False

try:
    imp.find_module('paho')
    import paho.mqtt.client as mqtt
    # Make things with supposed existing module
except ImportError:
    use_mqtt_client_sketch = True
    pass

global g_recv_topic
global g_recv_data

g_recv_data=""

# This is only a workaround for running mqtt client with 'hardcoded' data using plain socket interface
def mqtt_client_sketch():
    global g_recv_topic
    global g_recv_data
    connect_msg = bytearray([0x10, 0x0c, 00, 0x04, 0x4d, 0x51, 0x54, 0x54, 0x04, 0x02, 00, 0x3c, 00, 00])
    send_qos0_msg = bytearray([ 0x30, 0x1a, 0x00, 0x0b, 0x2f, 0x74, 0x6f, 0x70, 0x69, 0x63, 0x2f, 0x71, 0x6f, 0x73, 0x30, 0x64, 0x61, 0x74, 0x61, 0x5f, 0x74, 0x6f, 0x5f, 0x65, 0x73, 0x70, 0x33, 0x32])
    subscribe_qos0 = bytearray([ 0x82, 0x10, 0x00, 0x01, 0x00, 0x0b, 0x2f, 0x74, 0x6f, 0x70, 0x69, 0x63, 0x2f, 0x71, 0x6f, 0x73, 0x30, 0x00] )
    client = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client.settimeout(30)
    cli = ssl.wrap_socket(client)
    cli.connect(("iot.eclipse.org", 8883))
    cli.send(connect_msg)
    data = cli.recv(1024)
    print("Connect ack received {}".format(data))
    cli.send(subscribe_qos0)
    data = cli.recv(1024)
    print("Subscibe ack received {}".format(data))
    start = time.time()
    while (time.time() - start) <= 20:
        data = cli.recv(1024)
        print("Data received {}".format(data[-17:]))
        if data[-15:] == "/topic/qos0data":
            g_recv_topic = data[-15:][:11]
            g_recv_data = data[-4:]
            cli.send(send_qos0_msg)
            data = cli.recv(1024)
            print("data ack received {}".format(data))
            break
    cli.close()

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("/topic/qos0")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    global g_recv_topic
    global g_recv_data
    if g_recv_data == "" and  msg.payload == "data":
        client.publish("/topic/qos0", "data_to_esp32")
        g_recv_topic = msg.topic
        g_recv_data = msg.payload
    print(msg.topic+" "+str(msg.payload))

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
def test_examples_protocol_mqtt_ssl(env, extra_data):
    global g_recv_topic
    global g_recv_data
    """
    steps: |
      1. join AP and connects to ssl broker
      2. Test connects a client to the same broker
      3. Test evaluates python client received correct qos0 message
      4. Test ESP32 client received correct qos0 message
    """
    dut1 = env.get_dut("mqtt_ssl", "examples/protocols/mqtt/ssl")
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, "mqtt_ssl.bin")
    bin_size = os.path.getsize(binary_file)
    IDF.log_performance("mqtt_ssl_bin_size", "{}KB".format(bin_size//1024))
    IDF.check_performance("mqtt_ssl_size", bin_size//1024)
    # 1. start test
    dut1.start_app()
    # 2. Test connects to a broker
    if use_mqtt_client_sketch:
        mqtt_client_sketch()
    else:
        client = mqtt.Client()
        client.on_connect = on_connect
        client.on_message = on_message
        client.tls_set(None,
                    None,
                    None, cert_reqs=ssl.CERT_NONE, tls_version=ssl.PROTOCOL_TLSv1, ciphers=None)
        client.tls_insecure_set(True)

        print "Connecting..."
        client.connect("iot.eclipse.org", 8883, 60)
        print "...done"
        print "Start Looping..."
        start = time.time()
        while (time.time() - start) <= 20:
            client.loop()
        print "...done"
    # 3. check the message received back from the server
    if g_recv_topic == "/topic/qos0" and g_recv_data == "data" :
        print("PASS: Received correct message")
    else:
        print("Failure!")
        raise ValueError('Wrong data received topic: {}, data:{}'.format(g_recv_topic, g_recv_data))
    # 4. check that the esp32 client received data sent by this python client
    dut1.expect(re.compile(r"DATA=data_to_esp32"), timeout=30)

if __name__ == '__main__':
    test_examples_protocol_mqtt_ssl()
