from __future__ import unicode_literals
import re
import os
import sys
import time
import socket
import imp
import ssl
import paho.mqtt.client as mqtt

g_recv_data=""
g_recv_topic=""
g_broker_connected=0

# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    global g_broker_connected
    print("Connected with result code "+str(rc))
    g_broker_connected = 1
    client.subscribe("/topic/qos0")

# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    global g_recv_topic
    global g_recv_data
    payload = msg.payload.decode()
    if g_recv_data == "" and  payload == "data":
        client.publish("/topic/qos0", "data_to_esp32")
        g_recv_topic = msg.topic
        g_recv_data = payload
    print(msg.topic+" "+str(payload))

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
def test_examples_protocol_mqtt_wss(env, extra_data):
    global g_recv_topic
    global g_recv_data
    global g_broker_connected
    broker_url="iot.eclipse.org"
    """
    steps: |
      1. join AP and connects to wss broker
      2. Test connects a client to the same broker
      3. Test evaluates it received correct qos0 message
      4. Test ESP32 client received correct qos0 message
    """
    dut1 = env.get_dut("mqtt_websocket_secure", "examples/protocols/mqtt/wss")
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, "mqtt_websocket_secure.bin")
    bin_size = os.path.getsize(binary_file)
    IDF.log_performance("mqtt_websocket_secure_bin_size", "{}KB".format(bin_size//1024))
    IDF.check_performance("mqtt_websocket_secure_size", bin_size//1024)
    # 1. start test (and check the environment is healthy)
    dut1.start_app()
    client = None
    # 2. Test connects to a broker
    try:
        ip_address = dut1.expect(re.compile(r" sta ip: ([^,]+),"), timeout=30)
        print("Connected to AP with IP: {}".format(ip_address))
        client = mqtt.Client(transport="websockets")
        client.on_connect = on_connect
        client.on_message = on_message
        client.tls_set(None,
                    None,
                    None, cert_reqs=ssl.CERT_NONE, tls_version=ssl.PROTOCOL_TLSv1, ciphers=None)
        print("Connecting...")
        client.connect(broker_url, 443, 60)
        print("...done")
    except DUT.ExpectTimeout:
        raise ValueError('ENV_TEST_FAILURE: Cannot connect to AP')
    except:
        print("ENV_TEST_FAILURE: Unexpected error while connecting to broker {}: {}:".format(broker_url, sys.exc_info()[0]))
        raise
    print("Start Looping...")
    start = time.time()
    while (time.time() - start) <= 20:
        client.loop()
    print("...done")
    if g_broker_connected == 0:
        raise ValueError('ENV_TEST_FAILURE: Test script cannot connect to broker: {}'.format(broker_url))
    # 3. check the message received back from the server
    if g_recv_topic == "/topic/qos0" and g_recv_data == "data" :
        print("PASS: Received correct message")
    else:
        print("Failure!")
        raise ValueError('Wrong data received topic: {}, data:{}'.format(g_recv_topic, g_recv_data))
    # 4. check that the esp32 client received data sent by this python client
    dut1.expect(re.compile(r"DATA=data_to_esp32"), timeout=30)

if __name__ == '__main__':
    test_examples_protocol_mqtt_wss()
