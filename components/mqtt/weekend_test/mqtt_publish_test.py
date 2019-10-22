from __future__ import print_function
from __future__ import unicode_literals
from builtins import str
import re
import os
import sys
import ssl
import paho.mqtt.client as mqtt
from threading import Thread, Event
import time
import string
import random

try:
    import IDF
    from IDF.IDFDUT import ESP32DUT
except ImportError:
    # this is a test case write with tiny-test-fw.
    # to run test cases outside tiny-test-fw,
    # we need to set environment variable `TEST_FW_PATH`,
    # then get and insert `TEST_FW_PATH` to sys path before import FW module
    test_fw_path = os.getenv("TEST_FW_PATH")
    if test_fw_path and test_fw_path not in sys.path:
        sys.path.insert(0, test_fw_path)
    import IDF

import DUT


event_client_connected = Event()
event_stop_client = Event()
event_client_received_correct = Event()
message_log = ""
broker_host = {}
broker_port = {}
expected_data = ""
subscribe_topic = ""
publish_topic = ""
expected_count = 0


# The callback for when the client receives a CONNACK response from the server.
def on_connect(client, userdata, flags, rc):
    print("Connected with result code " + str(rc))
    event_client_connected.set()
    client.subscribe("/topic/qos0")


def mqtt_client_task(client):
    while not event_stop_client.is_set():
        client.loop()


def get_host_port_from_dut(dut1, config_option):
    value = re.search(r'\:\/\/([^:]+)\:([0-9]+)', dut1.app.get_sdkconfig()[config_option])
    return value.group(1), int(value.group(2))


# The callback for when a PUBLISH message is received from the server.
def on_message(client, userdata, msg):
    global message_log
    global expected_count
    payload = msg.payload.decode()
    if payload == expected_data:
        expected_count += 1
        print("[{}] Received...".format(msg.mid))
    message_log += "Received data:" + msg.topic + " " + payload + "\n"


def test_single_config(dut, transport, qos, repeat, published):
    global expected_count
    global expected_data
    global message_log
    sample_string = ''.join(random.choice(string.ascii_uppercase + string.ascii_lowercase + string.digits) for _ in range(16))
    event_client_connected.clear()
    expected_count = 0
    message_log = ""
    expected_data = sample_string * repeat
    print("PUBLISH TEST: transport:{}, qos:{}, sequence:{}, sample msg:'{}'".format(transport, qos, published, expected_data))
    client = None
    try:
        if transport in ["ws", "wss"]:
            client = mqtt.Client(transport="websockets")
        else:
            client = mqtt.Client()
        client.on_connect = on_connect
        client.on_message = on_message
        if transport in ["ssl", "wss"]:
            client.tls_set(None, None, None, cert_reqs=ssl.CERT_NONE, tls_version=ssl.PROTOCOL_TLSv1_2, ciphers=None)
            client.tls_insecure_set(True)
        print("Connecting...")
        client.connect(broker_host[transport], broker_port[transport], 60)
    except Exception:
        print("ENV_TEST_FAILURE: Unexpected error while connecting to broker {}: {}:".format(broker_host[transport], sys.exc_info()[0]))
        raise
    # Starting a py-client in a separate thread
    thread1 = Thread(target=mqtt_client_task, args=(client,))
    thread1.start()
    print("Connecting py-client to broker {}:{}...".format(broker_host[transport], broker_port[transport]))
    if not event_client_connected.wait(timeout=30):
        raise ValueError("ENV_TEST_FAILURE: Test script cannot connect to broker: {}".format(broker_host[transport]))
    client.subscribe(subscribe_topic, qos)
    dut.write("{} {} {} {} {}".format(transport, sample_string, repeat, published, qos), eol="\n")
    try:
        # waiting till subscribed to defined topic
        dut.expect(re.compile(r"MQTT_EVENT_SUBSCRIBED"), timeout=30)
        for i in range(published):
            client.publish(publish_topic, sample_string * repeat, qos)
            print("Publishing...")
        print("Checking esp-client received msg published from py-client...")
        dut.expect(re.compile(r"Correct pattern received exactly x times"), timeout=60)
        start = time.time()
        while expected_count < published and time.time() - start <= 60:
            time.sleep(1)
        # Note: tolerate that messages qos=1 to be received more than once
        if expected_count == published or (expected_count > published and qos == 1):
            print("All data received from ESP32...")
        else:
            raise ValueError("Not all data received from ESP32: Expected:{}x{}, Received:{}x{}".format(expected_data, published, message_log, expected_count))
    finally:
        event_stop_client.set()
        thread1.join()
    client.disconnect()
    event_stop_client.clear()


@IDF.idf_example_test(env_tag="Example_WIFI")
def test_weekend_mqtt_publish(env, extra_data):
    # Using broker url dictionary for different transport
    global broker_host
    global broker_port
    global publish_topic
    global subscribe_topic
    """
    steps: |
      1. join AP and connects to ssl broker
      2. Test connects a client to the same broker
      3. Test evaluates python client received correct qos0 message
      4. Test ESP32 client received correct qos0 message
    """
    dut1 = env.get_dut("mqtt_publish", "examples/protocols/mqtt/publish_test", dut_class=ESP32DUT)
    # check and log bin size
    binary_file = os.path.join(dut1.app.binary_path, "mqtt_publish.bin")
    bin_size = os.path.getsize(binary_file)
    IDF.log_performance("mqtt_publish_bin_size", "{}KB"
                        .format(bin_size // 1024))
    IDF.check_performance("mqtt_publish_size", bin_size // 1024)
    # Look for host:port in sdkconfig
    try:
        # python client subscribes to the topic to which esp client publishes and vice versa
        publish_topic = dut1.app.get_sdkconfig()["CONFIG_EXAMPLE_SUBSCIBE_TOPIC"].replace('"','')
        subscribe_topic = dut1.app.get_sdkconfig()["CONFIG_EXAMPLE_PUBLISH_TOPIC"].replace('"','')
        broker_host["ssl"], broker_port["ssl"] = get_host_port_from_dut(dut1, "CONFIG_EXAMPLE_BROKER_SSL_URI")
        broker_host["tcp"], broker_port["tcp"] = get_host_port_from_dut(dut1, "CONFIG_EXAMPLE_BROKER_TCP_URI")
        broker_host["ws"], broker_port["ws"] = get_host_port_from_dut(dut1, "CONFIG_EXAMPLE_BROKER_WS_URI")
        broker_host["wss"], broker_port["wss"] = get_host_port_from_dut(dut1, "CONFIG_EXAMPLE_BROKER_WSS_URI")
    except Exception:
        print('ENV_TEST_FAILURE: Cannot find broker url in sdkconfig')
        raise
    dut1.start_app()
    try:
        ip_address = dut1.expect(re.compile(r" sta ip: ([^,]+),"), timeout=30)
        print("Connected to AP with IP: {}".format(ip_address))
    except DUT.ExpectTimeout:
        print('ENV_TEST_FAILURE: Cannot connect to AP')
        raise
    for qos in [0, 1, 2]:
        for transport in ["tcp", "ssl", "ws", "wss"]:
            # simple test with empty message
            test_single_config(dut1, transport, qos, 0, 5)
            # decide on broker what level of test will pass (local broker works the best)
            if broker_host[transport].startswith("192.168") and qos < 1:
                # medium size, medium repeated
                test_single_config(dut1, transport, qos, 5, 50)
                # long data
                test_single_config(dut1, transport, qos, 1000, 10)
                # short data, many repeats
                test_single_config(dut1, transport, qos, 2, 200)
            elif transport in ["ws", "wss"]:
                # more relaxed criteria for websockets!
                test_single_config(dut1, transport, qos, 2, 5)
                test_single_config(dut1, transport, qos, 50, 1)
                test_single_config(dut1, transport, qos, 10, 20)
            else:
                # common configuration should be good for most public mosquittos
                test_single_config(dut1, transport, qos, 5, 10)
                test_single_config(dut1, transport, qos, 500, 3)
                test_single_config(dut1, transport, qos, 1, 50)


if __name__ == '__main__':
    test_weekend_mqtt_publish()
